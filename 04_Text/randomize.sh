#!/bin/bash

DEFAULT_DELAY=0.01
SPACE=40
NEW_LINE=12

# единственный параметр - время между появлением букв
DELAY="${1:-$DEFAULT_DELAY}"

TEXT=$(cat - | hexdump -ve '/1 "%o\n"')

COL=0
LINE=0
CHARS_LIST=""

for chr in $TEXT; do
    case $chr in
        $SPACE)
            COL=$((COL+1));;
        $NEW_LINE) 
            LINE=$((LINE+1))
            COL=0;;
        *) 
            CHARS_LIST="${COL} ${LINE} ${chr}\n${CHARS_LIST}"
            COL=$((COL+1));;
    esac
done

tput clear

echo -en $CHARS_LIST | shuf |
while read -r X Y CHR; do
    tput cup $Y $X
    printf '\'$CHR
    sleep $DELAY
done

tput cup $((LINE + 1)) 0
