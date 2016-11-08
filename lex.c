#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lex.h"

struct token *lex_token(struct lexer *lex);

char *tt_strings[] = {
	[TT_STR]   = "string",
	[TT_NUM]   = "number",
	[TT_COMMA] = ",",
	[TT_AOPEN] = "[",
	[TT_ACLOSE] = "]",
	[TT_COLON] = ":",
	[TT_OOPEN] = "{",
	[TT_OCLOSE] = "}",
	[TT_TRUE] = "true",
	[TT_FALSE] = "false",
	[TT_NULL] = "null",
	[TT_END] = "END"
};

struct token *token_make(tt typ, char *st, int len, int line, int col) {
	struct token *tk = (struct token *)malloc(sizeof(struct token));
	
	tk->typ = typ;
	tk->str = st;
	tk->len = len;
	
	tk->line = line;
	tk->col = col;
	
	return tk;
}

// progress lexer onto next char
char advance(struct lexer *l) {

	switch (*l->pos) {
	case '\n':	
		l->line++;
		l->col = 1;
		break;
	case '\0':
		return '\0';
	default:
		l->col++;
	}
	
	return *(++l->pos);
}

// create new lexer; MUST: call lexer_delete() after
struct lexer *lex_make(const char *str) {
	struct lexer *l = (struct lexer *)malloc(sizeof(struct lexer));
	
	l->pos = l->str = str;
	l->len = 0;
	
	l->line = 1;
	l->col = 1;
	
	return l;
}

// emits token of type at lexer's current position
struct token *emit(struct lexer *l, tt typ) {
	struct token *tk = token_make(typ, l->pos, l->len, l->line, l->col);
	l->pos += l->len;
	
	l->len = 0;
	return tk;
}

struct token *lex_string(struct lexer *lex) {
	// TODO: check don't go past end of string
	// TODO: check quote is not preceded by backslash
	struct token *tk;
	
	if (*lex->pos != '"' && *lex->pos != '\'')
		return NULL;

	char delim = *lex->pos;
	
	advance(lex);
	
	int line = lex->line;
	int col = lex->col;
	
	while (*(lex->pos+lex->len) != '\0' && *(lex->pos+lex->len) != delim) {
		lex->len += 1;
		
		if (*(lex->pos+lex->len) == '\n') {
			line++;
			col = 1;
			continue;
		}
		
		col++;
	}

	tk = emit(lex, TT_STR);
	
	lex->line = line;
	lex->col = col;
	
	// skip past last double quote
	advance(lex); 
	return tk;
}

struct token *lex_number(struct lexer *lex) {
	// TODO: support floating point 
	
	struct token *tk;
	int col = lex->col;
	
	while (isdigit(*(lex->pos+lex->len))) {
		lex->len++;
		col++;
	}
	 
	// TODO: check conditions for end of number more carefully
	if (isalpha(*(lex->pos+lex->len)))
		return NULL;
		
	tk = emit(lex, TT_NUM);
	lex->col = col;
	
	return tk;
}

// return -1 on fail, 0 on success
struct token *match(struct lexer *lex, tt typ, char *str) {

	struct token *tk;
	int col = lex->col;
	int i, len = strlen(str);
	
	for (i = 0; i < len && *(lex->pos+i) != '\0'; i++) {
		if (*(lex->pos+i) != *str) {
			return NULL;
		}
		str++;
		col++;
	}
	
	lex->len = i;
	
	tk = emit(lex, typ);
	lex->col = col;
	
	return tk;
}

int iswhitespace(char c) {
	return (c == ' ' || c == '\t' || c == '\n');
}

struct token *next_token(struct lexer *lex) {
	
	struct token *tk;

	while (iswhitespace(*lex->pos))
		advance(lex);
	
	if (*lex->pos == '\0')
		return NULL;
	
	switch(*lex->pos) {
	case '"':
	case '\'':
		tk = lex_string(lex);
		break;
	case ',':
		tk = match(lex, TT_COMMA, ",");
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		tk = lex_number(lex);
		break;
	case '[':
		tk = match(lex, TT_AOPEN, "[");
		break;
	case ']':
		tk = match(lex, TT_ACLOSE, "]");
		break;
	case '{':
		tk = match(lex, TT_OOPEN, "{");
		break;
	case '}':
		tk = match(lex, TT_OCLOSE, "}");
		break;
	case 't': 
		tk = match(lex, TT_TRUE, "true");
		break;
	case 'f':
		tk = match(lex, TT_FALSE, "false");
		break;
	case ':':
		tk = match(lex, TT_COLON, ":");
		break;
	case 'n':
		tk = match(lex, TT_NULL, "null");
		break;
	default:
		tk = NULL;
	} 
	return tk;
}
