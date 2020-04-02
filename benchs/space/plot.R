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

data <- read.csv("csv_space", header=TRUE, sep=",")

#PNG Device
png(
	filename="bench.png",
	width = 550, height = 450,
	units = "px", pointsize = 15
)

# Margins
par(mar = c(5.1, 4.1, 2.5, 2.1))

# Plot PBD
plot(
	data$size_per_element, data$reference_size,
	xlab="Size per element (SPE) (bytes)", ylab="Total size (SPE*100) (in bytes)",
	main="Memory Consumption, 64 bits system (log scale)",
	type="l", col="red", lwd=3, log="x"
)

# Saved space and not saved
lines(data$size_per_element, data$save_space, col="blue", lwd=3)
lines(data$size_per_element, data$not_save_space, col="orange", lwd=3)

#Legend
legend("topleft", legend = c("Ideal", "w/   SFMALLOC_SAVE_SPACE",
	"w/o SFMALLOC_SAVE_SPACE"), col = c("red", "blue", "orange"),
	lty=c(1,1), cex = 0.75)

#GRID
grid(NULL, NULL)

#Device off
dev.off()
