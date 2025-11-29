#!/bin/bash

PASS=0
FAIL=0
TOTAL=0

#massive
FILES=("test1.txt" "test2.txt" "test3.txt" "test4.txt" "test5.txt" "test6.txt")
# "test7.txt")
MULTI_FILES=("test1.txt test2.txt" "test2.txt test3.txt" "test3.txt test4.txt test5.txt" "test1.txt test2.txt test7.txt" "test7.txt test1.txt")
FLAGS=("" "-e" "-i" "-v" "-c" "-l" "-n" "-h" "-s" "-o" "-i -v" "-v -c" "-i -c" "-v -c -l -n")
# 
WORDS=( "Normal" "line" "Line" "unicode" "multiple")
MULTI_WORDS=("Normal line" "line Line" "unicode multiple" "Normal line unicode")
PATTERNS=("pattern1.txt" "pattern2.txt" "pattern3.txt" "pattern4.txt")


run_test(){

    local flags="$1"
    local words="$2"
    local file="$3"
    
    ((TOTAL++))

    echo "=== Running Test ==="
    echo "System grep:"
    eval "grep $flags '$words' $file" 2>&1
    echo -e "\n---"
    echo "Your grep:"
    eval "./s21_grep $flags '$words' $file" 2>&1

    system_output=$(eval grep "$flags '$words' $file" 2>&1)
    your_output=$(eval ./s21_grep "$flags '$words' $file" 2>&1)

    if [ "$system_output" = "$your_output" ]; then
        echo -e "\n== YES == PASS: \n$flags \n$words \n$file" 
        ((PASS++))
    else
        echo -e "\n== X == FAIL: \n$flags \n$words \n$file"
        ((FAIL++))
    fi
        echo "=============================" 

}

run_test_flag_f(){

    local flags="$1"
    local patterns="$2"
    local file="$3"
    
    ((TOTAL++))

    echo "=== Running Test ==="
    echo "System grep:"
    eval "grep $flags -f'$patterns' $file" 2>&1
    echo -e "\n---"
    echo "Your grep:"
    eval "./s21_grep $flags -f '$patterns' $file" 2>&1

    system_output=$(eval grep "$flags -f '$patterns' $file" 2>&1)
    your_output=$(eval ./s21_grep "$flags -f '$patterns' $file" 2>&1)

    if [ "$system_output" = "$your_output" ]; then
        echo -e "\n== YES == PASS: \n$flags \n$patterns \n$file" 
        ((PASS++))
    else
        echo -e "\n== X == FAIL: \n$flags \n$patterns \n$file"
        ((FAIL++))
    fi
        echo "=============================" 
   
}
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
Line with unicode: caf nave  music

Multiple empty lines above...


Line after multiple empties
Line with trailing spaces    
Line with mixed    spaces	and	tabs
Last line without newline at end" > test5.txt
    echo "" > test6.txt
    echo "Normal" > pattern1.txt
    echo "Normal Line" > pattern2.txt
    echo -e "Normal\n line  " > pattern3.txt
    echo "Normal|line " > pattern4.txt

}

clean_up(){

    rm -f test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt
    rm -f pattern1.txt pattern2.txt pattern3.txt pattern4.txt
}

main(){

    setup_files

    for file in "${FILES[@]}"; do
        echo "=== TESTING FILE: $file ==="
        for word in "${WORDS[@]}"; do
            echo "=== WORD: $word"
            for flags in "${FLAGS[@]}"; do
                run_test "$flags" "$word" "$file"
            done
        done
        echo
    done

        for file in "${FILES[@]}"; do
        echo "=== TESTING FILE: $file ==="
        for word in "${MULTI_WORDS[@]}"; do
            echo "=== WORD: $word"
            for flags in "${FLAGS[@]}"; do
                run_test "$flags" "$word" "$file"
            done
        done
        echo
    done

    for file in "${MULTI_FILES[@]}"; do
        echo "=== TESTING FILE: $file ==="
        for word in "${WORDS[@]}"; do
            echo "=== WORD: $word"
            for flags in "${FLAGS[@]}"; do
                run_test "$flags" "$word" "$file"
            done
        done
        echo
    done

            for file in "${MULTI_FILES[@]}"; do
        echo "=== TESTING FILE: $file ==="
        for pattern in "${PATTERNS[@]}"; do
            echo "=== WORD FROM FILE: $pattern"
            for flags in "${FLAGS[@]}";do
                run_test_flag_f "$flags" "$pattern" "$file"
            done
        done
        echo
    done
    

    echo "=== FINAL RESULTS ==="
    echo "PASS: $PASS"
    echo "FAIL: $FAIL"
    echo "TOTAL: $TOTAL"

    clean_up

}   

main "@"
