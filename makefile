# simplest makefile
# don't bother compiling .so and .o for 3MB of executable

# make debug for debug version
# make release for release

# tests and everything todo later

release_cmd := g++  -std=c++20 ./server/server.cpp ./src/*.cpp -I./include -lpthread

install:
	if [ -d ~/.yachat-kottochii ]; then \
	rm -r ~/.yachat-kottochii ; \
	fi
	mkdir ~/.yachat-kottochii
	cp -r ./html ~/.yachat-kottochii

release:
	$(release_cmd) -o ./bin/release

all: release

path:
	echo ${PATH}