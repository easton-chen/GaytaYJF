PART2=part2 part2_MMX part2_MMX2
NOOPT=-mno-mmx -mno-sse -mno-sse2
OPT=-mmmx
all: $(PART2) 

part2: part2.cpp
	g++ $(NOOPT) -o part2 part2.cpp
part2_MMX: part2_MMX.cpp	 
	g++ $(OPT) -o part2_MMX part2_MMX.cpp
part2_MMX2: part2_MMX2.cpp	 
	g++ -o part2_MMX2 part2_MMX2.cpp
part3: part3.cpp
	g++ $(NOOPT) -o part3 part3.cpp
.PHONY: clean

clean:
	rm -rf $(PART2)
