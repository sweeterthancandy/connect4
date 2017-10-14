
RELEASEFLAGS=-O2
DEBUGFLAGS=-ggdb -O0 
#RELEASEDEBUGFLAGS=$(DEBUGFLAGS)
RELEASEDEBUGFLAGS=$(RELEASEFLAGS)

CXX=clang++
CXXFLAGS=-std=c++14  -Wno-switch $(RELEASEDEBUGFLAGS)
LDFLAGS=-lgtest
CPPFLAGS=-Iinclude

ConnectFour:

%.png: %.dot
	dot -Grankdir=LR -Tpng -o $@ $<


clean:
	$(RM) ttt *.png *.dot
