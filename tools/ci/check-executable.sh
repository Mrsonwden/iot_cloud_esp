#!/bin/bash
# This script finds executable files in the repository, excluding some directories,
# then prints the list of all files which are not in executable-list.txt.
# Returns with error if this list is non-empty.

cd $IDF_PATH

tmp_list=$(mktemp)
out_list=$(mktemp)

# build exclude pattern like '-o -path ./components/component/submodule' for each submodule
submodule_excludes=$(git config --file .gitmodules --get-regexp path | awk '{ print "-o -path ./" $2 }')

# figure out which flag to use when searching for executable files
if [ "$(uname -s)" == "Darwin" ]; then
    perm_flag="-perm +111"
else
    perm_flag="-executable"
fi

find . -type d \( \
            -path ./.git \
            -o -name build \
            -o -name builds \
            $submodule_excludes \
        \) -prune -o -type f $perm_flag -print \
    | sed "s|^\./||" > $tmp_list

# this looks for lines present in tmp_list but not in executable-list.txt
comm -13 <(cat tools/ci/executable-list.txt | sed -n "/^#/!p" | sort) <(sort $tmp_list) > $out_list

ret=0
if [ -s $out_list ]; then
    ret=1
    echo "Error: the following file(s) have executable flag set:"
    echo ""
    cat $out_list
    echo ""
    echo "If any files need to be executable (usually, scripts), add them to tools/ci/executable-list.txt"
    echo "Make the rest of the files non-executable using 'chmod -x <filename>'."
    echo "On Windows, use 'git update-index --chmod=-x filename' instead."
    echo ""
fi

rm $tmp_list
rm $out_list

exit $ret
