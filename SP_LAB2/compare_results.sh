#!/bin/bash

# 출제자의 정답 파일과 내 파일 위치
REFERENCE_EXEC="./reference/dirtree"
MY_EXEC="./bin/dirtree"

# 결과를 저장할 디렉토리
ANS_DIR="./ans"
MY_RESULTS_DIR="./my_results"
mkdir -p $MY_RESULTS_DIR

# 테스트 케이스 폴더들
TEST_CASES="./test_case/test1 ./test_case/test2 ./test_case/test3"

# 차이점을 저장할 파일
DIFF_RESULTS="./diff_results.txt"
> $DIFF_RESULTS  # 기존 diff 결과를 덮어씀

# 각 테스트 케이스에 대해 실행 및 비교
for TEST in $TEST_CASES; do
    TEST_NAME=$(basename $TEST)  # test1, test2, test3
    
    REFERENCE_OUTPUT="$ANS_DIR/${TEST_NAME}_ans.txt"
    MY_OUTPUT="$MY_RESULTS_DIR/${TEST_NAME}_my.txt"
    
    echo "Running test case: $TEST with my executable"
    
    # 내 실행 파일 실행 후 결과 저장
    $MY_EXEC $TEST > $MY_OUTPUT
    
    # diff로 결과 비교
    diff $REFERENCE_OUTPUT $MY_OUTPUT > /dev/null
    if [ $? -eq 0 ]; then
        echo "$TEST_NAME: PASS"
    else
        echo "$TEST_NAME: FAIL" >> $DIFF_RESULTS
        echo "Differences found for $TEST_NAME. See below:" >> $DIFF_RESULTS
        diff $REFERENCE_OUTPUT $MY_OUTPUT >> $DIFF_RESULTS
    fi
done

echo "Comparison complete. Check $DIFF_RESULTS for failures and differences."
  