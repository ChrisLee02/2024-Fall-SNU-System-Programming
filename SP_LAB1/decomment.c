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

// 상태 전이 함수의 선언
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

// 액션 함수의 선언
void printToStdout(char input);
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

// 상태 전이와 액션을 처리하는 메인 함수 뼈대
State handleStateTransition(State currentState, char input) {
  State nextState;

  nextState = transitionState(currentState, input);
  executeAction(currentState, input);

  return nextState;
}

// 상태 전이 함수
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

// 액션 함수
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
      break;
  }
}

// 전이 및 액션 함수의 뼈대
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

void printToStdout(char input) { fprintf(stdout, "%c", input); }

// 액션 함수의 뼈대
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

void executeActionFromInsideSingleQuote(char input) { printToStdout(input); }

void executeActionFromInsideDoubleQuote(char input) { printToStdout(input); }

void executeActionFromEscapeSeqInSingleQuote(char input) {
  printToStdout(input);
}

void executeActionFromEscapeSeqInDoubleQuote(char input) {
  printToStdout(input);
}

void printErrorLog(int line_com) {
  fprintf(stderr, "Error: line %d: unterminated comment\n", line_com);
}

int main(void) {
  // This constant represents comment is closed so there's no comment line
  // number
  const int COMMENT_CLOSED = -1;
  // ich: int type variable to store character input from getchar()
  // (abbreviation of int character)
  int ich;
  // line_cur & line_com: current line number and comment line number
  // (abbreviation of current line and comment line)
  int line_cur, line_com;
  // ch: character that comes from casting (char) on ich (abbreviation of
  // character)
  char ch;

  line_cur = 1;
  line_com = COMMENT_CLOSED;
  State state = NORMAL_STATE;
  // This while loop reads all characters from standard input one by one
  while (1) {
    int got_eof = 0;

    ich = getchar();
    if (ich == EOF) break;

    ch = (char)ich;
    state = handleStateTransition(state, ich);
    // if state become INSIDE_MULTI_LINE_COMMENT first time, set line_com as
    // current line #
    if (state == INSIDE_MULTI_LINE_COMMENT && line_com == COMMENT_CLOSED) {
      line_com = line_cur;
    }
    // if state become NORMAL_STATE initialize line_com as -1
    if (state == NORMAL_STATE && line_com != COMMENT_CLOSED) {
      line_com = COMMENT_CLOSED;
    }

    if (ch == '\n') line_cur++;
    if (got_eof) break;
  }
  if (line_com != COMMENT_CLOSED) {
    printErrorLog(line_com);
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
