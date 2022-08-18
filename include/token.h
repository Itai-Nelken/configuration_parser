#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h> // size_t

// Update tokenPrint(), tokenTypeString() and token_type_name() in Token.c when adding new token types.
typedef enum token_type {
    // One character tokens
    TK_LBRACKET, TK_RBRACKET,
    TK_EQUAL,
    TK_NEWLINE,

    // Literals
    TK_NUMBER,
    TK_TRUE,
    TK_FALSE,
    TK_STRING,

    // keywords
    TK_IDENTIFIER,

    // Other
    TK_ERROR,
    TK_EOF
} TokenType;


typedef struct token {
    TokenType type;
    int line, at;
    union {
        int64_t number;
        struct {
            char *txt;
            size_t length;
        } string_or_id;
    } as;
} Token;

/***
 * Make a new base Token.
 *
 * @param type A TokenType.
 * @param line The line in which the token occurs.
 * @param at The first character's location.
 * @return A new base Token.
 ***/
Token tokenNew(TokenType type, int line, int at);

/***
 * Return the equivalent of a TokenType as a string.
 *
 * @param type A TokenType.
 * @return The equivalent of the TokenType as a string.
 ***/
const char *tokenTypeString(TokenType type);

#endif // TOKEN_H
