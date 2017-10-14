
CXX=clang++
CXXFLAGS=-std=c++14 -ggdb -O0

connect4:

%.png: %.dot
	dot -Grankdir=LR -Tpng -o $@ $<


clean:
	$(RM) ttt *.png *.dot
