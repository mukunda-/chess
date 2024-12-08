#!/usr/bin/env bash

YEAR=2024
MONTH=07
LICHESS_ARCHIVE=lichess_db_standard_rated_$YEAR\_$MONTH
URL=https://database.lichess.org/standard/$LICHESS_ARCHIVE.pgn.zst

cd data/$LICHESS_ARCHIVE
mkdir -p data/$LICHESS_ARCHIVE

# echo "Retrieving $URL";
# mkdir -p data/$LICHESS_ARCHIVE
# wget $URL

zstdcat $LICHESS_ARCHIVE.pgn.zst | ./build/bin/pgne3k lichess-tag-roster.txt # -#1000000 --minply 3 --tsv -t tag-criteria.txt --nobadresults --fixresulttags   --tagsubstr -Wuci 2> progress
