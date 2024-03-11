#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mvote_structs.h"

//******Get voters data from file******
Voter* get_voters(FILE* file, int* numVoters);

//******Command Prompt Implementation******
void prompt_implementation(Voter* voters, int m, int* numVoters);

//******Command: l <pin>******
Voter* get_voter_info(HashT* HT, int pin, int m);

//******Command: m <pin>******
ZipList* commandm (HashT* HT, int m, int pin, int* count, ZipList* ZL, int* numzips);

//******Command: o******
void sort_list (ZipList* ZL);
void swaplistnodes(ZipList* a, ZipList* b);
void printZipList(ZipList* ZL);

//******Inverted Index******
ZipList* insertinList(ZipList* ZL, int* num, Voter* v);

//******Linear Hashing******
Bucket* initializeBucket(int m);
HashT* initializeHashTable(int m);
int hashfunction(int k, int round, int m);
HashT* splitBucket(HashT* HT); 
int isfull(Bucket* b, int m);
HashT* insert_key(HashT* HT, int hashv, int m, Key key);
HashT* insert(HashT* HT, Key key, int m);
HashT* splitBucket(HashT* HT);
