/*
  Name: Lee, Ha Dong
  Assignment number: SP_LAB1
  File name: decomment.c
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* This is skeleton code for reading characters from
standard input (e.g., a file or console input) one by one until
the end of the file (EOF) is reached. It keeps track of the current
line number and is designed to be extended with additional
functionality, such as processing or transforming the input data.
In this specific task, the goal is to implement logic that removes
C-style comments from the input. */

typedef enum State {
  NORMAL_STATE,
  SLASH_RECEIVE_INITIALLY,
  INSIDE_ONE_LINE_COMMENT,
  INSIDE_MULTI_LINE_COMMENT,
  ASTERISK_RECEIVED_INITIALLY_IN_COMMENT,
  INSIDE_SINGLE_QUOTE,
  INSIDE_DOUBLE_QUOTE,
  ESCAPE_SEQ_IN_SINGLE_QUOTE,
  ESCAPE_SEQ_IN_DOUBLE_QUOTE,
} State;

/* Declare transition and executeAction functions
to write functions from top level to down */

// Declare transition functions
State transitionState(State currentState, char input);
State transitionFromNormalState(char input);
State transitionFromSlashReceiveInitially(char input);
State transitionFromInsideOneLineComment(char input);
State transitionFromInsideMultiLineComment(char input);
State transitionFromAsteriskReceivedInitiallyInComment(char input);
State transitionFromInsideSingleQuote(char input);
State transitionFromInsideDoubleQuote(char input);
State transitionFromEscapeSeqInSingleQuote(char input);
State transitionFromEscapeSeqInDoubleQuote(char input);

// Declare executeAction functions
void executeAction(State currentState, char input);
void executeActionFromNormalState(char input);
void executeActionFromSlashReceiveInitially(char input);
void executeActionFromInsideOneLineComment(char input);
void executeActionFromInsideMultiLineComment(char input);
void executeActionFromAsteriskReceivedInitiallyInComment(char input);
void executeActionFromInsideSingleQuote(char input);
void executeActionFromInsideDoubleQuote(char input);
void executeActionFromEscapeSeqInSingleQuote(char input);
void executeActionFromEscapeSeqInDoubleQuote(char input);

/* Handles state transition and action execution for the current state.
   Calls the transition and action function for the state.
   Returns the next state based on the input and current state. */
State handleStateTransition(State currentState, char input) {
  State nextState = transitionState(currentState, input);
  executeAction(currentState, input);

  return nextState;
}

/* Determines the next state based on the current state and input.
   Calls the specific state transition function. */
State transitionState(State currentState, char input) {
  switch (currentState) {
    case NORMAL_STATE:
      return transitionFromNormalState(input);
    case SLASH_RECEIVE_INITIALLY:
      return transitionFromSlashReceiveInitially(input);
    case INSIDE_ONE_LINE_COMMENT:
      return transitionFromInsideOneLineComment(input);
    case INSIDE_MULTI_LINE_COMMENT:
      return transitionFromInsideMultiLineComment(input);
    case ASTERISK_RECEIVED_INITIALLY_IN_COMMENT:
      return transitionFromAsteriskReceivedInitiallyInComment(input);
    case INSIDE_SINGLE_QUOTE:
      return transitionFromInsideSingleQuote(input);
    case INSIDE_DOUBLE_QUOTE:
      return transitionFromInsideDoubleQuote(input);
    case ESCAPE_SEQ_IN_SINGLE_QUOTE:
      return transitionFromEscapeSeqInSingleQuote(input);
    case ESCAPE_SEQ_IN_DOUBLE_QUOTE:
      return transitionFromEscapeSeqInDoubleQuote(input);
    default:
      assert("currentState should be one of cases above");
      return NORMAL_STATE;
  }
}

/* Executes the appropriate action for the current state and input.
   Calls the specific action execution function for the state. */
void executeAction(State currentState, char input) {
  switch (currentState) {
    case NORMAL_STATE:
      executeActionFromNormalState(input);
      break;
    case SLASH_RECEIVE_INITIALLY:
      executeActionFromSlashReceiveInitially(input);
      break;
    case INSIDE_ONE_LINE_COMMENT:
      executeActionFromInsideOneLineComment(input);
      break;
    case INSIDE_MULTI_LINE_COMMENT:
      executeActionFromInsideMultiLineComment(input);
      break;
    case ASTERISK_RECEIVED_INITIALLY_IN_COMMENT:
      executeActionFromAsteriskReceivedInitiallyInComment(input);
      break;
    case INSIDE_SINGLE_QUOTE:
      executeActionFromInsideSingleQuote(input);
      break;
    case INSIDE_DOUBLE_QUOTE:
      executeActionFromInsideDoubleQuote(input);
      break;
    case ESCAPE_SEQ_IN_SINGLE_QUOTE:
      executeActionFromEscapeSeqInSingleQuote(input);
      break;
    case ESCAPE_SEQ_IN_DOUBLE_QUOTE:
      executeActionFromEscapeSeqInDoubleQuote(input);
      break;
    default:
      assert("currentState should be one of cases above");
      break;
  }
}

/* The following functions (transitionFromXXXState) implement the
   specific logic for state transitions.

   Each function corresponds to a particular state and determines the
   next state based on the current input.

   The functions handle different inputs by using if statements within
   each function.

   Refer to dfa.png for a visual representation of the transitions. */
State transitionFromNormalState(char input) {
  if (input == '/') return SLASH_RECEIVE_INITIALLY;
  if (input == '\'') return INSIDE_SINGLE_QUOTE;
  if (input == '\"') return INSIDE_DOUBLE_QUOTE;
  return NORMAL_STATE;
}

State transitionFromSlashReceiveInitially(char input) {
  if (input == '/') return INSIDE_ONE_LINE_COMMENT;
  if (input == '*') return INSIDE_MULTI_LINE_COMMENT;
  return NORMAL_STATE;
}

State transitionFromInsideOneLineComment(char input) {
  if (input == '\n') return NORMAL_STATE;
  return INSIDE_ONE_LINE_COMMENT;
}

State transitionFromInsideMultiLineComment(char input) {
  if (input == '*') return ASTERISK_RECEIVED_INITIALLY_IN_COMMENT;
  return INSIDE_MULTI_LINE_COMMENT;
}

State transitionFromAsteriskReceivedInitiallyInComment(char input) {
  if (input == '/') return NORMAL_STATE;
  if (input == '*') return ASTERISK_RECEIVED_INITIALLY_IN_COMMENT;

  return INSIDE_MULTI_LINE_COMMENT;
}

State transitionFromInsideSingleQuote(char input) {
  if (input == '\'') return NORMAL_STATE;
  if (input == '\\') return ESCAPE_SEQ_IN_SINGLE_QUOTE;
  return INSIDE_SINGLE_QUOTE;
}

State transitionFromInsideDoubleQuote(char input) {
  if (input == '\"') return NORMAL_STATE;
  if (input == '\\') return ESCAPE_SEQ_IN_DOUBLE_QUOTE;
  return INSIDE_DOUBLE_QUOTE;
}

State transitionFromEscapeSeqInSingleQuote(char input) {
  return INSIDE_SINGLE_QUOTE;
}

State transitionFromEscapeSeqInDoubleQuote(char input) {
  return INSIDE_DOUBLE_QUOTE;
}

/* simply wrapped fprintf to std output stream
   for abstraction */
void printToStdout(char input) {
  fprintf(stdout, "%c", input);
}

/* The following functions (executeActionFromXXXState) implement the
   specific logic for executing actions based on the current state.

   Each function corresponds to a particular state and performs actions
   based on the current input, such as printing characters.

   The functions handle different inputs by using if statements within
   each function.

   Refer to dfa.png for a visual representation of the state transitions
   and their corresponding actions.
*/
void executeActionFromNormalState(char input) {
  if (input == '/') return;
  printToStdout(input);
}

void executeActionFromSlashReceiveInitially(char input) {
  if (input == '/') {
    printToStdout(' ');
    return;
  }
  if (input == '*') {
    printToStdout(' ');
    return;
  }
  printToStdout('/');
  printToStdout(input);
}

void executeActionFromInsideOneLineComment(char input) {
  if (input == '\n') printToStdout('\n');
}

void executeActionFromInsideMultiLineComment(char input) {
  if (input == '\n') printToStdout('\n');
}

void executeActionFromAsteriskReceivedInitiallyInComment(char input) {
  if (input == '\n') printToStdout('\n');
}

void executeActionFromInsideSingleQuote(char input) {
  printToStdout(input);
}

void executeActionFromInsideDoubleQuote(char input) {
  printToStdout(input);
}

void executeActionFromEscapeSeqInSingleQuote(char input) {
  printToStdout(input);
}

void executeActionFromEscapeSeqInDoubleQuote(char input) {
  printToStdout(input);
}

/* simply wrapped fprintf to std error stream
   for abstraction */
void printErrorLog(int lineCom) {
  fprintf(stderr, "Error: line %d: unterminated comment\n", lineCom);
}

/* The main function reads characters from standard input one by one.

   It calls handleStateTransition to update the state and execute
   actions.

   Additionally, it manages the tracking of whether a comment is open or
   not for proper error handling. */
int main(void) {
  /* This constant for line comment represents that comment is closed */
  const int COMMENT_CLOSED = -1;

  /* ich: int type variable to store character input from getchar()
  (abbreviation of int character) */
  int ich;

  /* lineCur & lineCom: current line number and comment line number
  (abbreviation of current line and comment line) */
  int lineCur, lineCom;

  /*  ch: character that comes from casting (char) on ich (abbreviation
   of character) */
  char ch;

  lineCur = 1;
  lineCom = COMMENT_CLOSED;
  State state = NORMAL_STATE;

  // This while loop reads all characters from standard input one by one
  while (1) {
    ich = getchar();
    if (ich == EOF) break;

    ch = (char)ich;
    state = handleStateTransition(state, ich);

    /* if state become INSIDE_MULTI_LINE_COMMENT first time, set lineCom
    as current line # */
    if (state == INSIDE_MULTI_LINE_COMMENT &&
        lineCom == COMMENT_CLOSED) {
      lineCom = lineCur;
    }

    // if state become NORMAL_STATE initialize lineCom as -1
    if (state == NORMAL_STATE && lineCom != COMMENT_CLOSED) {
      lineCom = COMMENT_CLOSED;
    }

    if (ch == '\n') lineCur++;
  }

  if (lineCom != COMMENT_CLOSED) {
    printErrorLog(lineCom);
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
