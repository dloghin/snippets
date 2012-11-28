# Copyright 2012 Dumi Loghin <dumiloghin@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This bash script tries to get the steps for transforming one word
# to another (with the same length). All intermediate word must have
# the same length, must be valid words and must differ only by one
# character.

#!/bin/bash

# test if a word is correctly spelled in english
# uses 'spell' application from Linux
# @param - the word to be checked
# @return - 1 if the word is valid, 0 - otherwise
is_valid_word() 
{
	# if arg is null => not valid (0)
	if [ -z $1 ]; then
		return 0
	else
	# if spell return the word, then is not valid (0), else is valid (1)
		echo $1 > temp		
		RET=`spell temp`	
		rm -r temp
		if [ -z $RET ]; then
			return 1
		else
			return 0
		fi
	fi
}

# the script must take two word as arguments
if [ $# -ne 2 ]; then
	echo "Usage: $0 <word_from> <word_to>"
	exit 1
fi

# check if spell is installed
CMD1=`type spell | grep 'not found'`
echo $CMD1 > temp
read CMD1 < temp
CMD2=`command -v spell`
# echo "|$CMD1|"
if [[ ! -z "$CMD1" ]] || [[ ! -x $CMD2 ]]; then
	echo "Error: spell is not installed!"
	exit 1
fi

# first check: words must have the same length
echo $1 > temp
N=`wc -c temp | cut -d ' ' -f 1`
N=$(($N-1))
echo $2 > temp
N1=`wc -c temp | cut -d ' ' -f 1`
N1=$(($N1-1))
rm -rf temp
if [ "$N" != "$N1" ]; then
	echo "Error: words have different number of characters!"
	exit 1
fi

# the words must be valid
is_valid_word $1
if [ $? -ne 1 ]; then
	echo "Error: $1 is not a valid word!"
	exit 1
fi
is_valid_word $2
if [ $? -ne 1 ]; then
	echo "Error: $2 is not a valid word!"

	exit 1
fi

# the algorithm:
# we use a depth first (Lee) search
# on each step we search for valid words that have only one different character 

Q=($1)		# words queue
P=(-1)		# parent index array
FIRST=0 	# current queue element
LAST=${#Q[*]}	# size of queue
FOUND=0 	# solution found?

# while queue is not empty and we didn't find the final word
while [ $FIRST -lt $LAST -a $FOUND -eq 0 ]
do
	WORD=${Q[$FIRST]}
	i=0
	while [ $i -lt $N ]
	do
		if [ "${WORD:$i:1}" != "${2:$i:1}" ]; then
			for CHAR in a b c d e f g h i j k l m n o p q r s t u v x y z
			do
				if [ "$CHAR" != "${WORD:$i:1}" ]; then
					i1=$(($i+1))
					i2=$(($N-$i1))
					if [ $i -eq 0 ]; then
					
	NEWWORD=${CHAR}${WORD:1:$i2}
					elif [ $i1 -eq $N ]; then
					
	NEWWORD=${WORD:0:$i}${CHAR}
					else
						NEWWORD=${WORD:0:$i}${CHAR}${WORD:$i1:$i2}
					fi
					
					is_valid_word $NEWWORD
					if [ $? -eq 1 ]; then
						# echo $NEWWORD
						if [ "$NEWWORD" == "$2" ]; then
						
	FOUND=1
						else
							Q[$LAST]=$NEWWORD
							P[$LAST]=$FIRST
							LAST=$(($LAST+1))
						fi
					fi
			
	fi
			done
		fi
		i=$(($i+1))
	done

	FIRST=$(($FIRST+1))
done

if [ $FOUND -eq 0 ]; then
	echo "No transformation found!"
	exit 1
fi

# Display the result using a stack
S=($2)		# start with last word
TOP=1
FIRST=$(($FIRST-1))
while [ $FIRST -gt -1 ]
do
	S[$TOP]=${Q[$FIRST]}

	TOP=$(($TOP+1))
	FIRST=${P[$FIRST]}
done
echo -n "Steps: "
while [ $TOP -gt 0 ]
do
	TOP=$(($TOP-1))
	if [ $TOP -eq 0 ]; then
		echo "${S[$TOP]}."
	else
		echo -n "${S[$TOP]} -> "
	fi
done
