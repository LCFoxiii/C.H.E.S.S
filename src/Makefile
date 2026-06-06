# I genuinely strongly suggest using debug
# 
# if you want to make changes to the code.
# If you just want to run it, use "all".

src = main.cpp search.cpp uci.cpp evaluate.cpp
name = chess

all:
	g++ -O3 -march=native -DNDEBUG -funroll-loops -fprefetch-loop-arrays -flto=auto -std=c++17 -pthread -o $(name) $(src)
debug:
	g++ -g -O0 -pthread -o $(name) $(src)
clean:
	rm -f $(name)