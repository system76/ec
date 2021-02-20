#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# This script produces layout data for the System76 Keyboard Configurator

set -e

rm -rf build/layouts
mkdir -p build/layouts
D="$(realpath build/layouts)"

binary="$D/keymap"
source="$binary.c"
header="src/common/include/common/keymap.h"
echo "#include <stdio.h>" > "$source"
echo "#include \"$header\"" >> "$source"
echo "int main(int argc, char **argv) {" >> "$source"
grep '^#define \(K_\|KT_FN\)' "$header" \
| cut -d ' ' -f2 \
| while read keycode
do
	name="$(echo "$keycode" | cut -d '_' -f2-)"
	echo "printf(\"${name},0x%04X\\n\", $keycode);" >> "$source"
done
echo "return 0;" >> "$source"
echo "}" >> "$source"
gcc -I. "$source" -o "$binary"
"$binary" | tee "$D/keymap.csv"

cd src/board
for board in */*
do
	file="$board/include/board/keymap.h"
	if [ ! -e "$file" ]
	then
		continue
	fi
	echo "# $board"
	mkdir -p "$D/$board"
	cp "$D/keymap.csv" "$D/$board"
	row=0
	rg \
		--multiline \
		--multiline-dotall \
		--regexp '#define LAYOUT\(.*\) \{.*\}' \
		"$file" \
	| grep --only-matching '\{.*\}' \
	| sed 's/^{ //' \
	| sed 's/ }$//' \
	| sed 's/, / /g' \
	| while read line
	do
		col=0
		for word in $line
		do
			if [ "$word" != "___" ]
			then
				echo "$word,$row,$col"
			fi
			col=$(expr $col + 1)
		done
		row=$(expr $row + 1)
	done \
	| sort -n \
	| tee "$D/${board}/layout.csv"
done
