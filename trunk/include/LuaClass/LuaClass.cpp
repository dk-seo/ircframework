#include "LuaClass.h"
#include "../LuaBindings.h"

lua_State *CLua::Engine;
string CLua::ScriptFile;

void CLua::report_errors(lua_State *L, int status)
{
	if ( status!=0 ) 
	{
		std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1); // remove error message
	}
}

void CLua::Init()
{
	Engine = luaL_newstate();

	luaL_openlibs(Engine);

	int s = luaL_loadfile(Engine, ScriptFile.c_str());
    if(s == 0) 
	{
		s = lua_pcall(Engine, 0, LUA_MULTRET, 0);
    }
	else
	{
		cout << "Not Loaded Script" << endl;
		cout << lua_tostring(Engine, -1);
	}
	cout << "Initiated Lua instance" << endl;

	BindLuaFunctions();
}

void CLua::setScriptFile(string FileName)
{
	ScriptFile = "lua/" + FileName;
}

string CLua::getScriptFile()
{
	return ScriptFile;
}

void CLua::callEvent(string EventName, string Message, string Channel, string Sender)
{
	luaL_dostring(Engine, (EventName+"('"+Message+"','"+Channel+"','"+Sender+"')").c_str());
}

void CLua::Close()
{
	lua_close(Engine);
}

