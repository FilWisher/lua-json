#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "parse.h"

static const struct luaL_Reg json_funcs[] = {
	{ "parse", l_parse },
	{ NULL, 	NULL }
};

int luaopen_json(lua_State *L) {
	luaL_newlib(L, json_funcs);
	return 1;
}
