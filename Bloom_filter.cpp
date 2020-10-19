// Sébastien Leray - M2 Cyber

#include <openssl/sha.h>
#include <string.h>
#include <gmp.h>
#include "Bloom_filter.h"

using namespace std;
using Hash_Function_t = unsigned char *(*)(const unsigned char *, size_t, unsigned char *);

/**
 * This is constructor of Bloom_filter class.
 * @param unsigned int m : The value must be a multiple of eight.
 * @param vector<pair<unsigned char *(*)(const unsigned char *, size_t, unsigned char *), size_t>> hashFunctions : This is the list of hash functions.
 */
Bloom_filter::Bloom_filter(size_t m, vector<pair<Hash_Function_t, size_t>> hashFunctions)
{
    // We create the bitfield
    this->bitfield = (unsigned char *)malloc(sizeof(unsigned char) * (m / 8));
    memset(this->bitfield, 0, (m / 8));
    this->m = m;
    this->hashFunctions = hashFunctions;
}

/**
 * This is deconstructor of Bloom_filter class.
 */
Bloom_filter::~Bloom_filter()
{
    free(this->bitfield);
}

/**
 * This method allows to add element in the bitfield.
 * @param unsigned char *elt : This is the element to add.
 */
void Bloom_filter::addElement(unsigned char *elt)
{
    // We use mpz because the output of hash functions is very big
    mpz_t indexMPZ;
    mpz_init(indexMPZ);

    for (vector<pair<Hash_Function_t, size_t>>::iterator it = this->hashFunctions.begin(); it != this->hashFunctions.end(); it++)
    {
        size_t hashFunctionSize = (*it).second;

        unsigned char digest[hashFunctionSize];

        // We execute the hash function on elt
        (*it).first((unsigned const char *)elt, strlen((const char *)elt), digest);

        mpz_import(indexMPZ, hashFunctionSize, -1, 1, 0, 0, digest);
        mpz_mod_ui(indexMPZ, indexMPZ, (this->m));

        unsigned long index = mpz_get_ui(indexMPZ);

        unsigned long numIndexArrayOfChar = index / 8;

        // We get the octet
        unsigned char y = this->bitfield[numIndexArrayOfChar];

        unsigned long pow = ((8 - ((index + 1) % 8)) % 8);

        mpz_t dec;
        mpz_init(dec);

        mpz_set_ui(dec, 2);
        mpz_pow_ui(dec, dec, pow);
        mpz_mul_ui(dec, dec, 1);

        y = y | mpz_get_ui(dec);

        mpz_clear(dec);

        this->bitfield[numIndexArrayOfChar] = y;
    }

    mpz_clear(indexMPZ);
}

/**
 * This method allows to know if the element is added to the bitfield.
 * @param unsigned char *elt : This is the element to test.
 */
bool Bloom_filter::contains(unsigned char *elt)
{
    mpz_t indexMPZ;
    mpz_init(indexMPZ);

    for (vector<pair<Hash_Function_t, size_t>>::iterator it = this->hashFunctions.begin(); it != this->hashFunctions.end(); it++)
    {
        size_t hashFunctionSize = (*it).second;

        unsigned char digest[hashFunctionSize];

        // We execute the hash function on elt
        (*it).first((unsigned const char *)elt, strlen((const char *)elt), digest);

        mpz_import(indexMPZ, hashFunctionSize, -1, 1, 0, 0, digest);
        mpz_mod_ui(indexMPZ, indexMPZ, (this->m));

        unsigned long index = mpz_get_ui(indexMPZ);

        unsigned long numIndexArrayOfChar = index / 8;

        unsigned char y = this->bitfield[numIndexArrayOfChar];

        unsigned long pow = ((8 - ((index + 1) % 8)) % 8);

        mpz_t dec;
        mpz_init(dec);

        mpz_set_ui(dec, 2);
        mpz_pow_ui(dec, dec, pow);
        mpz_mul_ui(dec, dec, 1);

        y = y & mpz_get_ui(dec);

        if (y == 0)
        {
            mpz_clear(dec);
            mpz_clear(indexMPZ);
            return false;
        }

        mpz_clear(dec);
    }

    mpz_clear(indexMPZ);

    return true;
}