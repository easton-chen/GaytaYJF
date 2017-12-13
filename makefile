PART2=part2 part2_MMX
NOOPT=-mno-mmx -mno-sse -mno-sse2
all: $(PART2) 

part2: part2.cpp
	g++ $(NOOPT) -pg -o  part2 part2.cpp
part2_MMX: part2_MMX.cpp	 
	g++ $(FLAG) -pg -o  part2_MMX part2_MMX.cpp
part3: part3.cpp
	g++ $(FLAG) -o part3 part3.cpp
.PHONY: clean

clean:
	rm -rf $(PART2)
