#!/bin/bash
if [ "$3" = "" ]; then
	DBS=$(nproc)
	echo "no arguments - reading nproc"
else
	DBS=$3
	echo "user specified number of processors"
fi
if [ $1 = 1 ]; then
  LINES=$(wc -l ./20news/twenty_news_groups_test.tsv)
else
  LINES=$(wc -l ./reuters/test/mega_document)
fi
#echo $LINES/$DBS
TOTALLINES=$(echo $LINES | awk 'BEGIN {FS=" "}{print $1}')
LINEC=$(echo $(($TOTALLINES/$DBS)))
# echo $TOTALLINES
# echo $LINEC
mkdir -p "parallel_test_files"
if [ $1 = 1 ]; then
  split ./20news/twenty_news_groups_test.tsv -d -l $LINEC "parallel_test_files/"
else
  split ./reuters/test/mega_document -d -l $LINEC "parallel_test_files/"
fi
COVEREDLINES=$(echo "$LINEC * $DBS" | bc)
if [ $COVEREDLINES != $TOTALLINES ]; then
  # echo "Unequal"
  THETWO=$(ls -Sr parallel_test_files/ | head -2)
  # echo $THETWO
  oldIFS="$IFS"
  IFS='
  '
  IFS=${IFS:0:1}
  lines=( $THETWO )
  IFS="$oldIFS"
  # echo "${lines[0]}"
  # echo "${lines[1]}"
  cat "parallel_test_files/${lines[0]}" >> "parallel_test_files/${lines[1]}"
  rm "parallel_test_files/${lines[0]}"
  # echo $(echo "$TOTALLINES - $COVEREDLINES" | bc)
fi
./naivebayes $1 $2 $DBS
rm -r "parallel_test_files"