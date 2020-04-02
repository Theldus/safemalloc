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

# Fancy colors =)
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Check error code
function check_error()
{
	if [ $? -eq 0 ]
	then
		echo -e " [${GREEN}PASSED${NC}]"
	else
		echo -e " [${RED}NOT PASSED${NC}]"
		exit 1
	fi
}

# Check for Valgrind.
if [ ! -x "$(command -v valgrind)" ]
then
	echo "Valgrind not found! Valgrind is necessary in order to"
	echo "run the race conditions test."
	exit 1
fi

# Check for race.
if [[ ! -f race ]]
then
	echo "data_race: race file not found!"
	echo "please build it first, by typing: make tests"
	exit 1
fi

echo -e "\nData racing tests:"

# Run test with drd and helgrind, in order to check data races.
echo -n "Running Valgrind DRD... "
valgrind --tool=drd --error-exitcode=1 ./race &> /dev/null
check_error

echo -n "Running Valgrind Helgrind... "
valgrind --tool=helgrind --error-exitcode=1 ./race &> /dev/null
check_error

echo ""
