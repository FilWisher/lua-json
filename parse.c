/*
	Parser and operations for parsing objects, arrays, booleans,
	strings, and numbers.
	
	Parse functions return 1 on success. 
	On failure they update the parser's 'err' field with an error
	message for printing.
*/

#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "lex.h"
#include "parse.h"

#define MAX_ERR (1024)

struct parser {
	struct lexer *lex;
	struct token *lookahead;
	// keep track of last token for deletion
	struct token *old;
	char err[MAX_ERR];
};

struct parser *parser_make(const char *str);
static int parse_string(lua_State *L, struct parser *p);
static int parse_number(lua_State *L, struct parser *p);
static int parse_boolean(lua_State *L, struct parser *p);
static int parse_array(lua_State *L, struct parser *p);
static int parse_object(lua_State *L, struct parser *p);
static int parse_token(lua_State *L, struct parser *p);

// TODO: replace use of *p->pos with next(p)
struct parser *parser_make(const char *str) {
	struct parser *p = (struct parser *)malloc(sizeof(struct parser));
	if (p == NULL) {
		return NULL;
	}
	p->lex = lex_make(str);
	if (p->lex == NULL) {
		return NULL;
	}
	p->lookahead = next_token(p->lex);
	p->old = NULL;
	return p;
}

struct token *next(struct parser *p) {
	struct token *tk;

	// TODO: remove TT_END checks, NULL indicates end of string 
	if (p->lookahead == NULL || p->lookahead->typ == TT_END)
		return p->lookahead;

	if (p->old != NULL)
		free(p->old);

	tk = p->lookahead;
	p->old = tk;
	
	p->lookahead = next_token(p->lex);
	
	return tk;
}

struct token *peek(struct parser *p) {
	return p->lookahead;
}

static int parse_token(lua_State *L, struct parser *p) {

	
	switch (peek(p)->typ) {
	case TT_STR:
		if (parse_string(L, p) != 1)
			return 0;
		break;
	case TT_NUM:
		if (parse_number(L, p) != 1)
			return 0;
		break;
	case TT_TRUE:
	case TT_FALSE:
		if (parse_boolean(L, p) != 1)
			return 0;
		break;
	case TT_NULL:
		lua_pushnil(L);
		next(p);
		break;
		
	case TT_AOPEN:
		if (parse_array(L, p) != 1)
			return 0;
		break;
	case TT_OOPEN:
		if (parse_object(L, p) != 1)
			return 0;
		break;
		
	default:
		sprintf(p->err, "couldn't parse token %s", tt_strings[peek(p)->typ]);
		lua_pushstring(L, p->err);
		lua_error(L);
	}
	return 1;
}

// NOTE: assumes lookahead->typ is TT_TRUE or TT_FALSE
static int parse_boolean(lua_State *L, struct parser *p) {
	
	struct token *tk = next(p);
	lua_pushboolean(L, tk->typ == TT_TRUE ? 1 : 0);
	return 1;
}

static int parse_number(lua_State *L, struct parser *p) {
	
	struct token *tk = next(p);
	int num;
	
	// TODO: hack while number parsing not implemented
	char *tmp = malloc(sizeof(char) * (tk->len + 1));
	sprintf(tmp, "%.*s", tk->len, tk->str);	
	num = atoi(tmp);
	
	free(tmp);
	
	lua_pushnumber(L, num);
	
	return 1;	
}

static int parse_string(lua_State *L, struct parser *p) {
	
	struct token *tk = next(p);
	
	lua_pushlstring(L, tk->str, tk->len);
	
	return 1;
}

int l_parse(lua_State *L) {
	
	const char *jsonstring;
	struct parser *p;
	
	int n = lua_gettop(L);
	if (n != 1 || !lua_isstring(L, 1)) {
		lua_pushstring(L, "parse requires a single string argument");
		lua_error(L);
	}
	
	jsonstring = lua_tostring(L, 1);	
	
	if ((p = parser_make(jsonstring)) == NULL) {
		sprintf(p->err, "could not make parser");
		lua_pushstring(L, p->err);
		lua_error(L);
	}
	
	if (!parse_token(L, p)) {
		free(p->lex);
		free(p);
		lua_pushstring(L, p->err);
		lua_error(L);
	}

	free(p->lex);
	if (p->old != NULL)
		free(p->old);
	free(p);
	
	return 1;
}

int expect(struct parser *p, tt typ) {
	struct token *tk = peek(p);
	if (tk == NULL) {
		sprintf(p->err, "expected %s but reached end of json",
			tt_strings[typ]);	
		return 0;
	}
	if (tk->typ != typ) {
		sprintf(p->err, "expected %s but got %s", 
			tt_strings[typ], tt_strings[tk->typ]);
		return 0;
	}
	next(p);
	return 1;
}

static int parse_array(lua_State *L, struct parser *p) {
	
	struct token *tk = peek(p);
	int index = 1;
	
	lua_newtable(L);
	
	if (!expect(p, TT_AOPEN))
		return 0;
	
	for (; tk != NULL && tk->typ != TT_ACLOSE && tk->typ != TT_END; tk = peek(p)) {
		
		lua_pushnumber(L, index++);
		
		if (!parse_token(L, p))
			return 0;
		
		lua_settable(L, -3);
	
		if (!expect(p, TT_COMMA))
			break;
	}
	
	return expect(p, TT_ACLOSE);
}

static int parse_object(lua_State *L, struct parser *p) {
	
	struct token *tk = peek(p);
	lua_newtable(L);
	
	if (!expect(p, TT_OOPEN))
		return 0;

	for (; tk != NULL && tk->typ != TT_OCLOSE && tk->typ != TT_END; tk = peek(p)) {
	
		if (!parse_string(L, p))
			return 0;
		
		if (!expect(p, TT_COLON))
			return 0;
		
		if (!parse_token(L, p))
			return 0;
		
		lua_settable(L, -3);
	
		if (!expect(p, TT_COMMA))
			break;
	}
	
	return expect(p, TT_OCLOSE);
}
