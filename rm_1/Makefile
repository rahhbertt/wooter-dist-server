FLAGS = -Wall -g 

all: wooter RM

wooter: network.cpp
	g++ ${FLAGS} network.cpp -o networks -pthread -std=c++11 -Wl,--no-as-needed

RM: net_2.cpp
	g++ ${FLAGS} net_2.cpp -o nets_2 -pthread -std=c++11 -Wl,--no-as-needed

clean: 
	rm -r fids flwes flwrs users woots
