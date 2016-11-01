#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lex.h"

#define must(p) \
if ((status = p) != 0) \
	return status \

int lex_array(struct lexer *lex);
int lex_token(struct lexer *lex);

char *tt_string(tt typ) {
	switch (typ) {
	case TT_STR: return "string";
	case TT_NUM: return "number";
	case TT_COMMA: return "comma";
	case TT_AOPEN: return "a-open";
	case TT_ACLOSE: return "a-close";
	
	case TT_COLON: return "colon";
	
	case TT_OOPEN: return "o-open";
	case TT_OCLOSE: return "o-close";
	
	case TT_TRUE: return "true";
	case TT_FALSE: return "false";
	case TT_NULL: return "null";
	default: return "";
	}
}

struct token *token_make(tt typ, char *st, int len) {
	struct token *tk = (struct token *)malloc(sizeof(struct token));
	tk->typ = typ;
	tk->st = st;
	tk->len = len;
	return tk;
}

// create new lexer; MUST: call lexer_delete() after
struct lexer *lex_make() {
	struct lexer *l = (struct lexer *)malloc(sizeof(struct lexer));
	l->i = 0;
	l->cap = 8;
	l->len = 0;
	l->tks = (struct token **)malloc(sizeof(struct token *) * l->cap);
	return l;
}

void lex_delete(struct lexer *lex) { 
	for (int i = 0; i < lex->i; i++) {
		free(*(lex->tks+i));
	}
	free(lex->tks);
	free(lex);
}

struct token *emit(struct lexer *lex, tt typ) {
	struct token *tk = token_make(typ, lex->pos, lex->len);
	lex->pos += lex->len;
	lex->len = 0;
	return tk;
}

void lex_push(struct lexer *lex, struct token *tk) {
	if (lex->i >= lex->cap) {
		lex->cap *= 2;
		lex->tks = realloc(lex->tks, sizeof(struct token *) * lex->cap);
	}
	*(lex->tks+lex->i) = tk;
	lex->i += 1;
}

int lex_string(struct lexer *lex) {
	// TODO: check don't go past end of string
	// TODO: check quote is not preceded by backslash
	
	if (*lex->pos != '"' && *lex->pos != '\'')
		return -1;
	
	char delim = *(lex->pos++);
	
	while (*(lex->pos+lex->len) != '\0' && *(lex->pos+lex->len) != delim) {
		lex->len += 1;
	}
	lex_push(lex, emit(lex, TT_STR));
	
	// skip past last double quote
	lex->pos += 1; 
	return 0;
}

int lex_number(struct lexer *lex) {
	// TODO: support floating point 
	
	while (isdigit(*(lex->pos+lex->len)))
		lex->len += 1;
	 
	// TODO: check conditions for end of number more carefully
	if (isalpha(*(lex->pos+lex->len)))
		return -1;
		
	lex_push(lex, emit(lex, TT_NUM));
	
	return 0;
}

// return -1 on fail, 0 on success
int match(struct lexer *lex, tt typ, char *str) {
	int i, len = strlen(str);
	for (i = 0; i < len && *(lex->pos+i) != '\0'; i++) {
		if (*(lex->pos+i) != *str) {
			return -1;
		}
		str++;
	}
	lex->len = i;
	lex_push(lex, emit(lex, typ));
	return 0;
}

void skipws(struct lexer *lex) {
	while (*lex->pos == ' ' || *lex->pos == '\t' || *lex->pos == '\n')
		lex->pos++;
}

int lex_object(struct lexer *lex) {
	int status;
	must(match(lex, TT_OOPEN, "{"));	
	skipws(lex);
	while ((status = match(lex, TT_OCLOSE, "}")) != 0) {
		
		must(lex_string(lex));
		skipws(lex);
		
		must(match(lex, TT_COLON, ":"));
		skipws(lex);
		
		must(lex_token(lex));
		skipws(lex);
		
		// it doesn't matter if we can't match "," 
		match(lex, TT_COMMA, ",");
		skipws(lex);
	}
	
	return 0;	
}

int lex_token(struct lexer *lex) {
	int status;
	switch(*lex->pos) {
	case ' ':
	case '\t':
		lex->pos += 1;
		break;
	case '"':
	case '\'':
		must(lex_string(lex));
		break;
	case ',':
		must(match(lex, TT_COMMA, ","));
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		must(lex_number(lex));
		break;
	case '[':
		must(lex_array(lex));
		break;
	case '{':
		must(lex_object(lex));
		break;
	case 't': 
		must(match(lex, TT_TRUE, "true"));
		break;
	case 'f':
		must(match(lex, TT_FALSE, "false"));
		break;
	case 'n':
		must(match(lex, TT_NULL, "null"));
		break;
	default:
		return -1;
	} 
	return 0;
}

int lex_array(struct lexer *lex) {
	int status;
	
	must(match(lex, TT_AOPEN, "["));
	skipws(lex);
	
	while ((status = match(lex, TT_ACLOSE, "]")) != 0) {
	
		must(lex_token(lex));
		skipws(lex);
		
		match(lex, TT_COMMA, ",");
		skipws(lex);
	}
	return 0;
}

int lex(struct lexer *lex, char *str) {
	int status;
	for (lex->pos = str; *lex->pos != '\0'; )
		must(lex_token(lex));
		
	lex_push(lex, token_make(TT_END, "", 0));
	return 0;
}
