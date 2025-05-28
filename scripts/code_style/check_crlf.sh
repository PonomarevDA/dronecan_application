#!/bin/bash

if ! command -v dos2unix >/dev/null 2>&1; then
    echo "Install dos2unix: sudo apt install dos2unix"
    exit 1
fi

for file in $(find $dir -type f); do
    if [[ $file == *.git/* ]] || \
       [[ $file == *.venv/* ]] || \
       [[ $file == *build* ]] || \
       [[ $file == *doc* ]] || \
       [[ $file == *shell ]]; then
        continue
    fi

    compare_result="$(dos2unix --safe < $file | cmp - $file)"
    if [[ $compare_result == *"differ"* ]]; then
        echo $compare_result
        is_error_occured=1
    fi
done

if [[ $is_error_occured == 1 ]]; then
    echo "Fix crlf issue!"
    exit 1
fi
