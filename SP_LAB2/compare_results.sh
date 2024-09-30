#!/bin/bash

# 출제자의 정답 파일과 내 파일 위치
REFERENCE_EXEC="./reference/dirtree"
MY_EXEC="./bin/dirtree"

# 결과를 저장할 디렉토리
ANS_DIR="./ans"
MY_RESULTS_DIR="./my_results"
DIFF_DIR="./diffs"  # 차이점을 저장할 디렉토리
DIFF_RESULTS="./diffs.txt"  # 모든 diff 결과를 저장할 파일
mkdir -p $MY_RESULTS_DIR
mkdir -p $DIFF_DIR  # 차이점 저장 디렉토리 생성
> $DIFF_RESULTS  # 기존 diffs.txt 파일 덮어쓰기

# 테스트 케이스 폴더들
TEST_CASES_DIR="./test_case"

# 모든 옵션 조합
OPTIONS_LIST=("" "-t" "-v" "-s" "-t -v" "-t -s" "-v -s" "-t -v -s")

# 테스트 케이스 디렉토리 모음
ALL_TEST_DIRS=()
for TEST_DIR in $(find $TEST_CASES_DIR -mindepth 1 -maxdepth 1 -type d); do
    ALL_TEST_DIRS+=("$TEST_DIR")
done

echo "Running tests..."

# 각 옵션 조합에 대해 테스트 케이스 실행 및 비교
for OPTIONS in "${OPTIONS_LIST[@]}"; do
    # 1. 개별 테스트 케이스 실행 및 비교
    for TEST_DIR in "${ALL_TEST_DIRS[@]}"; do
        TEST_NAME=$(basename $TEST_DIR)
        
        REFERENCE_OUTPUT="$ANS_DIR/${TEST_NAME}_ans_${OPTIONS// /_}.txt"
        MY_OUTPUT="$MY_RESULTS_DIR/${TEST_NAME}_my_${OPTIONS// /_}.txt"
        
        $MY_EXEC $OPTIONS $TEST_DIR > $MY_OUTPUT  # 내 파일 실행
        
        # diff로 결과 비교
        diff $REFERENCE_OUTPUT $MY_OUTPUT > /dev/null
        if [ $? -ne 0 ]; then
            DIFF_FILE="$DIFF_DIR/${TEST_NAME}_diff_${OPTIONS// /_}.txt"
            diff $REFERENCE_OUTPUT $MY_OUTPUT > $DIFF_FILE
            diff $REFERENCE_OUTPUT $MY_OUTPUT >> $DIFF_RESULTS
        fi
    done

    # 2. 여러 테스트 케이스를 2~3개씩 조합해서 실행 및 비교
    for ((i = 0; i < ${#ALL_TEST_DIRS[@]}; i++)); do
        for ((j = i + 1; j < ${#ALL_TEST_DIRS[@]}; j++)); do
            # 2개 조합
            REFERENCE_OUTPUT="$ANS_DIR/combination_${i}_${j}_ans_${OPTIONS// /_}.txt"
            MY_OUTPUT="$MY_RESULTS_DIR/combination_${i}_${j}_my_${OPTIONS// /_}.txt"
            
            $MY_EXEC $OPTIONS "${ALL_TEST_DIRS[i]}" "${ALL_TEST_DIRS[j]}" > $MY_OUTPUT

            # diff로 결과 비교
            diff $REFERENCE_OUTPUT $MY_OUTPUT > /dev/null
            if [ $? -ne 0 ]; then
                DIFF_FILE="$DIFF_DIR/combination_${i}_${j}_diff_${OPTIONS// /_}.txt"
                diff $REFERENCE_OUTPUT $MY_OUTPUT > $DIFF_FILE
                diff $REFERENCE_OUTPUT $MY_OUTPUT >> $DIFF_RESULTS
            fi

            # 3개 조합
            for ((k = j + 1; k < ${#ALL_TEST_DIRS[@]}; k++)); do
                REFERENCE_OUTPUT="$ANS_DIR/combination_${i}_${j}_${k}_ans_${OPTIONS// /_}.txt"
                MY_OUTPUT="$MY_RESULTS_DIR/combination_${i}_${j}_${k}_my_${OPTIONS// /_}.txt"
                
                $MY_EXEC $OPTIONS "${ALL_TEST_DIRS[i]}" "${ALL_TEST_DIRS[j]}" "${ALL_TEST_DIRS[k]}" > $MY_OUTPUT

                # diff로 결과 비교
                diff $REFERENCE_OUTPUT $MY_OUTPUT > /dev/null
                if [ $? -ne 0 ]; then
                    DIFF_FILE="$DIFF_DIR/combination_${i}_${j}_${k}_diff_${OPTIONS// /_}.txt"
                    diff $REFERENCE_OUTPUT $MY_OUTPUT > $DIFF_FILE
                    diff $REFERENCE_OUTPUT $MY_OUTPUT >> $DIFF_RESULTS
                fi
            done
        done
    done
    echo -n "."
done

echo -e "\nComparison complete. Check $DIFF_DIR for detailed differences and $DIFF_RESULTS for overall results."
