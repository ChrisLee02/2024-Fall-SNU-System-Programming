https://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html

# Unix Shell 과제 진행 계획

## 1. 준비 단계

### 목표

과제의 전체 흐름을 이해하고 스켈레톤 코드를 파악.

### 작업 목록

- `README.md`와 과제 설명서를 읽고 **평가 항목**과 **주요 기능**(내장 명령, 리디렉션, 파이프, 백그라운드 실행)을 숙지.
- **시스템 호출**(`fork`, `execvp`, `waitpid`, `dup2`, `pipe`)을 학습.
- 스켈레톤 코드의 파일별 역할과 구조 파악.

---

## 2. 주요 기능 구현

### A. 기본 명령어 입력 및 처리

- **목표**: `%` 프롬프트 표시, 명령어 입력 처리.
- **수정 파일**: `snush.c`
  - `main()`: 프롬프트 출력 및 입력 루프.
  - `shell_helper()`: 명령어 입력 -> 파싱 -> 실행 흐름 구현.

### B. 명령어 분석 (토큰화 및 구문 분석)

- **목표**: 명령어를 토큰화하고 구문을 분석.
- **수정 파일**: `lexsyn.c`
  - `lex_line()`: 문자열을 토큰으로 분리.
  - `syntax_check()`: 구문 오류 확인.

### C. 내장 명령어 구현 (`cd`, `exit`)

- **목표**: 디렉토리 변경(`cd`), 프로그램 종료(`exit`).
- **수정 파일**: `util.c`, `execute.c`
  - `check_builtin()`: 명령어 구분.
  - `execute_builtin()`:
    - `cd`는 `chdir()` 호출.
    - `exit`은 `exit(0)` 호출.

---

## 3. 입출력 리디렉션

### A. 입력 리디렉션 (`<`)

- **목표**: 파일에서 표준 입력 읽기.
- **수정 파일**: `execute.c`
  - `redin_handler()`: `open` 및 `dup2`로 `stdin` 재설정.

### B. 출력 리디렉션 (`>`)

- **목표**: 출력 파일로 표준 출력 저장.
- **수정 파일**: `execute.c`
  - `redout_handler()`: `creat` 및 `dup2`로 `stdout` 재설정.

---

## 4. 파이프

### A. 단일 파이프 (`|`)

- **목표**: 두 명령어를 파이프로 연결.
- **수정 파일**: `execute.c`
  - `fork_exec()`: `pipe()`와 `dup2`로 데이터 흐름 설정.

### B. 다중 파이프

- **목표**: 여러 명령어를 파이프 체인으로 연결.
- **참조 파일**: `token.c`
  - `count_pipe()`: 파이프 개수 파악.

---

## 5. 백그라운드 프로세스

### 목표

`&` 명령어를 백그라운드에서 실행.

### 수정 파일

- **`token.c`**: `check_bg()`로 백그라운드 여부 확인.
- **`snush.c`**:
  - `fork_exec()`에서 `WNOHANG`으로 비차단 대기.
  - `sigzombie_handler()`로 종료된 자식 프로세스 정리.

---

## 6. 신호 처리

### 목표

`SIGINT` 신호를 부모는 무시, 자식은 처리.

### 수정 파일

- **`snush.c`**:
  - `sigaction()`으로 부모가 SIGINT 무시 설정.

---

## 7. 디버깅 및 테스트

### A. 디버깅

- `DEBUG=1` 활성화로 에러 메시지 확인.

### B. 테스트

- 단위 테스트: 리디렉션, 파이프, 백그라운드 실행 등 기능별 테스트 케이스 작성.
- 전체 시스템 테스트: 오류 처리와 크래시 방지 확인.

---

이 구조에 따라 단계별로 진행하면 혼란 없이 효율적으로 과제를 완성할 수 있습니다. 필요한 부분은 언제든 요청하세요!
