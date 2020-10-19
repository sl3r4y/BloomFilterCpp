all: bloom_filter
	g++ -g -Wall -o bloom_filter Bloom_filter.cpp main.cpp -lcrypto -lgmp