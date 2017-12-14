PART2=part2 part2_MMX part2_SSE part2_AVX 
PART3=part3 part3_MMX part3_SSE
NOMMX=-mno-mmx 
NOSSE=-mno-sse
NOSSE2=-mno-sse2 

all: $(PART2) $(PART3)

part2: part2.cpp
	g++ $(NOMMX) $(NOSSE) $(NOSSE2) -o  part2 part2.cpp

part2_MMX: part2_MMX.cpp	 
	g++ $(NOSSE) $(NOSSE2) -o part2_MMX part2_MMX.cpp

part2_SSE: part2_SSE.cpp
	g++ -o part2_SSE part2_SSE.cpp

part2_AVX: part2_AVX.cpp
	g++ -o part2_AVX part2_AVX.cpp

part3: part3.cpp
	g++ $(NOMMX) $(NOSSE) $(NOSSE2) -g -o part3 part3.cpp

part3_MMX: part3_MMX.cpp	 
	g++ $(NOSSE) $(NOSSE2) -o part3_MMX part3_MMX.cpp

part3_SSE: part3_SSE.cpp
	g++ -o part3_SSE part3_SSE.cpp

.PHONY: clean

clean:
	rm -rf $(PART2) $(PART3) p*.yuv
