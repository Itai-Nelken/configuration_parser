#include <stdio.h>
#include <assert.h>
#include "token.h"


Token tokenNew(TokenType type, int line, int at) {
    Token tk = {
        .type = type,
        .line = line,
        .at = at,
        .as = {.string_or_id = {0, 0}}
    };
    return tk;
}

const char *tokenTypeString(TokenType type) {
    static const char *strings[] = {
        [TK_LBRACKET]    = "[",
        [TK_RBRACKET]    = "]",
        [TK_EQUAL]       = "=",
        [TK_NEWLINE]     = "<newline>",
        [TK_NUMBER]      = "<number>",
        [TK_TRUE]        = "true",
        [TK_FALSE]       = "false",
        [TK_STRING]      = "<stringr>",
        [TK_IDENTIFIER]  = "<identifier>",
        [TK_ERROR]       = "<error>",
        [TK_EOF]         = "<eof>"
    };
    return strings[(int)type];
}
