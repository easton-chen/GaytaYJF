PART2=part2 part2_MMX

all: $(PART2) 

part2: part2.cpp
	g++ -o part2 part2.cpp
part2_MMX: part2_MMX.cpp	 
	g++ -o part2 part2_MMX.cpp

.PHONY: clean

clean:
	rm -rf $(PART2)
