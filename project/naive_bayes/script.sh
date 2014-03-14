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
echo $LINEC
if [ $1 = 1 ]; then
  split ./20news/twenty_news_groups_test.tsv -d -l $LINEC
else
  split ./reuters/test/mega_document -d -l $LINEC
fi
./naivebayes $1 $2 $DBS
rm x*