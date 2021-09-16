#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


int print_error(char* path, int errnum) {
    return fprintf(stdout, "%s: cannont determine (%s)\n", path, strerror(errnum));
}

int errnoCheck(char* path) {
    int errnoTemp = errno;
    if (errnoTemp != 0) {
        print_error(path, errnoTemp);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        fprintf(stderr, "Usage: file path\n" );
        return EXIT_FAILURE;
    }

    assert(argc == 2);

    char* inputFile = argv[1];

    FILE* file = fopen(inputFile, "r");
    errnoCheck(argv[1]);
    char* fileType;

    char emptyDocument;
    int read = fread(&emptyDocument, 1, 1, file);
    
    fclose(file);
    file = fopen(inputFile, "r");
    errnoCheck(argv[1]);

    int checkAscii = 0;
    int checkISO8859 = 0;
    int checkUTF8 = 0;

    while(1) {
        unsigned char asciiChar;
        int read = fread(&asciiChar, 1, 1, file);
        errnoCheck(argv[1]);
        int asciiCodeChar = (int) asciiChar;
        
        if (read == 0) {
            break;
        }

        // printf(" %c = %d ", asciiChar, asciiCodeChar);

        // fix det rigtige interval
        if(asciiCodeChar > 127 || asciiCodeChar < 32) {
            if(asciiCodeChar > 13 || asciiCodeChar < 7) {
                if(asciiCodeChar != 27) {
                    checkAscii = 1;
                }
            }  
        }


        if(asciiCodeChar > 127 || asciiCodeChar < 32) {
            if(asciiCodeChar > 13 || asciiCodeChar < 7) {
                if(asciiCodeChar != 27) {
                    if(asciiCodeChar > 255 || asciiCodeChar < 160) {
                        checkISO8859 = 1;
                    }
                }
            }  
        }
    }

    fclose(file);
    file = fopen(inputFile, "r");
    errnoCheck(argv[1]);

    while(1) {
        unsigned char UTF8;
        int read = fread(&UTF8, 1, 1, file);
        errnoCheck(argv[1]);
        int newValue4Byte = UTF8 >> 3;
        int newValue3Byte = UTF8 >> 4;
        int newValue2Byte = UTF8 >> 5;
        int newValue1Byte = UTF8 >> 7;

        if (read == 0) {
            break;
        }

        if (UTF8 < 7) {
            checkUTF8 = 1;
        }       

        if(newValue4Byte == 30) {
            read = fread(&UTF8, 1, 1, file);
            newValue4Byte = UTF8 >> 6;
            if (newValue4Byte == 2) {
                read = fread(&UTF8, 1, 1, file);
                newValue4Byte = UTF8 >> 6;
                if (newValue4Byte == 2) {
                    read = fread(&UTF8, 1, 1, file);
                    newValue4Byte = UTF8 >> 6;
                    if (newValue4Byte == 2) {
                        
                    } else {
                        checkUTF8 = 1;
                        break;
                    }
                } else {
                    checkUTF8 = 1;
                    break;
                }
            } else {
                checkUTF8 = 1;
                break;
            }
        } else if (newValue3Byte == 14) {
            read = fread(&UTF8, 1, 1, file);
            newValue3Byte = UTF8 >> 6;
            if (newValue3Byte == 2) {
                read = fread(&UTF8, 1, 1, file);
                newValue3Byte = UTF8 >> 6;
                if (newValue3Byte == 2) {
                    
                } else {
                    checkUTF8 = 1;
                    break;
                }
            }
        } else if (newValue2Byte == 6) {
            read = fread(&UTF8, 1, 1, file);
            newValue2Byte = UTF8 >> 6;
            if (newValue2Byte == 2) {
                
            } else {
                checkUTF8 = 1;
                break;
            }
        } else if (newValue1Byte == 0) {
            
        } else {
            checkUTF8 = 1;
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
    } else if (checkUTF8 == 0) {
        fileType = "UTF-8 Unicode text";
    } else {
        fileType = "data";
    }

    fprintf(stdout, "%s: %s\n", inputFile, fileType);
    return EXIT_SUCCESS;
}
