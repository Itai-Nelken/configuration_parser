#include <stdlib.h>
#include <string.h> // strdup
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"
#include "config.h"
#include "token.h"
#include "array.h"
#include "parser.h"

void free_literal(Literal l) {
    if(l.type == LIT_STRING) {
        free(l.as.string);
    }
}

void free_pair(Pair *p) {
    free(p->key);
    free_literal(p->value);
    free(p);
}

/* parser */
typedef struct parser {
    Scanner *scanner;
    Token previous_token, current_token;
    bool had_error;
} Parser;

static inline bool is_eof(Parser *p) {
    return p->current_token.type == TK_EOF;
}

static Token advance(Parser *p) {
    // skip error tokens as the scanner already reported them,
    // but set had_error to true.
    Token tk;
    while((tk = scannerNextToken(p->scanner)).type == TK_ERROR) {
        p->had_error = true;
    }
    p->previous_token = p->current_token;
    p->current_token = tk;
    return tk;
}

static inline Token peek(Parser *p) {
    return p->current_token;
}

static inline Token previous(Parser *p) {
    return p->previous_token;
}

static void error(Parser *p, const char *format, ...) {
    p->had_error = true;
    va_list ap;
    fprintf(stderr, "[line %d at %d] Error: ", previous(p).line, previous(p).at);
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    fputc('\n', stderr);
}

static bool consume(Parser *p, TokenType expected) {
    if(peek(p).type != expected) {
        error(p, "Expected '%s' but got '%s'.", tokenTypeString(expected), tokenTypeString(peek(p).type));
        return false;
    }
    advance(p);
    return true;
}

static bool match(Parser *p, TokenType expected) {
    if(peek(p).type != expected) {
        return false;
    }
    advance(p);
    return true;
}

#define TRY_CONSUME(parser, expected, on_fail) ({ \
                    if(!consume((parser), (expected))) { \
                        void *_tmp = (on_fail); \
                        if(_tmp) { free(_tmp); } \
                        return NULL; \
                    } \
                    true; \
                    })
static inline void skip_newlines(Parser *p) {
    while(!is_eof(p) && match(p, TK_NEWLINE)) /* nothing */ ;
}

static inline Pair *make_pair(char *key, Literal value) {
    Pair *p = calloc(1, sizeof *p);
    p->key = key;
    p->value = value;
    return p;
}

static inline ConfigTable *make_table(char *name, Array pairs) {
    ConfigTable *t = calloc(1, sizeof *t);
    t->name = name;
    t->pairs = pairs;
    return t;
}

static char *parse_identifier(Parser *p) {
    if(!consume(p, TK_IDENTIFIER)) {
        return NULL;
    }
    return strndup(previous(p).as.string_or_id.txt, previous(p).as.string_or_id.length);
}

static Literal parse_literal(Parser *p) {
    if(match(p, TK_NUMBER)) {
        return (Literal){
            .type = LIT_NUMBER,
            .as.number = previous(p).as.number
        };
    } else if(match(p, TK_TRUE)) {
        return (Literal){
            .type = LIT_BOOLEAN,
            .as.boolean = true
        };
    } else if(match(p, TK_FALSE)) {
        return (Literal){
            .type = LIT_BOOLEAN,
            .as.boolean = false
        };
    } else if(match(p, TK_STRING)) {
        return (Literal){
            .type = LIT_STRING,
            .as.string = strndup(previous(p).as.string_or_id.txt, previous(p).as.string_or_id.length)
        };
    } else {
        error(p, "Expected one of [<number>, true, false, <string>] but got '%s'.", tokenTypeString(peek(p).type));
    }
    return (Literal){
        .type = LIT_NONE
    };
}

static Pair *parse_pair(Parser *p) {
    char *key = parse_identifier(p);
    if(!key) {
        return NULL;
    }
    TRY_CONSUME(p, TK_EQUAL, key);
    Literal value = parse_literal(p);
    if(!consume(p, TK_NEWLINE)) {
        free(key);
        free_literal(value);
        return NULL;
    }
    return make_pair(key, value);
}

static ConfigTable *parse_table(Parser *p) {
   TRY_CONSUME(p, TK_LBRACKET, 0);
    char *name = parse_identifier(p);
    if(!name) {
        return NULL;
    }
    TRY_CONSUME(p, TK_RBRACKET, name);
    TRY_CONSUME(p, TK_NEWLINE, name);

    Array pairs;
    arrayInit(&pairs);
    while(!is_eof(p) && peek(p).type != TK_NEWLINE) {
        Pair *pair = parse_pair(p);
        if(pair) {
            arrayPush(&pairs, (void *)pair);
        }
    }
    return make_table(name, pairs);
}

#undef TRY_CONSUME

// literal    -> STRING | NUMBER | BOOLEAN
// pair       -> IDENTIFIER '=' literal
// table      -> '[' IDENTIFIER ']' NEWLINE (pair)+
// config     -> (table | pair)*
bool config_parser_parse(char *source, Array *tables) {
    Scanner scanner;
    scannerInit(&scanner, source);

    Parser p = {
        .scanner = &scanner,
        .previous_token = {.type = TK_ERROR},
        .current_token = {.type = TK_ERROR},
        .had_error = false
    };

    Array tmp;
    arrayInit(&tmp);
    ConfigTable *top_level = make_table(strdup("__toplevel__"), tmp);
    arrayPush(tables, (void *)top_level);

    advance(&p);
    // consume any newlines as they don't matter in the global scope.
    skip_newlines(&p);
    while(!is_eof(&p)) {
        switch(peek(&p).type) {
            case TK_LBRACKET: {
                ConfigTable *t = parse_table(&p);
                if(t) {
                    arrayPush(tables, (void *)t);
                }
                break;
            }
            case TK_IDENTIFIER: {
                Pair *pair = parse_pair(&p);
                if(pair) {
                    arrayPush(&top_level->pairs, (void *)pair);
                }
                break;
            }
            default:
                error(&p, "Only tables and pairs are allowed in the toplevel scope.");
                advance(&p); // so we don't get stuck in an infinite loop on the same token.'
                break;
        }
        // consume any remaining newlines.
        skip_newlines(&p);
    }

    scannerFree(&scanner);
    return !p.had_error;
}
