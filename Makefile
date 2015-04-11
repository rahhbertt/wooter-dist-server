FLAGS = -Wall -g -std=c++11

all: wooter

wooter: network.cpp
	g++ ${FLAGS} -o networks network.cpp -pthread

clean:
	rm -r fids flwes flwrs users woots
