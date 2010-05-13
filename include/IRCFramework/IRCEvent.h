#ifndef _IRCEVENT_H_
#define _IRCEVENT_H_

#include <string>
#include "Channel.h"

using namespace std;

namespace IRCFramework
{
	struct IRCEvent
	{
		string Message;
		User *Sender;
		string Channel;
		IRCEvent(string aMessage, User *aSender, string aChannel) :
			Message(aMessage),
			Sender(aSender),
			Channel(aChannel)
		{
		};
		IRCEvent(string aMessage) :
			Message(aMessage)
		{
		};
	};
}


#endif