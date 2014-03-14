#!/bin/bash
if [ "$3" = "" ]; then
	DBS=$(nproc)
	echo "no arguments - reading nproc"
else
	DBS=$3
	echo "user specified number of processors"
fi
LINES=$(wc -l ./20news/twenty_news_groups_test.tsv)
#echo $LINES/$DBS
TOTALLINES=$(echo $LINES | awk 'BEGIN {FS=" "}{print $1}')
LINEC=$(echo $(($TOTALLINES/$DBS)))
echo $LINEC
split ./20news/twenty_news_groups_test.tsv -d -l $LINEC
./naivebayes $1 $2 $DBS
rm x*