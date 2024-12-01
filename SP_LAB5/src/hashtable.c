/*---------------------------------------------------------------------------*/
/* hashtable.c                                                               */
/* Author: Junghan Yoon, KyoungSoo Park                                      */
/* Modified by: (Your Name)                                                  */
/*---------------------------------------------------------------------------*/
#include "hashtable.h"
/*---------------------------------------------------------------------------*/
int hash(const char *key, size_t hash_size) {
  TRACE_PRINT();
  unsigned int hash = 0;
  while (*key) {
    hash = (hash << 5) + *key++;
  }

  return hash % hash_size;
}
/*---------------------------------------------------------------------------*/
hashtable_t *hash_init(size_t hash_size, int delay) {
  TRACE_PRINT();
  int i, j, ret;
  hashtable_t *table = malloc(sizeof(hashtable_t));

  if (table == NULL) {
    DEBUG_PRINT("Failed to allocate memory for hash table");
    return NULL;
  }

  table->hash_size = hash_size;
  table->total_entries = 0;

  table->buckets = malloc(hash_size * sizeof(node_t *));
  if (table->buckets == NULL) {
    DEBUG_PRINT("Failed to allocate memory for hash table buckets");
    free(table);
    return NULL;
  }

  table->locks = malloc(hash_size * sizeof(rwlock_t));
  if (table->locks == NULL) {
    DEBUG_PRINT("Failed to allocate memory for hash table locks");
    free(table->buckets);
    free(table);
    return NULL;
  }

  table->bucket_sizes = malloc(hash_size * sizeof(*table->bucket_sizes));
  if (table->bucket_sizes == NULL) {
    DEBUG_PRINT("Failed to allocate memory for hash table bucket sizes");
    free(table->buckets);
    free(table->locks);
    free(table);
    return NULL;
  }

  for (i = 0; i < hash_size; i++) {
    table->buckets[i] = NULL;
    table->bucket_sizes[i] = 0;
    ret = rwlock_init(&table->locks[i], delay);
    if (ret != 0) {
      DEBUG_PRINT("Failed to initialize read-write lock");
      for (j = 0; j < i; j++) {
        rwlock_destroy(&table->locks[j]);
      }
      free(table->buckets);
      free(table->locks);
      free(table->bucket_sizes);
      free(table);
      return NULL;
    }
  }

  return table;
}
/*---------------------------------------------------------------------------*/
int hash_destroy(hashtable_t *table) {
  TRACE_PRINT();
  node_t *node, *tmp;
  int i;

  for (i = 0; i < table->hash_size; i++) {
    node = table->buckets[i];
    while (node) {
      tmp = node;
      node = node->next;
      free(tmp->key);
      free(tmp->value);
      free(tmp);
    }
    if (rwlock_destroy(&table->locks[i]) != 0) {
      DEBUG_PRINT("Failed to destroy read-write lock");
      return -1;
    }
  }

  free(table->buckets);
  free(table->locks);
  free(table->bucket_sizes);
  free(table);

  return 0;
}
/*---------------------------------------------------------------------------*/
// list utility function
node_t *list_search(node_t *head, const char *key) {
  node_t *current = head;
  while (current) {
    if (strcmp(current->key, key) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
int list_insert(node_t **head, const char *key, const char *value) {
  if (list_search(*head, key) != NULL) {
    return 0;
  }

  node_t *new_node = malloc(sizeof(node_t));
  if (!new_node) {
    return -1;
  }

  new_node->key = strdup(key);
  if (!new_node->key) {
    free(new_node);
    return -1;
  }

  new_node->value = strdup(value);
  if (!new_node->value) {
    free(new_node->key);
    free(new_node);
    return -1;
  }

  new_node->next = *head;
  *head = new_node;

  return 1;
}

/*---------------------------------------------------------------------------*/
int list_update(node_t *head, const char *key, const char *value) {
  node_t *target = list_search(head, key);
  if (!target) {
    return 0;
  }

  char *new_value = strdup(value);
  if (!new_value) {
    return -1;
  }

  free(target->value);
  target->value = new_value;

  return 1;
}

int list_delete(node_t **head, const char *key) {
  node_t *current = *head;
  node_t *prev = NULL;

  while (current) {
    if (strcmp(current->key, key) == 0) {
      if (prev) {
        prev->next = current->next;
      } else {
        *head = current->next;
      }

      free(current->key);
      free(current->value);
      free(current);
      return 1;
    }
    prev = current;
    current = current->next;
  }

  return 0;
}

/*---------------------------------------------------------------------------*/
int hash_insert(hashtable_t *table, const char *key, const char *value) {
  TRACE_PRINT();
  node_t *node;
  rwlock_t *lock;
  unsigned int index = hash(key, table->hash_size);

  /*---------------------------------------------------------------------------*/
  /* edit here */
  /**
   * inserts a key-value pair to the hash table.
   * returns -1 when any internal errors occur.
   * returns 1 when successfully inserted.
   * returns 0 when the key already exists. (collision)
   */
  node_t *head = table->buckets[index];
  lock = &table->locks[index];
  if (rwlock_write_lock(lock) == -1) {
    return -1;
  }

  int result = list_insert(&head, key, value);
  if (result == 1) {
    table->buckets[index] = head;
    table->bucket_sizes[index]++;
    table->total_entries++;
  }

  if (rwlock_write_unlock(lock) == -1) {
    return -1;
  }
  /*---------------------------------------------------------------------------*/

  return result;
}
/*---------------------------------------------------------------------------*/
int hash_search(hashtable_t *table, const char *key, const char **value) {
  TRACE_PRINT();
  node_t *node;
  rwlock_t *lock;
  unsigned int index = hash(key, table->hash_size);

  /*---------------------------------------------------------------------------*/
  /* edit here */
  /**
   * searches a key-value pair in the hash table,
   * and modify the given value pointer to point found value.
   * returns -1 when any internal errors occur.
   * returns 1 when successfully found.
   * returns 0 when there is no such key found.
   */
  node_t *head = table->buckets[index];
  lock = &table->locks[index];
  if (rwlock_read_lock(lock) == -1) {
    return -1;
  }

  node = list_search(head, key);

  if (node == NULL) {
    if (rwlock_read_unlock(lock) == -1) {
      return -1;
    }
    return 0;
  }

  // for thread-safe return, dup str
  *value = strdup(node->value);
  if (*value == NULL) {
    rwlock_read_unlock(lock);
    return -1;
  }

  if (rwlock_read_unlock(lock) == -1) {
    free(*value);
    return -1;
  }
  /*---------------------------------------------------------------------------*/

  return 1;
}
/*---------------------------------------------------------------------------*/
int hash_update(hashtable_t *table, const char *key, const char *value) {
  TRACE_PRINT();
  rwlock_t *lock;
  unsigned int index = hash(key, table->hash_size);

  /*---------------------------------------------------------------------------*/
  /* edit here */
  /**
   * updates a key-value pair in the hash table.
   * returns -1 when any internal errors occur.
   * returns 1 when successfully updated.
   * returns 0 when there is no such key found.
   */
  node_t *head = table->buckets[index];
  lock = &table->locks[index];

  if (rwlock_write_lock(lock) == -1) {
    return -1;
  }

  int result = list_update(head, key, value);

  if (rwlock_write_unlock(lock) == -1) {
    return -1;
  }
  /*---------------------------------------------------------------------------*/

  return result;
}
/*---------------------------------------------------------------------------*/
int hash_delete(hashtable_t *table, const char *key) {
  TRACE_PRINT();
  node_t *node, *prev;
  rwlock_t *lock;
  unsigned int index = hash(key, table->hash_size);

  /*---------------------------------------------------------------------------*/
  /* edit here */
  /**
   * deletes a key-value pair from the hash table.
   * returns -1 when any internal errors occur.
   * returns 1 when successfully deleted.
   * returns 0 when there is no such key found.
   */
  node_t *head = table->buckets[index];
  lock = &table->locks[index];

  if (rwlock_write_lock(lock) == -1) {
    return -1;
  }

  int result = list_delete(&head, key);

  if (result == 1) {
    table->buckets[index] = head;
    table->bucket_sizes[index]--;
    table->total_entries--;
  }

  if (rwlock_write_unlock(lock) == -1) {
    return -1;
  }

  /*---------------------------------------------------------------------------*/
  return result;
}
/*---------------------------------------------------------------------------*/
/* function to dump the contents of the hash table, including locks status */
void hash_dump(hashtable_t *table) {
  TRACE_PRINT();
  node_t *node;
  int i;

  printf("[Hash Table Dump]");
  printf("Total Entries: %ld\n", table->total_entries);

  for (i = 0; i < table->hash_size; i++) {
    if (!table->bucket_sizes[i]) {
      continue;
    }
    printf("Bucket %d: %ld entries\n", i, table->bucket_sizes[i]);
    printf("  Lock State -> Read Count: %d, Write Count: %d\n",
           table->locks[i].read_count, table->locks[i].write_count);
    node = table->buckets[i];
    while (node) {
      printf(
          "    Key:   %s\n"
          "    Value: %s\n",
          node->key, node->value);
      node = node->next;
    }
  }
  printf("End of Dump\n");
}