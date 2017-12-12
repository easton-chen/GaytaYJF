PART2=part2 part2_MMX part3
FLAG=-O0
all: $(PART2) 

part2: part2.cpp
	g++ $(FLAG) -o part2 part2.cpp
part2_MMX: part2_MMX.cpp	 
	g++ $(FLAG) -o part2_MMX part2_MMX.cpp
part3: part3.cpp
	g++ $(FLAG) -o part3 part3.cpp
.PHONY: clean

clean:
	rm -rf $(PART2)
