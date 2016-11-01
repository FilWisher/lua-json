#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "parse.h"

static int l_test(lua_State *L) {
	//const char *str = lua_tostring(L, 2);
	lua_pushstring(L, "HEHAHAH");
	return 1;
}

static const struct luaL_Reg json_funcs[] = {
	{ "testit", l_test },
	{ "parse", l_parse },
	{ NULL, 	NULL }
};

int luaopen_json(lua_State *L) {
	luaL_newlib(L, json_funcs);
	return 1;
}
