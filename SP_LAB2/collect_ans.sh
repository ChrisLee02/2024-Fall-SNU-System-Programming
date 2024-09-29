#!/bin/bash

# 출제자의 정답 파일 위치
REFERENCE_EXEC="./reference/dirtree"

# 결과를 저장할 디렉토리
ANS_DIR="./ans"
mkdir -p $ANS_DIR

# 테스트 케이스 폴더들
TEST_CASES_DIR="./test_case"

# 모든 옵션 조합
OPTIONS_LIST=("" "-t" "-v" "-s" "-t -v" "-t -s" "-v -s" "-t -v -s")
# OPTIONS_LIST=("" "-t" "-v" "-s")

# 각 옵션 조합에 대해 테스트 케이스 실행
for OPTIONS in "${OPTIONS_LIST[@]}"; do
    echo "Running with options: $OPTIONS"
    
    # test_case 디렉토리 아래에 있는 모든 폴더를 순회
    for TEST_DIR in $(find $TEST_CASES_DIR -mindepth 1 -maxdepth 1 -type d); do
        TEST_NAME=$(basename $TEST_DIR)  # test1, test2 등 폴더 이름 추출
        OUTPUT_FILE="$ANS_DIR/${TEST_NAME}_ans_${OPTIONS// /_}.txt"  # 옵션명을 파일명에 포함
        
        echo "Running test case: $TEST_DIR with reference executable and options: $OPTIONS"
        
        # 출제자의 실행 파일 실행 후 결과 저장 (옵션을 함께 전달)
        $REFERENCE_EXEC $OPTIONS $TEST_DIR > $OUTPUT_FILE
        
        echo "Saved result to $OUTPUT_FILE"
    done
done

echo "All reference results have been collected for all option combinations."
