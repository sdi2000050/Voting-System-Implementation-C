typedef struct voter{
    int Pin;
    char* fname;
    char* lname;
    int zcode;
    char vote;                  // Y/N if the voter has voted or not
}Voter;

typedef struct key{
    int Pin;
    Voter* v;                   // Pointer to the informations of the voter with Pin 
}Key;

typedef struct listvoters{
    Voter* v;
    struct listvoters* nextv;
}ListV;

typedef struct zipcode{
    int countvote;              // Number of voters of zip
    int zip;
    ListV* lv;                  // List with the voters of zip
    struct zipcode* next;
}ZipList;

typedef struct bucket{
    Key* keys;
    struct bucket* nextB;       // Pointer to the next overflow bucket 
    int cap;                    // Capacity of the bucket
}Bucket;

typedef struct hashtable{
    Bucket** table;
    int num_buckets;
    int round;
    int p;
    int num_keys;
    int prev_num_buck;
}HashT;