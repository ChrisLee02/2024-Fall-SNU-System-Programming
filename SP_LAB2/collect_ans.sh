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

# 테스트 케이스 디렉토리 모음
ALL_TEST_DIRS=()
for TEST_DIR in $(find $TEST_CASES_DIR -mindepth 1 -maxdepth 1 -type d); do
    ALL_TEST_DIRS+=("$TEST_DIR")
done

# 각 옵션 조합에 대해 테스트 케이스 실행
for OPTIONS in "${OPTIONS_LIST[@]}"; do
    echo "Running with options: $OPTIONS"

    # 1. 개별 테스트 케이스 실행
    for TEST_DIR in "${ALL_TEST_DIRS[@]}"; do
        TEST_NAME=$(basename $TEST_DIR)  # test1, test2 등 폴더 이름 추출
        OUTPUT_FILE="$ANS_DIR/${TEST_NAME}_ans_${OPTIONS// /_}.txt"  # 옵션명을 파일명에 포함

        echo "Running test case: $TEST_DIR with reference executable and options: $OPTIONS"
        
        # 출제자의 실행 파일 실행 후 결과 저장
        $REFERENCE_EXEC $OPTIONS $TEST_DIR > $OUTPUT_FILE
        
        echo "Saved result to $OUTPUT_FILE"
    done

    # 2. 여러 테스트 케이스를 2~3개씩 조합해서 실행
    for ((i = 0; i < ${#ALL_TEST_DIRS[@]}; i++)); do
        for ((j = i + 1; j < ${#ALL_TEST_DIRS[@]}; j++)); do
            # 2개의 테스트 폴더 조합
            OUTPUT_FILE="$ANS_DIR/combination_${i}_${j}_ans_${OPTIONS// /_}.txt"
            echo "Running combination of: ${ALL_TEST_DIRS[i]} and ${ALL_TEST_DIRS[j]}"
            
            # 두 폴더를 argv로 넘겨서 실행
            $REFERENCE_EXEC $OPTIONS "${ALL_TEST_DIRS[i]}" "${ALL_TEST_DIRS[j]}" > $OUTPUT_FILE
            
            echo "Saved combination result to $OUTPUT_FILE"
            
            # 3개까지 조합
            for ((k = j + 1; k < ${#ALL_TEST_DIRS[@]}; k++)); do
                OUTPUT_FILE="$ANS_DIR/combination_${i}_${j}_${k}_ans_${OPTIONS// /_}.txt"
                echo "Running combination of: ${ALL_TEST_DIRS[i]}, ${ALL_TEST_DIRS[j]}, and ${ALL_TEST_DIRS[k]}"
                
                # 세 폴더를 argv로 넘겨서 실행
                $REFERENCE_EXEC $OPTIONS "${ALL_TEST_DIRS[i]}" "${ALL_TEST_DIRS[j]}" "${ALL_TEST_DIRS[k]}" > $OUTPUT_FILE
                
                echo "Saved combination result to $OUTPUT_FILE"
            done
        done
    done
done

echo "All reference results have been collected for all option combinations."
