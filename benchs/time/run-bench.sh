#!/usr/bin/env bash

#
# MIT License
#
# Copyright (c) 2019 Davidson Francis <davidsondfgl@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
start=10000
stop=100000
multiplier=10000

# Check for R.
if [ ! -x "$(command -v Rscript)" ]
then
	echo "Rscript not found! GDB and Rscript is necessary in order to"
	echo "run the benchmarks."
	exit 1
fi

# Check for time file.
if [[ ! -f time ]]
then
	echo "time: time file not found!"
	echo "please build it first"
	exit 1
fi

# Prepare CSV
echo "num_pointers, time_sfmalloc, time_malloc" > csv_time

echo -e "\nCalculating times..."
for (( size=start; size<=stop; ))
do
	echo "Test #size: $size"
	time_sfmalloc=$(./time $size 1 | tr -cd '[:digit:]')
	time_malloc=$(./time   $size 2 | tr -cd '[:digit:]')
	echo "$size, $time_sfmalloc, $time_malloc" >> csv_time
	size=$((size+5000))
done

# Plot graph =)
echo -e "Plotting graph...\n"
Rscript plot.R &> /dev/null
