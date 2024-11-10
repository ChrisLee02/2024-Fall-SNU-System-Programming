#!/bin/bash

# 테스트 디렉토리 생성
mkdir -p self_check

# 테스트 케이스 작성
cat <<EOF > self_check/test_nonexistent_input.txt
echo "# TEST: Nonexistent input file"
wc -l < nonexistent_file
echo "# END TEST"
EOF

cat <<EOF > self_check/test_multiple_input_redirection.txt
echo "# TEST: Multiple input redirection"
wc -l < file1 < file2
echo "# END TEST"
EOF

cat <<EOF > self_check/test_multiple_output_redirection.txt
echo "# TEST: Multiple output redirection"
echo "Hello" > file1 > file2
echo "# END TEST"
EOF

cat <<EOF > self_check/test_missing_input_redirection.txt
echo "# TEST: Missing input redirection file"
wc -l <
echo "# END TEST"
EOF

cat <<EOF > self_check/test_missing_output_redirection.txt
echo "# TEST: Missing output redirection file"
echo "Hello" >
echo "# END TEST"
EOF

cat <<EOF > self_check/test_no_read_permission.txt
echo "# TEST: Input redirection with no read permission"
echo "Hello" > no_read_file
chmod -r no_read_file
wc -l < no_read_file
rm no_read_file
echo "# END TEST"
EOF

cat <<EOF > self_check/test_no_write_permission.txt
echo "# TEST: Output redirection with no write permission"
echo "Hello" > no_write_file
chmod -w no_write_file
echo "New Line" > no_write_file
rm no_write_file
echo "# END TEST"
EOF

cat <<EOF > self_check/test_invalid_command_redirection.txt
echo "# TEST: Invalid command with redirection"
invalid_command > output.txt
echo "# END TEST"
EOF

cat <<EOF > self_check/test_single_pipe.txt
echo "# TEST: Single pipe"
echo "Hello World" | wc -w
echo "# END TEST"
EOF

cat <<EOF > self_check/test_multiple_pipe.txt
echo "# TEST: Multiple pipe"
echo "Hello World" | tr ' ' '\n' | wc -l
echo "# END TEST"
EOF

cat <<EOF > self_check/test_pipe_with_redirection.txt
echo "# TEST: Pipe with redirection"
echo "Hello World" | wc -w > pipe_output.txt
cat pipe_output.txt
rm pipe_output.txt
echo "# END TEST"
EOF

# 백그라운드 실행 테스트 케이스 작성
cat <<EOF > self_check/test_background_single.txt
echo "# TEST: Single background process"
sleep 5 &
echo "Background process started"
echo "# END TEST"
EOF

cat <<EOF > self_check/test_background_multiple.txt
echo "# TEST: Multiple background processes"
sleep 3 &
sleep 4 &
sleep 5 &
echo "Multiple background processes started"
echo "# END TEST"
EOF

cat <<EOF > self_check/test_background_termination.txt
echo "# TEST: Background process termination"
sleep 2 &
echo "Wait for 2 seconds and check"
echo "# END TEST"
EOF


echo "테스트 케이스 파일이 'self_check' 디렉토리에 생성되었습니다."
