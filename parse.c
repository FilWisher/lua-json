#include <stdlib.h>

#include <lua.h>
#include <lauxlib.h>

#include "lex.h"
#include "parse.h"

struct parser {
	struct token **pos;
};

struct parser *parser_make(const char *str);
static int parse_string(lua_State *L, struct parser *p);
static int parse_number(lua_State *L, struct parser *p);
static int parse_boolean(lua_State *L, struct parser *p);
static int parse_array(lua_State *L, struct parser *p);
static int parse_object(lua_State *L, struct parser *p);
static int parse_token(lua_State *L, struct parser *p);

static char err[1024];

static int parse_token(lua_State *L, struct parser *p) {

	struct token *tk = *p->pos;
	// TODO: set more sensibly
	
	switch (tk->typ) {
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
		p->pos++;
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
		sprintf(err, "couldn't parse token %s", tt_string(tk->typ));
		lua_pushstring(L, err);
		lua_error(L);
		//lua_pushnil(L);
	}
	return 1;
}

static int parse_boolean(lua_State *L, struct parser *p) {
	struct token *tk = *p->pos;
	
	lua_pushboolean(L, tk->typ == TT_TRUE ? 1 : 0);
	
	free(tk);
	p->pos++;
	
	return 1;
}

static int parse_number(lua_State *L, struct parser *p) {
	struct token *tk = *p->pos;
	int num;
	
	// TODO: hack while number parsing not implemented
	char *tmp = malloc(sizeof(char) * (tk->len + 1));
	sprintf(tmp, "%.*s", tk->len, tk->st);	
	num = atoi(tmp);
	
	free(tmp);
	free(tk);
	p->pos++;
	
	lua_pushnumber(L, num);
	
	return 1;	
}

static int parse_string(lua_State *L, struct parser *p) {
	struct token *tk = *p->pos;
	
	lua_pushlstring(L, tk->st, tk->len);
	
	free(tk);
	p->pos++;
	
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
		lua_pushstring(L, "could not make parser");
		lua_error(L);
	}
	
	if (!parse_token(L, p)) {
		lua_pushstring(L, "could not parse json");
		lua_error(L);
	}
	
	return 1;
}

int expect(struct parser *p, tt typ) {
	struct token *cur = *p->pos;
	if (cur->typ == typ) {
		p->pos++;
		return 0;
	}
	return -1;
}

static int parse_array(lua_State *L, struct parser *p) {
	struct token *tk;
	int index = 1;
	
	lua_newtable(L);
	if (expect(p, TT_AOPEN) != 0)
		return 0;
	for (tk = *p->pos; tk->typ != TT_ACLOSE && tk->typ != TT_END; tk = *p->pos) {
	
		lua_pushnumber(L, index++);
		if (!parse_token(L, p))
			return 0;
		lua_settable(L, -3);
		
		if (expect(p, TT_ACLOSE) != 0 && expect(p, TT_COMMA) != 0)
			return 0;
	}
	
	return 1;
}

static int parse_object(lua_State *L, struct parser *p) {
	
	lua_newtable(L);
	if (expect(p, TT_OOPEN) != 0) {
		lua_pushstring(L, "expected opening {");
		lua_error(L);
	}
	struct token *tk;
	for (tk = *p->pos; tk->typ != TT_OCLOSE && tk->typ != TT_END; tk = *p->pos) {
	
		if (!parse_string(L, p)) {
			lua_pushstring(L, "could not parse string for key");
			lua_error(L);
		}
			
		if (expect(p, TT_COLON) != 0) {
			sprintf(err, "expected colon but got token of type %s with value %.*s", 
				tt_string(tk->typ), tk->len, tk->st);
			lua_pushstring(L, err);
			lua_error(L);
		}
		
		if (!parse_token(L, p)) {
			lua_pushstring(L, "could not parse token for value");
			lua_error(L);
		}
		
		// TODO: MUST have comma if more to come
		expect(p, TT_COMMA);
		
		lua_settable(L, -3);
	}
	return 1;
}

struct parser *parser_make(const char *str) {
	struct lexer *l = lex_make();
	if (lex(l, str) != 0) {
		lex_delete(l);
		return NULL;
	}
	struct parser *p = (struct parser *)malloc(sizeof(struct parser));
	p->pos = l->tks;
	return p;
}
