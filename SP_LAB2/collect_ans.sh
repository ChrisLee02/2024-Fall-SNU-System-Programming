#!/bin/bash

# 출제자의 정답 파일 위치
REFERENCE_EXEC="./reference/dirtree"

# 결과를 저장할 디렉토리
ANS_DIR="./ans"
mkdir -p $ANS_DIR

# 테스트 케이스 폴더들
TEST_CASES="./test_case/test1 ./test_case/test2 ./test_case/test3"

# 각 테스트 케이스에 대해 실행
for TEST in $TEST_CASES; do
    TEST_NAME=$(basename $TEST)  # test1, test2, test3 등을 얻기 위해 basename 사용
    OUTPUT_FILE="$ANS_DIR/${TEST_NAME}_ans.txt"
    
    echo "Running test case: $TEST with reference executable"
    
    # 출제자의 실행 파일 실행 후 결과 저장
    $REFERENCE_EXEC $TEST > $OUTPUT_FILE
    
    echo "Saved result to $OUTPUT_FILE"
done

echo "All reference results have been collected."
