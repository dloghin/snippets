# Copyright 2013 Dumi Loghin <dumiloghin@gmail.com>
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

# This bash script finds all valid English words, with a specified length,
# from an input text file and stores them (uniquelly) into an output file. 

#/bin/bash

WORD_SIZE=8

OUT_FILE=list.txt

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
		if [ ${#RET} -gt 0 ]; then
			return 0
		else
			return 1
		fi
	fi
}

if [ $# -lt 1 ]
then
	echo "Usage: $0 <text_file>"
	exit 1
fi

echo "# Word List" > $OUT_FILE

cat $1 | tr '[ <>.,:;!?/\-\\()&"=]' '\n' | tr '[:upper:]' '[:lower:]' > tmp 

while read raw_word
do
	word=${raw_word//[^a-zA-Z]/}
	is_valid_word $word
	if [[ ( $? -eq 1 ) && ( ${#word} -eq $WORD_SIZE ) ]]
	then
		FOUND=`grep $word $OUT_FILE`
		if [ ${#FOUND} -eq 0 ]
		then
			echo $word >> $OUT_FILE
		fi
	fi
done < tmp

rm -f tmp
