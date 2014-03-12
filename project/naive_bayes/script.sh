#!/bin/bash
        DBS=$(nproc)
        LINES=$(wc -l twenty_news_groups_test.tsv)
        #echo $LINES/$DBS
        FNAME=$(echo $LINES | awk 'BEGIN {FS=" "}{print $1}')
        #echo $FNAME/4
        LINEC=$(echo $(($FNAME/$DBS)))
        echo $LINEC
        split twenty_news_groups_test.tsv -d -l $LINEC
        ./naivebayes 1 1