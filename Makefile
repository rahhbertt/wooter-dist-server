FLAGS = -Wall -g 

all: wooter

wooter: network.cpp
	g++ ${FLAGS} network.cpp -o networks -pthread -std=c++11 -Wl,--no-as-needed

clean: 
	rm -r fids flwes flwrs users woots
