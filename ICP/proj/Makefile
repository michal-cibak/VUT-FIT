# Project: ICP - Editor a interpret hierarchicky strukturovaných funkčních bloků
# Authors: Michal Cibák - xcibak00, FIT VUT
#          Tomáš Hrúz   - xhruzt00, FIT VUT
# Date: 15.4.2021

.PHONY: all
all: funblocks

funblocks: main.cpp funblocks.cpp funblocks.hpp
	g++ -g -std=c++17 -pedantic -Wall -Wextra -O2 main.cpp funblocks.cpp -o funblocks

run: all
	./funblocks

clean: funblocks
	-rm funblocks
