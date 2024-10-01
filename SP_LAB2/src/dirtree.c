//--------------------------------------------------------------------------------------------------
// System Programming                         I/O Lab Fall 2024
//
/// @file
/// @brief resursively traverse directory tree and list all entries
/// @author Lee, Ha Dong
/// @studid 2021-18641
/// @Assignment #: SP_LAB2
/// @File name: dirtree.c

//--------------------------------------------------------------------------------------------------

#define _GNU_SOURCE
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_DIR 64  ///< maximum number of supported directories

/// @brief output control flags
#define F_TREE 0x1     ///< enable tree view
#define F_SUMMARY 0x2  ///< enable summary
#define F_VERBOSE 0x4  ///< turn on verbose mode

/// @brief struct holding the summary
struct summary {
  unsigned int dirs;   ///< number of directories encountered
  unsigned int files;  ///< number of files
  unsigned int links;  ///< number of links
  unsigned int fifos;  ///< number of pipes
  unsigned int socks;  ///< number of sockets

  unsigned long long size;    ///< total size (in bytes)
  unsigned long long blocks;  ///< total number of blocks (512 byte blocks)
};

/// @brief abort the program with EXIT_FAILURE and an optional error message
///
/// @param msg optional error message or NULL
void panic(const char *msg) {
  if (msg) fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

/// @brief read next directory entry from open directory 'dir'. Ignores '.' and
/// '..' entries
///
/// @param dir open DIR* stream
/// @retval entry on success
/// @retval NULL on error or if there are no more entries
struct dirent *getNext(DIR *dir) {
  struct dirent *next;
  int ignore;

  do {
    errno = 0;
    next = readdir(dir);
    if (errno != 0) {
      perror(NULL);
    }
    ignore = next && ((strcmp(next->d_name, ".") == 0) ||
                      (strcmp(next->d_name, "..") == 0));
  } while (next && ignore);

  return next;
}

/// @brief qsort comparator to sort directory entries. Sorted by name,
/// directories first.
///
/// @param a pointer to first entry
/// @param b pointer to second entry
/// @retval -1 if a<b
/// @retval 0  if a==b
/// @retval 1  if a>b
static int dirent_compare(const void *a, const void *b) {
  // In this code, qsort sorts array of pointer(struct dirent **dirents)
  // so we need to convert a as ** and dereference it.
  struct dirent *e1 = *(struct dirent **)a;
  struct dirent *e2 = *(struct dirent **)b;

  // if one of the entries is a directory, it comes first
  if (e1->d_type != e2->d_type) {
    if (e1->d_type == DT_DIR) return -1;
    if (e2->d_type == DT_DIR) return 1;
  }

  // otherwise sorty by name
  return strcmp(e1->d_name, e2->d_name);
}

// update dstats' file count, by incrementing each file/folder's type.
void update_dstats(struct summary *dstats, struct dirent *entry) {
  switch (entry->d_type) {
    case DT_DIR:
      dstats->dirs++;
      break;
    case DT_REG:
      dstats->files++;
      break;
    case DT_LNK:
      dstats->links++;
      break;
    case DT_FIFO:
      dstats->fifos++;
      break;
    case DT_SOCK:
      dstats->socks++;
      break;
    default:
      break;
  }
}

// update dstats' meta data, by adding each file/folder's size and block size.
void update_dstats_metadata(struct summary *dstats,
                            const struct stat *statbuf) {
  dstats->size += statbuf->st_size;
  dstats->blocks += statbuf->st_blocks;
}

/// @brief read directory, save to dynamic array, and sort.
///
/// @param dir pointer of target directory
/// @param count pointer to save count of children of directory
/// @retval address of dynamic array of pointer to dirent
/// @retval if failed, return NULL
struct dirent **read_and_sort_directory(DIR *dir, int *count) {
  *count = 0;
  struct dirent *entry;

  while ((entry = getNext(dir)) != NULL) {
    (*count)++;
  }

  struct dirent **entries = malloc((*count) * sizeof(struct dirent *));
  if (entries == NULL) {
    return NULL;
  }

  rewinddir(dir);
  int index = 0;
  while ((entry = getNext(dir)) != NULL) {
    entries[index] = entry;
    index++;
  }

  qsort(entries, *count, sizeof(struct dirent *), dirent_compare);

  return entries;
}

/// @brief handle pstr according to flags and element's position
///         if failed, there's no allocation of memory.
///
/// @param name_with_pstr string to be copied for name with pstr
/// @param new_pstr string to be copied for next pstr
/// @param pstr prefix string printed in front of each entry
/// @param entry directory entry
/// @param flags output control flags (F_*)
/// @param is_last_element indicates this row is last or not
/// @retval 0 if succeed
/// @retval 1 if failed
int handle_pstr(char **name_with_pstr, char **new_pstr, const char *pstr,
                struct dirent *entry, unsigned int flags, int is_last_element) {
  char *double_white_space_prefix = "  ";
  char *stick_white_space_prefix = "| ";
  char *stick_dash_prefix = "|-";
  char *backtick_dash_prefix = "`-";
  char *prefix_chosen;

  // pstr for printing this row
  if (flags & F_TREE) {
    if (is_last_element) {
      prefix_chosen = backtick_dash_prefix;
    } else {
      prefix_chosen = stick_dash_prefix;
    }
  } else {
    prefix_chosen = double_white_space_prefix;
  }

  if (asprintf(name_with_pstr, "%s%s%s", pstr, prefix_chosen, entry->d_name) ==
      -1) {
    return -1;
  }

  // pstr for next step
  if (flags & F_TREE) {
    if (is_last_element) {
      prefix_chosen = double_white_space_prefix;
    } else {
      prefix_chosen = stick_white_space_prefix;
    }
  } else {
    prefix_chosen = double_white_space_prefix;
  }

  if (asprintf(new_pstr, "%s%s", pstr, prefix_chosen) == -1) {
    free(*name_with_pstr);
    return -1;
  }
  return 0;
}

// return character corresponding to each d_type.
char type_into_character(struct dirent *entry) {
  switch (entry->d_type) {
    case DT_DIR:
      return 'd';
    case DT_LNK:
      return 'l';
    case DT_CHR:
      return 'c';
    case DT_BLK:
      return 'b';
    case DT_FIFO:
      return 'f';
    case DT_SOCK:
      return 's';
    case DT_REG:
      return ' ';
    default:
      return ' ';
  }
}

// print one line with appropriate format according to flags
void print_row(const char *name_with_pstr, DIR *dir, struct dirent *entry,
               unsigned int flags, struct summary *dstats) {
  if (strlen(name_with_pstr) > 54) {
    printf("%.*s...", 51, name_with_pstr);
  } else {
    if (flags & F_VERBOSE)
      printf("%-54s", name_with_pstr);
    else
      printf("%s", name_with_pstr);
  }
  if (flags & F_VERBOSE) {
    struct stat sb;
    int dd = dirfd(dir);
    if (dd == -1) return;
    if (fstatat(dd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) < 0) {
      printf("No such file or directory");
    } else {
      struct passwd *pw = getpwuid(sb.st_uid);
      struct group *gr = getgrgid(sb.st_gid);

      char type = type_into_character(entry);

      printf("  %8.8s:%-8.8s  %10lld  %8lld  %1c", pw ? pw->pw_name : "unknown",
             gr ? gr->gr_name : "unknown", (long long)sb.st_size,
             (long long)sb.st_blocks, type);
      update_dstats_metadata(dstats, &sb);
    }
  }
  printf("\n");
}

/// @brief recursively process directory @a dn and print its tree
///
/// @param dn absolute or relative path string
/// @param pstr prefix string printed in front of each entry
/// @param stats pointer to statistics
/// @param flags output control flags (F_*)
void processDir_recursive(const char *dn, const char *pstr,
                          struct summary *dstats, unsigned int flags) {
  DIR *dir = opendir(dn);
  struct dirent *entry;
  int count = 0;

  if (dir == NULL) {
    if (errno == EACCES) {
      printf("%s  ERROR: Permission denied\n", pstr);
    }
    return;
  }

  struct dirent **entries = read_and_sort_directory(dir, &count);
  if (entries == NULL) {
    closedir(dir);
    return;
  }

  for (int i = 0; i < count; i++) {
    entry = entries[i];
    char *path;
    if (asprintf(&path, "%s/%s", dn, entry->d_name) == -1) {
      free(entries);
      closedir(dir);
      return;
    }

    char *name_with_pstr = NULL;
    char *new_pstr = NULL;

    if (handle_pstr(&name_with_pstr, &new_pstr, pstr, entry, flags,
                    (i == count - 1)) == -1) {
      free(entries);
      free(path);
      closedir(dir);
      return;
    }

    print_row(name_with_pstr, dir, entry, flags, dstats);

    update_dstats(dstats, entry);

    if (entry->d_type == DT_DIR) {
      processDir_recursive(path, new_pstr, dstats, flags);
    }
    free(path);
    free(new_pstr);
    free(name_with_pstr);
  }

  free(entries);
  closedir(dir);
}

// print header of each directory according to flags
void print_header(const char *dn, unsigned int flags) {
  if (flags & F_SUMMARY) {
    if (flags & F_VERBOSE) {
      printf("%-54s  %8s:%-8s  %10s  %8s %1s \n", "Name", "User", "Group",
             "Size", "Blocks", "Type");
    } else {
      printf("Name\n");
    }
    printf(
        "----------------------------------------------------------------------"
        "------------------------------\n");
  }

  printf("%s\n", dn);
}

// print footer of each directory according to flags
void print_footer(struct summary *dstats, unsigned int flags) {
  printf(
      "----------------------------------------------------------------------"
      "------------------------------\n");
  char *result;
  if (asprintf(&result,
               "%d file%s, %d director%s, %d link%s, %d pipe%s, and %d "
               "socket%s",
               dstats->files, dstats->files == 1 ? "" : "s", dstats->dirs,
               dstats->dirs == 1 ? "y" : "ies", dstats->links,
               dstats->links == 1 ? "" : "s", dstats->fifos,
               dstats->fifos == 1 ? "" : "s", dstats->socks,
               dstats->socks == 1 ? "" : "s") == -1) {
    return;
  }

  if (strlen(result) > 68) {
    printf("%.*s...", 65, result);
  } else {
    printf("%-68s", result);
  }

  if (flags & F_VERBOSE) {
    printf("   %14llu %9llu", dstats->size, dstats->blocks);
  }
  printf("\n\n");
  free(result);
}

// update tstats, by adding dstats
void update_tstats(struct summary *tstats, struct summary *dstats) {
  tstats->dirs += dstats->dirs;
  tstats->files += dstats->files;
  tstats->links += dstats->links;
  tstats->fifos += dstats->fifos;
  tstats->socks += dstats->socks;

  tstats->size += dstats->size;
  tstats->blocks += dstats->blocks;
}

/// @brief process directory's name with error handling and update tstat
///
/// @param dn absolute or relative path string
/// @param pstr prefix string printed in front of each entry
/// @param stats pointer to statistics
/// @param flags output control flags (F_*)
// print header of input folder and handle error
// if there's no error call recursive process function
void processDir(const char *dn, const char *pstr, struct summary *tstats,
                unsigned int flags, int ndir) {
  print_header(dn, flags);

  struct summary dstats;
  memset(&dstats, 0, sizeof(dstats));

  DIR *dir = opendir(dn);
  if (dir == NULL) {
    if (errno == EACCES) {
      printf("  ERROR: Permission denied\n");
    }
    return;
  }
  closedir(dir);

  processDir_recursive(dn, pstr, &dstats, flags);

  update_tstats(tstats, &dstats);
  if (flags & F_SUMMARY) {
    print_footer(&dstats, flags);
  }
}

/// @brief print program syntax and an optional error message. Aborts the
/// program with EXIT_FAILURE
///
/// @param argv0 command line argument 0 (executable)
/// @param error optional error (format) string (printf format) or NULL
/// @param ... parameter to the error format string
void syntax(const char *argv0, const char *error, ...) {
  if (error) {
    va_list ap;

    va_start(ap, error);
    vfprintf(stderr, error, ap);
    va_end(ap);

    printf("\n\n");
  }

  assert(argv0 != NULL);

  fprintf(stderr,
          "Usage %s [-t] [-s] [-v] [-h] [path...]\n"
          "Gather information about directory trees. If no path is given, the "
          "current directory\n"
          "is analyzed.\n"
          "\n"
          "Options:\n"
          " -t        print the directory tree (default if no other option "
          "specified)\n"
          " -s        print summary of directories (total number of files, "
          "total file size, etc)\n"
          " -v        print detailed information for each file. Turns on tree "
          "view.\n"
          " -h        print this help\n"
          " path...   list of space-separated paths (max %d). Default is the "
          "current directory.\n",
          basename(argv0), MAX_DIR);

  exit(EXIT_FAILURE);
}

/// @brief program entry point
int main(int argc, char *argv[]) {
  //
  // default directory is the current directory (".")
  //
  const char CURDIR[] = ".";
  const char *directories[MAX_DIR];
  int ndir = 0;

  struct summary tstat;
  unsigned int flags = 0;

  //
  // parse arguments
  //
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      // format: "-<flag>"
      if (!strcmp(argv[i], "-t"))
        flags |= F_TREE;
      else if (!strcmp(argv[i], "-s"))
        flags |= F_SUMMARY;
      else if (!strcmp(argv[i], "-v"))
        flags |= F_VERBOSE;
      else if (!strcmp(argv[i], "-h"))
        syntax(argv[0], NULL);
      else
        syntax(argv[0], "Unrecognized option '%s'.", argv[i]);
    } else {
      // anything else is recognized as a directory
      if (ndir < MAX_DIR) {
        directories[ndir++] = argv[i];
      } else {
        printf(
            "Warning: maximum number of directories exceeded, ignoring '%s'.\n",
            argv[i]);
      }
    }
  }
  // if no directory was specified, use the current directory
  if (ndir == 0) directories[ndir++] = CURDIR;
  //
  // process each directory
  //
  // TODO
  //
  // Pseudo-code
  // - reset statistics (tstat)
  // - loop over all entries in 'directories' (number of entires stored in
  // 'ndir')
  //   - reset statistics (dstat)
  //   - if F_SUMMARY flag set: print header
  //   - print directory name
  //   - call processDir() for the directory
  //   - if F_SUMMARY flag set: print summary & update statistics
  memset(&tstat, 0, sizeof(tstat));
  //...
  for (int i = 0; i < ndir; i++) {
    processDir(directories[i], "", &tstat, flags, ndir);
  }
  //
  // print grand total
  //
  if ((flags & F_SUMMARY) && (ndir > 1)) {
    printf(
        "Analyzed %d directories:\n"
        "  total # of files:        %16d\n"
        "  total # of directories:  %16d\n"
        "  total # of links:        %16d\n"
        "  total # of pipes:        %16d\n"
        "  total # of sockets:      %16d\n",
        ndir, tstat.files, tstat.dirs, tstat.links, tstat.fifos, tstat.socks);

    if (flags & F_VERBOSE) {
      printf(
          "  total file size:         %16llu\n"
          "  total # of blocks:       %16llu\n",
          tstat.size, tstat.blocks);
    }
  }

  //
  // that's all, folks!
  //
  return EXIT_SUCCESS;
}
