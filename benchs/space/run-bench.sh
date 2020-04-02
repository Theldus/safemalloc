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
start=50
stop=1000
increment=10

# Check for R.
if [ ! -x "$(command -v Rscript)" ]
then
	echo "Rscript not found! GDB and Rscript is necessary in order to"
	echo "run the benchmarks."
	exit 1
fi

# Check for space and space_save.
if [[ ! -f space || ! -f space_save ]]
then
	echo "memory_estimator: space and/or space_save not found!"
	echo "please build them first"
	exit 1
fi

# Prepare CSV
echo "size_per_element, reference_size, save_space, not_save_space" > csv_space

echo -e "\nCalculating expected sizes..."
for (( size=start; size<stop; size+=increment ))
do
	echo "Test #size: $size"
	bytes_yes_save=$(./space_save 100 $size | head -2 | tail -1 | tr -dc '[:digit:]')
	bytes_not_save=$(./space      100 $size | head -2 | tail -1 | tr -dc '[:digit:]')
	echo "$size, $((100*size)), $bytes_yes_save, $bytes_not_save" >> csv_space
done

# Plot graph =)
echo -e "Plotting graph...\n"
Rscript plot.R &> /dev/null
