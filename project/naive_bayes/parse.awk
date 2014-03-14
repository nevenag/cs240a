awk 'BEGIN{FS="\t"}
{
printf("%s \t %s",$1,$2)
split($3,text,"Lines:")
printf("%s \n",text[2])
}'
