#!/bin/bash

# 출제자의 정답 파일과 내 파일 위치
REFERENCE_EXEC="./reference/dirtree"
MY_EXEC="./bin/dirtree"

# 결과를 저장할 디렉토리
ANS_DIR="./ans"
MY_RESULTS_DIR="./my_results"
mkdir -p $MY_RESULTS_DIR

# 테스트 케이스 폴더들
TEST_CASES_DIR="./test_case"

# 차이점을 저장할 파일
DIFF_RESULTS="./diff_results.txt"
> $DIFF_RESULTS  # 기존 diff 결과를 덮어씀

# 모든 옵션 조합
OPTIONS_LIST=("" "-t" "-v" "-s" "-t -v" "-t -s" "-v -s" "-t -v -s")
# OPTIONS_LIST=("")

# 각 옵션 조합에 대해 테스트 케이스 실행 및 비교
for OPTIONS in "${OPTIONS_LIST[@]}"; do
    echo "Comparing results with options: $OPTIONS"
    
    # test_case 디렉토리 아래에 있는 모든 폴더를 순회
    for TEST_DIR in $(find $TEST_CASES_DIR -mindepth 1 -maxdepth 1 -type d); do
        TEST_NAME=$(basename $TEST_DIR)  # test1, test2 등 폴더 이름 추출
        
        REFERENCE_OUTPUT="$ANS_DIR/${TEST_NAME}_ans_${OPTIONS// /_}.txt"
        MY_OUTPUT="$MY_RESULTS_DIR/${TEST_NAME}_my_${OPTIONS// /_}.txt"
        
        echo "Running test case: $TEST_DIR with my executable and options: $OPTIONS"
        
        # 내 실행 파일 실행 후 결과 저장 (옵션을 함께 전달)
        $MY_EXEC $OPTIONS $TEST_DIR > $MY_OUTPUT
        
        # diff로 결과 비교
        diff $REFERENCE_OUTPUT $MY_OUTPUT > /dev/null
        if [ $? -eq 0 ]; then
            echo "$TEST_NAME with options '$OPTIONS': PASS"
        else
            echo "$TEST_NAME with options '$OPTIONS': FAIL" >> $DIFF_RESULTS
            echo "Differences found for $TEST_NAME with options '$OPTIONS'. See below:" >> $DIFF_RESULTS
            diff $REFERENCE_OUTPUT $MY_OUTPUT >> $DIFF_RESULTS
        fi
    done
done

echo "Comparison complete. Check $DIFF_RESULTS for failures and differences."
