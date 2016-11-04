#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdlib.h>

#include <stdio.h>
#include <assert.h>

#include "parse.h"

void test_array() {
	
	lua_State *L = luaL_newstate();
	lua_pushstring(L, "[ 'hello', 1234, true, null, false ]");
	assert(l_parse(L));

	assert(lua_type(L, -1) == LUA_TTABLE);	
	
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		printf("%s: %s\n",
			lua_typename(L, lua_type(L, -2)),
			lua_typename(L, lua_type(L, -1)));
		lua_pop(L, 1);
	}
	lua_close(L);
}

void test_object() {
	
	lua_State *L = luaL_newstate();
	lua_pushstring(L, "{ 'hello': 1234, 'cool': true }");
	assert(l_parse(L));
	
	assert(lua_type(L, -1) == LUA_TTABLE);
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		printf("%s: %s\n",
			lua_typename(L, lua_type(L, -2)),
			lua_typename(L, lua_type(L, -1)));
		lua_pop(L, 1);
	}
	lua_close(L);
}

void test_nested() {
	lua_State *L = luaL_newstate();
	lua_pushstring(L, "[ ['hello'], { 'wowza': 1234, 'bur': true}, [[[null]], [false] ]]");
	assert(l_parse(L));
	
	assert(lua_type(L, -1) == LUA_TTABLE);
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		printf("%s: %s\n",
			lua_typename(L, lua_type(L, -2)),
			lua_typename(L, lua_type(L, -1)));
		lua_pop(L, 1);
	}
	lua_close(L);
}

int main() {
	
	test_array();
	test_object();
	test_nested();
	
}
