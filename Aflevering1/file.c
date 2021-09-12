#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>


int print_error(char* path, int errnum) {
    return fprintf(stdout, "%s: cannont determine (%s)\n", path, strerror(errnum));
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage: file path\n" );
        return 1;
    }

    assert(argc == 2);

    char* inputFile = argv[1];

    FILE* file = fopen(inputFile, "r");
    char* fileType;

    int errnoCheck = errno;
    if (errnoCheck != 0) {
        print_error(argv[1], errnoCheck);
        return 0;
    }

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
        int asciiCodeChar = (int) asciiChar;
        printf(" %d = %c ", asciiCodeChar, asciiChar);

        // skal vi lave errnoChecks over det hele???
        

        // hvad sker der for a,ø,å på bitform??
        if(asciiCodeChar > 128) {
            checkAscii = 1;
        }

        if(asciiCodeChar > 128 || asciiCodeChar < 159 && asciiCodeChar > 256) {
            checkISO8859 = 1;
        }

        if (read == 0) {
            break;
        }
    }


    // UTF-Unicode, hvad i al verden??
    if (read == 0) {
        fileType = "empty";
    } else if (checkAscii == 0) {
        fileType = "ascii";
    } else if (checkISO8859 == 0) {
        fileType = "ISO8859";
    } else {
        fileType = "data";
    }

    fprintf(stdout, "%s: %s\n", inputFile, fileType);
    return 0;
}