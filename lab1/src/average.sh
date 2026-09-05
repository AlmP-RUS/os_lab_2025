#!/bin/sh

sum=0

for number; do
	sum=$((sum + number))
done

echo "Number of arguments: $#"
echo "Average: $(($sum/$#))"

