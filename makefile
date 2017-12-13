PART2=part2 part2_MMX
NOOPT=-mno-mmx -mno-sse -mno-sse2
all: $(PART2) 

part2: part2.cpp
	g++ $(NOOPT) -o part2 part2.cpp
part2_MMX: part2_MMX.cpp	 
	g++ -o part2_MMX part2_MMX.cpp

.PHONY: clean

clean:
	rm -rf $(PART2)
