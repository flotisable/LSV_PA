CXX      := g++
LD       := g++
CXXFLAGS := -std=c++11 -DTEST=1
PROG     := testMuxDecompCore

projectDir := ../..

all: $(PROG)

$(PROG): main.o testLsvMuxDecomp.o $(projectDir)/lsvMuxDecomp.o
	$(LD) -o $@ $^

main.o: main.cpp testLsvMuxDecomp.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

testLsvMuxDecomp.o: testLsvMuxDecomp.cpp testLsvMuxDecomp.h $(projectDir)/lsvMuxDecomp.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(projectDir)/lsvMuxDecomp.o: $(projectDir)/lsvMuxDecomp.cpp $(projectDir)/lsvMuxDecomp.h testLsvMuxDecomp.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

test: $(PROG)
	./$(PROG)

clean:
	rm $(projectDir)/*.o *.o $(PROG)
