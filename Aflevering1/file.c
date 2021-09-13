#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>


int print_error(char* path, int errnum) {
    return fprintf(stdout, "%s: cannont determine (%s)\n", path, strerror(errnum));
}

int errnoCheck(char* path) {
    int errnoTemp = errno;
    if (errnoTemp != 0) {
        print_error(path, errnoTemp);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage: file path\n" );
        return 1;
    }

    assert(argc == 2);

    char* inputFile = argv[1];

    FILE* file = fopen(inputFile, "r");
    errnoCheck(argv[1]);
    char* fileType;
    
    // if (file == NULL) {
    //     fprintf(stdout, "No file with that name exists\n");
    //     return 0;
    // }

    char emptyDocument;
    int read = fread(&emptyDocument, 1, 1, file);

    int checkAscii = 0;
    int checkISO8859 = 0;

    while(1) {
        char asciiChar;
        int read = fread(&asciiChar, 1, 1, file);
        errnoCheck(argv[1]);
        int asciiCodeChar = (int) asciiChar;
        
        // printf(" %c = %d ", asciiChar, asciiCodeChar);

        // skal vi lave errnoChecks over det hele???
        
        // hvad sker der for a,ø,å på bitform??
        // fix det rigtige interval
        if(asciiCodeChar > 127 || asciiCodeChar < 7) {
            checkAscii = 1;
        }

        if(asciiCodeChar > 127 || asciiCodeChar < 7) {
            if(asciiCodeChar > -1 || asciiCodeChar < -159) {
                checkISO8859 = 1;
            }
        }

        if (read == 0) {
            break;
        }
    }


    // UTF-Unicode, hvad i al verden??
    // prøv at kig på bits og deres startværdier, de skal nemlig være noget specifikt, for at det kan være UTF
    if (read == 0) {
        fileType = "empty";
    } else if (checkAscii == 0) {
        fileType = "ASCII text";
    } else if (checkISO8859 == 0) {
        fileType = "ISO-8859 text";
    } else {
        fileType = "data";
    }

    fprintf(stdout, "%s: %s\n", inputFile, fileType);
    return 0;
}
