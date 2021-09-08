#include <stdio.h>
#include <assert.h>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage: file path\n" );
        return 1;
    }

    assert(argc == 2);

    char* inputFile = argv[1];

    FILE* file = fopen(inputFile, "r");
    char* fileType;

    char emptyDocument;
    int read = fread(&emptyDocument, 1, 1, file);

    int checkAscii = 0;

    while(1) {
        char asciiChar;
        int read = fread(&asciiChar, 1, 1, file);
        int asciiCodeChar = (int) asciiChar;
        
        if(asciiCodeChar > 128) {
            checkAscii = 1;
        }

        if (read == 0) {
            break;
        }
    }

    if (read == 0) {
        fileType = "empty";
    } else if (checkAscii == 0) {
        fileType = "ascii";
    } else {
        fileType = "data";
    }

    if (file == NULL) {
        fprintf(stdout, "No file with that name exists");
        return 0;
    } else {
        fprintf(stdout, "%s: %s\n", inputFile, fileType);
        return 0;
    }
}