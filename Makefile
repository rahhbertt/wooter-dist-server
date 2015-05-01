FLAGS = -Wall -g -pthread -std=c++11 -Wl,--no-as-needed

all: wooter

wooter: wooter.cpp
	g++ ${FLAGS} wooter.cpp -o wooter_server 

clean: 
	rm -r fids flwes flwrs users woots rm_*/*
