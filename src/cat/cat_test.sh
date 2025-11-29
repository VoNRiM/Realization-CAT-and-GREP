#!/bin/bash

PASS=0
FAIL=0
TOTAL=0

#massive
FILES=("test1.txt" "test2.txt" "test3.txt" "test4.txt" "test5.txt" "test6.txt" "test7.txt")
MULTI_FILES=("test1.txt test2.txt" "test2.txt test3.txt" "test3.txt test4.txt test5.txt" "test1.txt test2.txt test7.txt" "test7.txt test1.txt")
FLAGS=("" "-n" "--number" "--squeeze-blank" "--number-nonblank" "-b" "-E" "-t" "-e" "-T" "-s" "-n -E" "-n -T" "-b -E" "-s -n" "-E -T" "-n -b -E -T -s"
"--number -b" "--number --squeeze-blank -s")
run_test() {

    local flags="$1"
    local file="$2"

    ((TOTAL++))

   

    echo "=== Running Test ==="
    echo "System cat:"
    eval "cat $flags $file"
    echo -e "\n---"
    echo "Your cat:"
    eval "./s21_cat $flags $file"

    system_output=$(eval cat $flags "$file")
    your_output=$(eval ./s21_cat $flags "$file")
    #result = $(eval "$command" 2>&1)

    if [ "$system_output" = "$your_output" ]; then
        echo -e "\n== YES == PASS: \n$flags \n$file"
        ((PASS++))
    else
        echo -e "\n== X == FAIL: \n$flags \n$file"     
        ((FAIL++))
    fi
        echo "============================="
}
# run_mutifail_test(){

# }
setup_files(){
    echo -e "line 1\nline 2\nline 3" > test1.txt
    echo -e "\tTAbbed line\n\nMultiple\n\nEmpty lines" > test2.txt
    echo -e "Line with\tspecial\tchars\nAnd new line " > test3.txt
    echo -n "No newline at end" > test4.txt
    echo -e "Normal line 1
Normal line 2

Line after empty line
    Line with leading spaces
Line with	tabs	here
Line with special chars: !@#$%^&*()_+
Line with unicode: 

Multiple empty lines above...


Line after multiple empties
Line with trailing spaces    
Line with mixed    spaces	and	tabs
Last line without newline at end" > test5.txt
}
    echo "" > test6.txt
    
clean_up(){
    rm -f test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt 
}

main() {
    setup_files
    for file in "${FILES[@]}"; do 
        echo "=== TESTING FILE: $file ==="
        for flags in "${FLAGS[@]}"; do
            run_test "$flags" "$file"
        done
        echo
    done

    for file in "${MULTI_FILES[@]}"; do 
        echo "=== TESTING FILE: $file ==="
        for flags in "${FLAGS[@]}"; do
            run_test "$flags" "$file"
        done
        echo
    done

    # run_test "Basic output" "" "test1.txt"
    # run_test "Line numbers" "-n" "test1.txt" 
    # run_test "Show ends" "-E" "test1.txt"
    # run_test "Squeeze empty" "-s" "test2.txt"
    # run_test "Show tabs" "-T" "test3.txt"
    # run_test "Number non-empty" "-b" "test2.txt"
    
    echo "=== FINAL RESULTS ==="
    echo "PASS: $PASS"
    echo "FAIL: $FAIL"
    echo "TOTAL: $TOTAL"

    clean_up
}

main "$@"