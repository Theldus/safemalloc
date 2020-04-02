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

data <- read.csv("csv_time", header=TRUE, sep=",")

#PNG Device
png(
	filename="bench.png",
	width = 550, height = 450,
	units = "px", pointsize = 15
)

# Margins
par(mar = c(5.1, 4.1, 2.5, 2.1))

factor <- 1000

# Plot PBD
plot(
	data$num_pointers, (data$time_sfmalloc)/factor,
	xlab="Number of allocations", ylab="Time (in milliseconds)",
	main="Time per allocation",
	type="l", col="blue", lwd=3, ylim=c(0,max(data$time_sfmalloc)/factor)
)

# Saved space and not saved
lines(data$num_pointers, (data$time_malloc)/factor, col="orange", lwd=3)

#Legend
legend("topleft", legend = c("sfmalloc", "malloc"), col = c("blue", "orange"),
	lty=c(1,1), cex = 0.75)

#GRID
grid(NULL, NULL)

#Device off
dev.off()
