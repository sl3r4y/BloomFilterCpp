// Sébastien Leray - M2 Cyber

#include <iostream>
#include <string.h>
#include <openssl/sha.h>
#include <stddef.h>
#include <ctime>
#include <chrono>
#include <sys/stat.h>
#include "Bloom_filter.h"

using namespace std;
using Hash_Function_t = unsigned char *(*)(const unsigned char *, size_t, unsigned char *);

/**
 * This method allows to get the list contained in file.
 * @param char *filename : The name of the file.
 * @param unsigned char *buffer : The buffer which will contain the data.
 * @param size_t bufferSize : The size of the buffer.
 */
void getListFromFile(char *filename, unsigned char *buffer, size_t bufferSize)
{

    FILE *file;

    if ((file = fopen(filename, "r")) == NULL)
    {
        cerr << "The file does not exist !" << endl;
        exit(1);
    }

    if (fread(buffer, 1, bufferSize - 1, file) <= 0)
    {
        cerr << "An error is occurred while reading the file !" << endl;
        exit(1);
    }

    if (fclose(file) != 0)
    {
        cerr << "An error is occurred while closing the file !" << endl;
    }
}

/**
 * This method allows to get the size in bytes of the file.
 * @param char *filename : The name of the file.
 */
size_t getSizeOfFileInBytes(char *filename)
{
    struct stat st;
    stat(filename, &st);

    // Allows to get all contents of the file in one buffer
    return st.st_size + 1;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        cerr << "It miss argument(s) !" << endl;
        cout << "Usage: bloom_filter file bitFieldSize" << endl;
        exit(1);
    }

    char filename[20];

    strncpy(filename, argv[1], 19);

    size_t bufferSize = getSizeOfFileInBytes(filename);

    unsigned char *buffer = (unsigned char *)malloc(sizeof(unsigned char) * bufferSize);

    getListFromFile(filename, buffer, bufferSize);

    // Must be a multiple of eight
    size_t sizeOfBitfieldInBits = atoll(argv[2]);

    vector<pair<Hash_Function_t, size_t>> hashFunctions;

    hashFunctions.push_back(pair<Hash_Function_t, size_t>(SHA1, SHA_DIGEST_LENGTH));
    hashFunctions.push_back(pair<Hash_Function_t, size_t>(SHA512, SHA512_DIGEST_LENGTH));
    hashFunctions.push_back(pair<Hash_Function_t, size_t>(SHA384, SHA384_DIGEST_LENGTH));

    Bloom_filter bloom_filter = Bloom_filter(sizeOfBitfieldInBits, hashFunctions);

    vector<unsigned char *> L;

    unsigned char *token = (unsigned char *)strtok((char *)buffer, " ");

    int nbElement = 0;

    // In this loop, we created the list with elements from buffer variable
    while (token != NULL)
    {
        unsigned char tmp[17];

        strncpy((char *)tmp, (const char *)token, 15);

        // Here, we add each elements in L
        L.push_back((unsigned char *)tmp);

        // And here, we add element in bitfield
        bloom_filter.addElement((unsigned char *)tmp);

        // We go to the next element
        token = (unsigned char *)strtok(NULL, " ");

        nbElement++;
    }

    free(buffer);

    /***********************************************
     *************  TIMING TEST PART  **************
     ***********************************************/

    // The element for the test is "blanc"
    unsigned char *testElt = (unsigned char *)"blanc";

    /*********** Part with Bloom filter ************/

    chrono::time_point<chrono::system_clock> start, end;

    start = chrono::system_clock::now();

    bool presentOrNot = bloom_filter.contains(testElt);

    end = chrono::system_clock::now();

    chrono::duration<double> elapsed_seconds = end - start;



    /** Part with browse each element of the list **/

    chrono::time_point<chrono::system_clock> start2, end2;
    chrono::duration<double> elapsed_seconds2;

    bool presentOrNot2 = false;

    start2 = chrono::system_clock::now();

    for (vector<unsigned char *>::iterator it = L.begin(); it < L.end(); it++)
    {
        if (strncmp((const char *)testElt, (const char *)(*it), strlen((const char *)testElt)) == 0)
        {
            end2 = chrono::system_clock::now();
            elapsed_seconds2 = end2 - start2;
            presentOrNot2 = true;
            break;
        }
    }

    if (!presentOrNot2)
    {
        end2 = chrono::system_clock::now();
        elapsed_seconds2 = end2 - start2;
    }

    cout << "***********************************************" << endl;
    cout << "* The size of the list : " << nbElement << endl;
    cout << "***********************************************" << endl;
    cout << "* Part with Bloom filter :" << endl;
    cout << "* " << testElt << " contains in the list : " << presentOrNot << endl;
    cout << "* Elapsed time: " << elapsed_seconds.count() << "s" << endl;
    cout << "***********************************************" << endl;
    cout << "* Part with browse each element of the list :" << endl;
    cout << "* " << testElt << " contains in the list : " << presentOrNot2 << endl;
    cout << "* Elapsed time: " << elapsed_seconds2.count() << "s" << endl;
    cout << "***********************************************" << endl;

    return 0;
}
