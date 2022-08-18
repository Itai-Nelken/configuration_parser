#include <stdio.h>
#include <stdlib.h> // strtol, size_t
#include <string.h> // memcmp
#include <stdarg.h>
#include <assert.h>
#include <stdint.h> // int64_t
#include <stdbool.h>
#include "token.h"
#include "scanner.h"

void scannerInit(Scanner *s, char *source) {
    s->source = source;
    s->start = s->current = 0;
    s->line = 1;
}

void scannerFree(Scanner *s) {
    s->source = NULL;
    s->start = s->current = 0;
    s->line = 0;
}

// helpers

static inline bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static inline bool isAscii(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static void error(Scanner *s, const char *format, ...) {
    va_list ap;
    fprintf(stderr, "[line %d at %zu] Error: ", s->line, s->current);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
}

static inline Token make_token(Scanner *s, TokenType type) {
    return tokenNew(type, s->line, s->start);
}

static inline bool is_end(Scanner *s) {
    return s->source[s->current] == '\0';
}

static inline char advance(Scanner *s) {
    return s->source[s->current++];
}

static inline char peek(Scanner *s) {
    return s->source[s->current];
}

static void skip_whitespace(Scanner *s) {
    for(;;) {
        switch(peek(s)) {
            case ' ':
            case '\r':
            case '\t':
                advance(s);
                break;
            case '#':
                // comment, skip until end of line.
                while(!is_end(s) && peek(s) != '\n') {
                    advance(s);
                }
                advance(s); // consume the newline
                break;
            default:
                return;
        }
    }
}

static Token scan_number(Scanner *s) {
    // TODO: add hex, octal, and binary literals.
    while(!is_end(s) && (isDigit(peek(s)) || peek(s) == '_')) {
        advance(s);
    }
    int64_t value = strtol(&s->source[s->start], NULL, 10);
    Token tk = make_token(s, TK_NUMBER);
    tk.as.number = value;
    return tk;
}

static TokenType scan_boolean_or_identifier_type(Scanner *s) {
    while(!is_end(s) && (isAscii(peek(s)) || isDigit(peek(s)) || peek(s) == '_')) {
        advance(s);
    }
    char *lexeme = s->source + s->start;
    size_t length = s->current - s->start;

    switch(lexeme[0]) {
        case 't': return (length == 4 && memcmp(lexeme, "true", 4) == 0) ? TK_TRUE : TK_IDENTIFIER;
        case 'f': return (length == 5 && memcmp(lexeme, "false", 5) == 0) ? TK_FALSE : TK_IDENTIFIER;
        default:
            break;
    }
    return TK_IDENTIFIER;
}

Token scan_token(Scanner *s) {
    skip_whitespace(s);
    s->start = s->current;
    if(is_end(s)) {
        return make_token(s, TK_EOF);
    }

    char c = advance(s);
    if(isDigit(c)) {
        return scan_number(s);
    }
    if(isAscii(c) || c == '_') {
        Token tk = make_token(s, scan_boolean_or_identifier_type(s));
        tk.as.string_or_id.txt = s->source + s->start;
        tk.as.string_or_id.length = s->current - s->start;
        return tk;
    }

    switch(c) {
        case '[': return make_token(s, TK_LBRACKET);
        case ']': return make_token(s, TK_RBRACKET);
        case '=': return make_token(s, TK_EQUAL);
        case '\n': return make_token(s, TK_NEWLINE);
        case '"': {
            while(!is_end(s) && peek(s) != '"') {
                advance(s);
            }
            // consume the last double-quote.
            advance(s);
            Token tk = make_token(s, TK_STRING);
            // + 1 to trim the leading double-quote
            tk.as.string_or_id.txt = s->source + s->start + 1;
            // - 2 to trim the double-quotes.
            tk.as.string_or_id.length = (int)(s->current - s->start - 2);
            return tk;
        }
        default:
            break;
    }
    error(s, "Unknown character '%c'", c);
    return make_token(s, TK_ERROR);
}


Token scannerNextToken(Scanner *s) {
    if(!s->source) {
        return tokenNew(TK_EOF, 0, 0);
    }
    return scan_token(s);
}
