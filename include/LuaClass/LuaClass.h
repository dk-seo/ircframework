#ifndef _LUACLASS_H_
#define _LUACLASS_H_

#include "lua/lua.hpp"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class CLua
{
	public:
		static void Init();
		static void Close();

		static void report_errors(lua_State *L, int status);
		
		static lua_State *Engine;
		
		static void	setScriptFile(string FileName);
		static string getScriptFile();
		static void callEvent(string EventName, string Message = "", string Channel = "", string Sender = "");
		
		static int handleError(lua_State *L);
		
	private:
		static string ScriptFile;
};

#endif