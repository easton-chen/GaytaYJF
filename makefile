PART2=part2 part2_MMX part2_SSE part2_AVX
NOMMX=-mno-mmx 
NOSSE=-mno-sse
NOSSE2=-mno-sse2 
OPT=-mmmx
all: $(PART2) 

part2: part2.cpp
	g++ $(NOMMX) $(NOSSE) $(NOSSE2) -o  part2 part2.cpp
part2_MMX: part2_MMX.cpp	 
	g++ $(NOSSE) $(NOSSE2) -o part2_MMX part2_MMX.cpp
part2_SSE: part2_SSE.cpp
	g++ -o part2_SSE part2_SSE.cpp
part2_AVX: part2_AVX.cpp
	g++ -o part2_AVX part2_AVX.cpp
part3: part3.cpp
	g++ $(NOOPT) -o part3 part3.cpp
.PHONY: clean

clean:
	rm -rf $(PART2)
