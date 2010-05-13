#include <iostream>
#include <string.h>
#include "IRCFramework/IRC.h"
#include "LuaClass/LuaClass.h"
#include <time.h>

bool Run = true;
string PassCode;

void onDisconnect(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "Disconnected from Server" << 
		endl;
	Run = false;
}

void onPing(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "We just got PINGed" << endl;
}

void onConnect(void *aSender, IRCFramework::IRCEvent aEvent)
{
	CLua::callEvent("onConnect", aEvent.Message);
}

void onMessage(void *aSender, IRCFramework::IRCEvent aEvent)
{
	vector<string> Commands;
	IRCFramework::Tokenise(aEvent.Message, Commands, " ");
	if(Commands[0] == "!admin")
	{
		if(Commands.size() > 1)
		{
			if(Commands[1] == "authinfo")
			{
				PassCode = "";
				for(unsigned int i=0; i<12; i++)
				{
					PassCode += (rand() % 26) + 65;
				}
				cout << "Authorise with: !auth login " << PassCode << endl;
			}
			else if(Commands[1] == "login")
			{
				if(Commands.size() == 3)
				{
					if(Commands[2] == PassCode)
					{
						if(PassCode != "")
						{
							IRCFramework::IRC::sendToChannel(aEvent.Channel, "Sucessfully authorised: "+aEvent.Sender->Name);
							PassCode = "";
							aEvent.Sender->isAdmin = true;
						}
						else
						{
							IRCFramework::IRC::sendToChannel(aEvent.Channel, "Passcode outdated");
						}
					}
					else
					{
						IRCFramework::IRC::sendToChannel(aEvent.Channel, "Incorrect Password");
					}
				}
				else
				{
					IRCFramework::IRC::sendToChannel(aEvent.Channel, "Not the right number of parameters");
				}
			}
			else if(Commands[1] == "lua")
			{
				if(aEvent.Sender->isAdmin)
				{
					if(Commands.size() > 2)
					{
						if(Commands[2] == "reload")
						{
							CLua::Close();
							CLua::Init();
							IRCFramework::IRC::sendToChannel(aEvent.Channel, "Reloaded!");
						}
					}
				}
				else
				{
					IRCFramework::IRC::sendToChannel(aEvent.Channel, "Not Enough permissions");
				}
			}

		}
	}
	CLua::callEvent("onMessage", aEvent.Message, aEvent.Channel, aEvent.Sender->Name);
}

void onNotice(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "onNotice" << endl;
	CLua::callEvent("onNotice", aEvent.Message, aEvent.Sender->Name);
}

void onUserPart(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "onUserPart" << endl;
}

void onUserJoin(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "onUserJoin" << endl;
}

void onKick(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "We just got PINGed" << endl;
}

void onJoin(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "onJoin" << endl;
}

void onFailedJoin(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "onFailedJoin" << endl;
}

void onTopicChange(void *aSender, IRCFramework::IRCEvent aEvent)
{
	cout << "onTopicChange" << endl;
}


int main()
{
	srand ( time(NULL) );
	CLua::setScriptFile("IRC.lua");
	CLua::Init();
	
	IRCFramework::IRC::Init();
	
	IRCFramework::IRC::onDisconnect += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onDisconnect);
	IRCFramework::IRC::onPing += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onPing);
	IRCFramework::IRC::onConnect += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onConnect);
	IRCFramework::IRC::onMessageRecieved += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onMessage);

	IRCFramework::IRC::onNoticeRecieved += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onNotice);
	IRCFramework::IRC::onUserPartChannel += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onUserPart);
	IRCFramework::IRC::onUserJoinChannel += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onUserJoin);
	IRCFramework::IRC::onKick += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onKick);
	IRCFramework::IRC::onJoin += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onJoin);
	IRCFramework::IRC::onFailedJoin += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onFailedJoin);
	IRCFramework::IRC::onTopicChange += new Event::StaticSubscription<IRCFramework::IRCEvent> (&onTopicChange);

	CLua::callEvent("onLoad");
	while(Run)
	{
		IRCFramework::IRC::Tick();
	}
	system("pause");
	return 0;
}