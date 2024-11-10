#!/bin/bash

# 디렉터리 설정
TEST_DIR="self_check"
RESULTS_DIR="results"

# 결과 디렉터리 생성
mkdir -p $RESULTS_DIR

# 테스트 루프
for testcase in $TEST_DIR/*.txt; do
    test_name=$(basename "$testcase" .txt)
    
    # 샘플 바이너리 실행
    ./sample_snush < "$testcase" > "$RESULTS_DIR/${test_name}_sample_output.txt" 2> "$RESULTS_DIR/${test_name}_sample_error.txt"
    
    # 구현된 바이너리 실행
    ./snush < "$testcase" > "$RESULTS_DIR/${test_name}_my_output.txt" 2> "$RESULTS_DIR/${test_name}_my_error.txt"
    
    # 결과 비교
    diff -u "$RESULTS_DIR/${test_name}_sample_output.txt" "$RESULTS_DIR/${test_name}_my_output.txt" > "$RESULTS_DIR/${test_name}_diff_output.txt"
    diff -u "$RESULTS_DIR/${test_name}_sample_error.txt" "$RESULTS_DIR/${test_name}_my_error.txt" > "$RESULTS_DIR/${test_name}_diff_error.txt"
    
    # 차이 여부 출력
    if [[ -s "$RESULTS_DIR/${test_name}_diff_output.txt" || -s "$RESULTS_DIR/${test_name}_diff_error.txt" ]]; then
        echo "[FAIL] $test_name: 출력 차이 발견"
    else
        echo "[PASS] $test_name: 출력 일치"
    fi
done

# 결과 요약
echo
echo "테스트 결과는 '$RESULTS_DIR' 디렉토리에 저장되었습니다."
