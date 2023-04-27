#include <stdlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include "parser.h"
struct Parser {
    FILE *f;
    char current_line[MAX_PARSER_LINE_LENGTH + 1];
    uint32_t line_offset;                           //index value of current line
};

static void my_set_string(char *res, char *string) {
    uint32_t i;
    for (i = 0; string[i] != '\0'; i += 1) {
        res[i] = string[i];
    }
    res[i] = '\0';
    return;
}

static void add_char(char *w, char c) {
    int i;
    for (i = 0; w[i] != '\0'; i++);
    w[i] = c;
    w[i + 1] = '\0';
    return; 
}

static void emptyS(char *s) {
    for (uint32_t i = 0; s[i]; i += 1) {
        s[i] = '\0';
    }
    return;
}

static int get_strlen(char *s) {
    int i;
    for (i = 0; s[i]; i += 1);
    return i;
}

Parser *parser_create(FILE *f) {
    Parser *p = (Parser *) malloc(sizeof(Parser));
    if (p) {
        p->f = f;
        p->line_offset = 0;
        if (!fgets(p->current_line, MAX_PARSER_LINE_LENGTH +1, f)) {
            free(p);
            p = NULL;
        }
    }
    return p;
}

void parser_delete(Parser **p) {
    free(*p);
    *p = NULL;
    return;
}

//''' = 45, '-' = 39, ' ' = 32
//errors to fix:
//doesn't count endline word as true
//word must be allocated
bool next_word(Parser *p, char *word) {
    /**
    if (p->line_offset > MAX_PARSER_LINE_LENGTH) {
        p->line_offset = 0;
        emptyS(p->current_line);
        fgets(p->current_line, MAX_PARSER_LINE_LENGTH + 1, p->f);
        return false;
    }
    **/
    emptyS(word);
    char buffer[MAX_PARSER_LINE_LENGTH + 1] = "", c;
    while (true) {
        c = p->current_line[p->line_offset];
        if (!c) {
            if (get_strlen(buffer) == 0) {
                //printf("breaks on line:\n'%s'\n", p->current_line);
                break;
            } else {
                p->line_offset += 1;
                my_set_string(word, buffer);
                return true;
            }
        } else if (c == ' ' || c == '\n') {
            if (get_strlen(buffer) == 0) {
                p->line_offset += 1;
                return true;
            } else {
                p->line_offset += 1;
                my_set_string(word, buffer);
                return true;
            }
        } else {
            c = tolower(c);
            if (isalnum(c) == 0 && c != 39 && c != 45) {
                p->line_offset += 1;
                my_set_string(word, buffer);
                return true;
            } else {
                add_char(buffer, c);
                p->line_offset += 1;
            }
        }
    }
    /**
    p->line_offset = 0;
    emptyS(p->current_line);
    if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH + 1, p->f)) {
        return true;
    } else {
        return false;
    }
    **/
    word = NULL;
    return false;
}
