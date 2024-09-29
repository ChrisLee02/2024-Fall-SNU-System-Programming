#!/bin/bash

# 테스트 디렉토리 생성
mkdir -p test_case/tlqkf

# 실제 파일 생성
touch test_case/tlqkf/valid_file

# 권한을 없애서 접근을 막음 (이렇게 하면 stat 호출 시 에러 발생)
chmod 000 test_case/tlqkf/valid_file

# 결과 확인
echo "Created test_case/tlqkf with:"
echo "- valid_file (file with no access permissions)"
