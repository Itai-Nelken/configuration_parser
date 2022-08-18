#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>
#include "token.h"

typedef struct scanner {
    char *source;
    int line;
    size_t start, current;
} Scanner;

/***
 * Initialize a Scanner.
 * 
 * @param s A Scanner to initialize.
 * @param source The source.
 ***/
void scannerInit(Scanner *s, char *source);

/***
 * Free a Scanner.
 * 
 * @param s A Scanner to free.
 ***/
void scannerFree(Scanner *s);

/***
 * Scan the next token.
 * 
 * @param s An initialized Scanner.
 * @return The next token.
 ***/
Token scannerNextToken(Scanner *s);

#endif // SCANNER_H
