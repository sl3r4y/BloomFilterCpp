// Sébastien Leray - M2 Cyber

#ifndef __BLOOM_FILTER__
#define __BLOOM_FILTER__

#include <iostream>
#include <vector>
#include <stddef.h>

using namespace std;

class Bloom_filter
{
public:
    /**
     * This is constructor of Bloom_filter class.
     * @param unsigned int m : The value must be a multiple of eight.
     * @param vector<pair<unsigned char *(*)(const unsigned char *, size_t, unsigned char *), size_t>> hashFunctions : This is the list of hash functions.
     */
    Bloom_filter(size_t m, vector<pair<unsigned char *(*)(const unsigned char *, size_t, unsigned char *), size_t>> hashFunctions);
    /**
     * This is deconstructor of Bloom_filter class.
     */
    ~Bloom_filter();
    /**
     * This method allows to add element in the bitfield.
     * @param unsigned char *elt : This is the element to add.
     */
    void addElement(unsigned char *elt);
    /**
     * This method allows to know if the element is added to the bitfield.
     * @param unsigned char *elt : This is the element to test.
     */
    bool contains(unsigned char *elt);

private:
    unsigned char *bitfield;
    size_t m;
    vector<pair<unsigned char *(*)(const unsigned char *, size_t, unsigned char *), size_t>> hashFunctions;
};

#endif
