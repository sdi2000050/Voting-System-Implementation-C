#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mvote.h"

int main(int argc, char *argv[]) {
    
    if (argc != 5) {                                    // Check if the correct number of command line arguments is provided
        fprintf(stderr, "Usage: %s -f registeredvoters -b bucketentries\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *input_file = NULL;                            // Parse command line arguments
    int m = 0;

    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-f") == 0) {
            input_file = argv[i + 1];
        } else if (strcmp(argv[i], "-b") == 0) {
            m = atoi(argv[i + 1]);
        } else {
            fprintf(stderr, "Invalid option: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    if (input_file == NULL || m <= 0) {                    // Check if input_file and m are provided
        fprintf(stderr, "Invalid command line arguments.\n");
        exit(EXIT_FAILURE);
    }

    
    FILE *file = fopen(input_file, "r");                    // Open and process the input file 
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", input_file);
        exit(EXIT_FAILURE);
    }
  
    int numVoters = 0;
    Voter* voters = get_voters(file,&numVoters);

    prompt_implementation(voters,m,&numVoters);               // Start the prompt

    fclose(file);

    return 0;
}
