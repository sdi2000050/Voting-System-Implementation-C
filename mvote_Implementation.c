#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "mvote.h"

size_t bytes = 0;                   // Global variable to calculate the total allocated bytes

//************************Get voters data from file************************

Voter* get_voters(FILE* file, int* numVoters){
    char line[100];                                         // Assuming a maximum line length of 100 characters

    while (fgets(line, sizeof(line), file) != NULL) {       // Count the number of lines in the file (number of voters)
        (*numVoters)++;
    }

    
    Voter* voters = (Voter*) malloc((*numVoters) * sizeof(Voter));       // Allocate memory for the array of Voter structures
    bytes += ((*numVoters) * sizeof(Voter));
    if (voters == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    rewind(file);                                             // Rewind the file to read data again

    for (int i = 0; i < (*numVoters); i++) {                     // Read and parse data into the voters array
        fgets(line, sizeof(line), file);
        sscanf(line, "%d %ms %ms %d", &voters[i].Pin, &voters[i].fname, &voters[i].lname, &voters[i].zcode);
        voters[i].vote='N';
    }

    return voters;
}

//************************Command Prompt Implementation************************

void prompt_implementation(Voter* voters, int m, int* numVoters){
    HashT* HT = initializeHashTable(m);

    Key key;
    for (int i = 0; i < *numVoters; i++) {                  //Insert the voters of file in the hash table
        key.Pin=voters[i].Pin;
        key.v=&voters[i];
        HT = insert(HT, key, m);        
    }

    int numzips = 0;
    ZipList* ZL = (ZipList*) malloc(sizeof(ZipList));
    bytes += sizeof(ZipList);
    if (ZL == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    char input[100];                            // Assuming a maximum input length of 100 characters
    int exit_flag = 0;                          // Flag to determine when to exit the prompt loop
    int count=0;
    while (!exit_flag) {
        printf("mvote-promt >> ");              // Print the command prompt
        fgets(input, sizeof(input), stdin);     // Read user's input
                       
        input[strcspn(input, "\n")] = 0;        // Remove newline character
        
        //Command: l <pin>
        if (strncmp(input, "l ", 2) == 0) {  
            int pin;                                   
            if (sscanf(input, "l %d", &pin) == 1) {             // Check if Pin is integer
                if (pin >= 0){                                  // Check if Pin is positive  
                    Voter* vtr = get_voter_info(HT,pin,m);
                    if ( vtr != NULL ) {
                        printf("%d %s %s %d %c\n",vtr->Pin,vtr->lname,vtr->fname,vtr->zcode,vtr->vote);
                    }else{
                        printf("Participant %d not in cohort\n", pin);
                    } 
                }else {
                    printf("Malformed Pin\n");                  // Malformed Input (PIN)
                }
            } else {
                printf("Malformed Pin\n");                      // Malformed Input (PIN)
            }
        } //Command: i <pin> <lname> <fname> <zip> 
        else if (strncmp(input, "i ", 2) == 0) {
            int pin;
            char lname[100];                            // Assuming a maximum length of 100 for last and first name
            char fname[100];
            int zip;

            if (sscanf(input,"i %d %99s %99s %d", &pin, lname, fname, &zip) == 4) {         //Attempt to take the input arguments
                if ( pin>=0 ){                         // Check if Pin is positive
                    int valid_name=1;                  // Check if the names contain only letters

                    int i;
                    for(i=0; lname[i]!='\0'; i++){
                        if(!isalpha(lname[i])){
                            valid_name = 0;
                            break;
                        }
                    }
                    for(i=0; fname[i]!='\0'; i++){
                        if(!isalpha(fname[i])){
                            valid_name = 0;
                            break;
                        }
                    }

                    if (valid_name){
                        if(zip >= 0){                               // Check if zip is positive integer               
                            Voter* vtr = get_voter_info(HT,pin,m);  // All arguments are valid                    
                            if ( vtr != NULL ){                                  // Check if pin already exist
                                printf("%d already exist\n", pin);
                            }
                            else{
                                (*numVoters)++;
                                Voter* new_vtr = realloc(voters,(*numVoters) * sizeof(Voter));    // Put the new voter in the voters array
                                bytes += sizeof(Voter);
                                if (new_vtr == NULL) {
                                    printf("Memory allocation error\n");
                                    exit(EXIT_FAILURE);
                                }
                                voters = new_vtr;
                                i = (*numVoters) - 1;
                                voters[i].Pin = pin;
                                voters[i].fname = strdup(fname);
                                voters[i].lname = strdup(lname);
                                voters[i].zcode = zip;
                                voters[i].vote = 'N';
                                Key key;  
                                key.Pin = pin;
                                key.v = &(new_vtr[i]);
                                HT= insert(HT,key,m);                                       // Insert the new voter in the hash table
                                printf("Inserted %d %s %s %d N\n", pin, lname, fname, zip);
                            }
                        } else {
                            printf("Malformed Input\n");
                        }
                    }else {
                        printf("Malformed Input\n");
                    }
                } else {
                    printf("Malformed Input\n");
                }
            }else {
                printf("Malformed Input\n");
            }
        }//Command: m <pin> 
        else if (strncmp(input, "m ", 2) == 0) {
            int pin;      
            if (sscanf(input, "m %d", &pin) == 1) {           // Check if Pin is positive integer
                if (pin >= 0){     
                    ZL = commandm(HT,m,pin,&count,ZL,&numzips);
                }else {
                    printf("Malformed Pin\n");               // Malformed Input (PIN)
                } 
            } else {
                printf("Malformed Pin\n");                  // Malformed Input (PIN)
            }            
        } //Command: bv <fileofkeys>
        else if (strncmp(input, "bv ", 3) == 0) {
            char* fileofkeys ;
            char line[50];                                                  // Assuming a maximum line length of 50 characters
            int numkeys = 0;
            if (sscanf(input, "bv %ms", &fileofkeys) == 1) {

                FILE *file = fopen(fileofkeys, "r");                        // Open and process the file of keys 
                if (file == NULL) {
                    fprintf(stderr, "Error opening file: %s\n", fileofkeys);
                    exit(EXIT_FAILURE);
                }
                while (fgets(line, sizeof(line), file) != NULL) {           // Count the number of lines in the file (number of keys)
                    (numkeys)++;
                }               
                rewind(file); 
                                                              
                for (int i = 0; i < numkeys; i++) {                         // Rewind the file to read keys again       
                    fgets(line, sizeof(line), file);
                    int key;
                    if (sscanf(line, "%d", &key) == 1) {            // Check if Pin is positive integer
                        if(key>=0){
                            ZL = commandm(HT,m,key,&count,ZL,&numzips);
                        }else{
                            printf("Malformed Pin\n");
                            break;
                        }   
                    }else{
                        printf("Malformed Pin\n");
                        break;
                    }                    
                }
                fclose(file);
            }    
        } //Command: v
        else if (strcmp(input, "v") == 0) {
            printf("Voted so far %d\n", count);
        } //Command: perc
        else if (strcmp(input, "perc") == 0) {
            double perc = (double)count * 100/(double)(*numVoters); 
            printf("%f\n", perc);
        } //Command: z <zipcode>
        else if (strncmp(input, "z ", 2) == 0) {
            int zip;
            sscanf(input, "z %d", &zip);
            ZipList* current = ZL;                                   
            while(current->next != NULL){
                if (current->zip == zip){           // Find zip in list and print the pins
                    printf("%d voted in %d \n", current->countvote, current->zip);
                    ListV* curvtr = current->lv;
                    while (curvtr != NULL) {
                        printf("%d\n", curvtr->v->Pin);
                        curvtr = curvtr->nextv;
                    }
                }
                current = current ->next;
            }
            if (current->zip == zip){               // If zip in the last node
                printf("%d voted in %d \n", current->countvote, current->zip);
                ListV* curvtr = current->lv;
                while (curvtr != NULL) {
                    printf("%d\n", curvtr->v->Pin);
                    curvtr = curvtr->nextv;
                }
            }            
        } //Command: o
        else if (strcmp(input, "o") == 0) {              
            printZipList(ZL);
        } //Command: exit
        else if (strcmp(input, "exit") == 0) {
            exit_flag = 1;                              // Set the exit flag to terminate the loop
        } //Invalid command
        else {
            printf("Invalid command.\n");
        }

    }

    ZipList* current = ZL;
    while (current != NULL) {
        ListV* currentv = current->lv;
        while (currentv != NULL) {
            ListV* tempv = currentv;
            currentv = currentv->nextv;
            free(tempv);                              // Free the ListV node
        }
        ZipList* tempNode = current;
        current = current->next;
        free(tempNode);                               // Free the ZipList node
    }
    for (int i = 0; i < HT->num_buckets; i++) {
        Bucket* currentb = HT->table[i];
        while (currentb != NULL) {
            free(currentb->keys);                     // Free the keys array in the bucket
            Bucket* tempb = currentb;
            currentb = currentb->nextB;
            free(tempb);                              // Free the bucket structure
        }
    }
    free(HT->table);
    free(HT);
    free(voters);

    printf("%zu of Bytes Released\n", bytes);
}

//***********************Command: l <pin>************************

Voter* get_voter_info(HashT* HT, int pin, int m){
    int r = HT->round +1;
    int hashv;
    for(int i=0; i<=r; i++){
        hashv=hashfunction(pin, i, m);
        Bucket* currentB = HT->table[hashv];
        while(currentB != NULL){
            for(int j=0; j<m; j++){
                if(currentB->keys[j].Pin == pin){
                    return currentB->keys[j].v;
                }
            }
            currentB = currentB->nextB;
        }
    }
    return NULL;
}

//***********************Command: m <pin>************************

ZipList* commandm (HashT* HT, int m, int pin, int* count, ZipList* ZL, int* numzips){
    Voter* vtr = get_voter_info(HT,pin,m);
    if ( vtr != NULL ) {
        if (vtr->vote == 'N'){
            vtr->vote='Y';
            printf("|%d| Marked Voted\n", pin);
            (*count)++;
            ZL = insertinList(ZL,numzips,vtr);                  // Update the list with the new voter
            sort_list(ZL);                                      // Sort the list after the new insertion
        }
    }else{
        printf("%d does not exist\n", pin);
    } 
    return ZL;              
}

//***********************Command: ο************************

void sort_list (ZipList* ZL){
    int swap;
    ZipList* zl;
    ZipList* lzl = NULL;

    do{
        swap = 0;
        zl = ZL;
        while(zl->next != lzl){
            if (zl->countvote < zl->next->countvote){           // Check if the current node has smaller count than the next
                swaplistnodes(zl,zl->next);                     // Swap the nodes
                swap=1;
            }
            zl = zl->next;
        }
        lzl = zl;
    } while (swap); 

}

void swaplistnodes(ZipList* a, ZipList* b){
    int count = a->countvote;
    int zip = a->zip;
    ListV* lv = a->lv;

    a->countvote = b->countvote;
    a->zip = b->zip;
    a->lv = b->lv;

    b->countvote = count;
    b->zip = zip;
    b->lv = lv;

}

void printZipList(ZipList* ZL) {
    ZipList* current = ZL;
    while (current != NULL) {
        printf("%d %d\n", current->zip, current->countvote);
        current = current->next;
    }
}

//************************Inverted Index************************


ZipList* insertinList(ZipList* ZL, int* num, Voter* v){
    if((*num)==0){                                          // If the list is empty initialize list with first voter
        ZL->countvote = 1;
        ZL->lv = (ListV*) malloc (sizeof(ListV));
        bytes += sizeof(ListV);
        if (ZL->lv == NULL) {
            printf("Memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        ZL->lv->v = v;
        ZL->lv->nextv = NULL;
        ZL->zip = v->zcode;
        ZL->next = NULL;
        (*num)++;
        return ZL;
    }
    
    ZipList* current= ZL;                                   // If the list is not empty check if the zipcode of voter exist in list
    while(current->next != NULL){
        if (current->zip == v->zcode){
            ListV* curvtr = current->lv;                    // If zipcode already in list update the list of voters
            while(curvtr->nextv != NULL){
                curvtr = curvtr->nextv;                     // Find the end of the list
            }
            curvtr->nextv = (ListV*)malloc(sizeof(ListV));  // Allocate memory for the new voter node
            bytes += sizeof(ListV);
            if (curvtr->nextv == NULL) {
                printf("Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
            curvtr = curvtr->nextv;                         // Move to the newly allocated node
            curvtr->v = v;
            curvtr->nextv = NULL;
            current->countvote++;
            return ZL;
        }
        current = current->next;
    }
    if (current->zip == v->zcode){
        ListV* curvtr = current->lv;                    // If zipcode in last node
        while(curvtr->nextv != NULL){
            curvtr = curvtr->nextv;                     // Find the end of the list and add the voter
        }
        curvtr->nextv = (ListV*)malloc(sizeof(ListV));
        bytes += sizeof(ListV);
        if (curvtr->nextv == NULL) {
            printf("Memory allocation error\n");
            exit(EXIT_FAILURE);
        }  
        curvtr = curvtr->nextv;          
        curvtr->v = v;               
        curvtr->nextv = NULL;
        current->countvote++;
        return ZL;
    }                                                       
                                    
    current->next = (ZipList*) malloc (sizeof(ZipList));    // If zipcode of voter not in list add the new zipcode in the end of the list
    bytes += sizeof(ZipList);
    if (current->next == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    current->next->countvote = 1;
    current->next->lv = (ListV*) malloc (sizeof(ListV));
    bytes += sizeof(ListV);
    if (current->next->lv == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    current->next->lv->v = v;
    current->next->lv->nextv = NULL;
    current->next->zip = v->zcode;
    current->next->next = NULL;
    (*num)++; 
        
    return ZL;

}


//************************Linear Hashing************************

HashT* initializeHashTable(int m){
    HashT* HT = (HashT*)malloc(sizeof(HashT));
    bytes += sizeof(HashT);
    if (HT == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    HT->table = (Bucket**)malloc(m*sizeof(Bucket));     //Initialize the hash table
    bytes += m * sizeof(Bucket);
    if (HT->table == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    for(int i=0; i<m; i++){
        HT->table[i] = initializeBucket(m);
    }
    HT->num_buckets = m;
    HT->round = 0;
    HT->p = 0;
    HT->num_keys = 0;
    HT->prev_num_buck=m;
    return HT;
}

Bucket* initializeBucket(int m){
    Bucket* bucket = (Bucket*)malloc(sizeof(Bucket));
    bytes += sizeof(Bucket);
    if (bucket == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    bucket->keys = (Key*)malloc(m * sizeof(Key));
    bytes += m * sizeof(Key);
    if (bucket->keys == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < m; i++) {
        bucket->keys[i].Pin = 0;                    //Initialize the bucket with Pin value zero
        bucket->keys[i].v=NULL;
    }
    bucket->nextB = NULL;
    bucket->cap = m;
    return bucket;
}


int hashfunction(int k, int round, int m){
    int h=k % (int)(pow(2,round) * m);      // Hash Function : h(k) = k mod 2^round * m
    return h;
}


int isfull(Bucket* b, int m){
    int count=0;
    for(int i=0; i<m; i++){                 // Check if the current bucket is full
        if (b->keys[i].Pin != 0){
            count++;
        }
    }
    return count;
}

HashT* insert_key(HashT* HT, int hashv, int m, Key key){
    Bucket* currentb=HT->table[hashv];      
    int count = isfull(currentb, m);
    while (count == m) {
        if (currentb->nextB == NULL) {
            currentb->nextB = initializeBucket(m);      // If the current bucket is full and there isn't an overflown bucket create a new bucket   
        }
        currentb = currentb->nextB;                     // Search all the overflown buckets until find a not full one
        count = isfull(currentb, m);
    }
            
    int i=0;
    while (currentb->keys[i].Pin != 0){                 // Insert the key in the first empty space of the bucket
        i++;
    }
    currentb->keys[i] = key;

    return HT;
}

HashT* insert(HashT* HT, Key key, int m){ 

    int hashv = hashfunction(key.Pin, HT->round, m);            // Calculate the hash function
    
    if(hashv < HT->p){
        hashv = hashfunction(key.Pin, HT->round + 1, m);        // If hash value < p calculate the hash function for round + 1
    }

    HT=insert_key(HT,hashv,m,key);

    HT->num_keys++;

    double loadfactor = (double)HT->num_keys / (HT->num_buckets * m);    // If the load factor > 0,75 split the bucket p
    if (loadfactor > 0.75){
        HT = splitBucket(HT);
    }

    if(HT->num_buckets == HT->prev_num_buck * 2 ){               // When the hash table is double than when the round started update the p, round, previous number of buckets values 
        HT->p = 0;
        HT->round++;
        HT->prev_num_buck= HT->num_buckets;
    }

    return HT;
}

HashT* splitBucket(HashT* HT){
    int splitb = HT->p;
    int newb = HT->num_buckets;
    int m = HT->table[splitb]->cap;
    HT->num_buckets++;

    HashT* newHT = initializeHashTable(m);                    // Create a new hash table to reallocate the new bucket 
    newHT->table = (Bucket**) realloc(HT->table,sizeof(Bucket*)*HT->num_buckets);
    bytes += sizeof(Bucket);
    if (newHT->table == NULL) {
        printf("Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newHT->table[newb] = initializeBucket(m);
    newHT->p = HT->p;
    newHT->num_buckets = HT->num_buckets;                     // Update the bucket values correctly
    newHT->num_keys = HT->num_keys;
    newHT->round = HT->round;
    newHT->prev_num_buck = HT->prev_num_buck;

    Bucket* currentb=newHT->table[splitb];
    while(currentb != NULL){
        for (int i = 0; i < m; i++) {                         // Take all the buckets with pointer p and destribute the keys with hash function with round + 1
            Key key = currentb->keys[i];
            if (key.Pin != 0){
                int hashv = hashfunction(key.Pin, newHT->round+1, m);
                if (hashv != splitb) {
                    int j = 0;
                    while (newHT->table[hashv]->keys[j].Pin != 0) {
                        j++;
                    }
                    
                    newHT=insert_key(newHT,hashv,m,key);
                    
                    currentb->keys[i].Pin = 0;
                    currentb->keys[i].v = NULL;
                }
            }
        }
        currentb=currentb->nextB;
    }

    newHT->p++;

    return newHT;
}
