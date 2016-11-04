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

struct token *token_make(tt typ, char *st, int len) {
	struct token *tk = (struct token *)malloc(sizeof(struct token));
	tk->typ = typ;
	tk->str = st;
	tk->len = len;
	return tk;
}

// create new lexer; MUST: call lexer_delete() after
struct lexer *lex_make(const char *str) {
	struct lexer *l = (struct lexer *)malloc(sizeof(struct lexer));
	l->pos = l->str = str;
	l->len = 0;
	return l;
}

struct token *emit(struct lexer *lex, tt typ) {
	struct token *tk = token_make(typ, lex->pos, lex->len);
	lex->pos += lex->len;
	lex->len = 0;
	return tk;
}

struct token *lex_string(struct lexer *lex) {
	// TODO: check don't go past end of string
	// TODO: check quote is not preceded by backslash
	struct token *tk;
	
	if (*lex->pos != '"' && *lex->pos != '\'')
		return NULL;
	
	char delim = *(lex->pos++);
	
	while (*(lex->pos+lex->len) != '\0' && *(lex->pos+lex->len) != delim) {
		lex->len += 1;
	}
	
	tk = emit(lex, TT_STR);
	
	// skip past last double quote
	lex->pos += 1; 
	return tk;
}

struct token *lex_number(struct lexer *lex) {
	// TODO: support floating point 
	
	while (isdigit(*(lex->pos+lex->len)))
		lex->len += 1;
	 
	// TODO: check conditions for end of number more carefully
	if (isalpha(*(lex->pos+lex->len)))
		return NULL;
		
	return emit(lex, TT_NUM);
}

// return -1 on fail, 0 on success
struct token *match(struct lexer *lex, tt typ, char *str) {
	int i, len = strlen(str);
	for (i = 0; i < len && *(lex->pos+i) != '\0'; i++) {
		if (*(lex->pos+i) != *str) {
			return NULL;
		}
		str++;
	}
	lex->len = i;
	return emit(lex, typ);
}

int iswhitespace(char c) {
	return (c == ' ' || c == '\t');
}

struct token *next_token(struct lexer *lex) {
	
	struct token *tk;

	while (iswhitespace(*lex->pos))
		lex->pos++;
	
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
