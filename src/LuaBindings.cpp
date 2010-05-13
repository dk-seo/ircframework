#include "LuaClass/LuaClass.h"
#include <sstream>

#include "IRCFramework/IRC.h"

int my_function(lua_State *L)
{
	int argc = lua_gettop(L);

	std::cerr << "-- my_function() called with " << argc
	<< " arguments:" << std::endl;

	for ( int n=1; n<=argc; ++n ) 
	{
		std::cerr << "-- argument " << n << ": "
				  << lua_tostring(L, n) << std::endl;
	}

	lua_pushnumber(L, 123); // return value
	return 1; // number of return values
}

int ircQuit(lua_State *L)
{
	int argc = lua_gettop(L);
	
	if(argc == 1)
	{
		string Reason = lua_tostring(L, 1);
		IRCFramework::IRC::Quit(Reason);
		return 0;
	}
	IRCFramework::IRC::Quit("Uses IRCFramework by IndiX.");
	return 0;
}

int ircJoin(lua_State *L)
{
	int argc = lua_gettop(L);
	
	if(argc == 1)
	{
		string Channel = lua_tostring(L, 1);
		IRCFramework::IRC::Join(Channel);
		return 0;
	}
	cout << "ircJoin not given enough parameters" << endl;
	return 0;
}

int ircPart(lua_State *L)
{
	int argc = lua_gettop(L);
	
	if(argc == 1)
	{
		string Channel = lua_tostring(L, 1);
		IRCFramework::IRC::Part(Channel);
		return 0;
	}
	cout << "ircPart not given enough parameters" << endl;
	return 0;
}

int ircSendToChannel(lua_State *L)
{
	int argc = lua_gettop(L);
	
	if(argc == 2)
	{
		string Channel = lua_tostring(L, 1);
		string Message = lua_tostring(L, 2);
		IRCFramework::IRC::sendToChannel(Channel, Message);
		return 0;
	}
	cout << "ircSendToChannel not given enough parameters" << endl;
	return 0;
}

int ircConnect(lua_State *L)
{
	int argc = lua_gettop(L);
	
	if(argc == 2)
	{
		string HostName = lua_tostring(L, 1);
		int Port = lua_tointeger(L, 2);
		IRCFramework::IRC::Connect(HostName, Port);
		return 0;
	}
	cout << "ircConnect not given enough parameters" << endl;
	return 0;
}

int ircSetUserInformation(lua_State *L)
{
	int argc = lua_gettop(L);
	
	if(argc == 2)
	{
		string Nick = lua_tostring(L, 1);
		string User = lua_tostring(L, 2);
		IRCFramework::IRC::setUserInformation(Nick, User);
		return 0;
	}
	cout << "ircSendUserInformation not given enough parameters" << endl;
	return 0;
}


void BindLuaFunctions()
{
	lua_register(CLua::Engine, "ircSetUserInformation", ircSetUserInformation);
	lua_register(CLua::Engine, "ircConnect", ircConnect);

	lua_register(CLua::Engine, "ircQuit", ircQuit);
	lua_register(CLua::Engine, "ircJoin", ircJoin);
	lua_register(CLua::Engine, "ircPart", ircPart);

	lua_register(CLua::Engine, "ircSendToChannel", ircSendToChannel);
}
