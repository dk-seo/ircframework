#ifndef _IRC_H_
#define _IRC_H_
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <winsock2.h>
#include <windows.h>

#include "Event/Event.h"

#include "IRCEvent.h"
#include "Channel.h"

using namespace std;

namespace IRCFramework
{
	class IRC
	{
	public:
		/**
		*	Initiate the IRC static class
		**/
		static void Init();

		/**
		*	Checks for messages from server and sends out events etc
		**/
		static void Tick();

		/**
		*	Connects to an IRC Server
		**/
		static void Connect(string HostName, int Port);

		/**
		*	Sets the information which the framework needs to connect to the server
		**/
		static void setUserInformation(string Nick, string User);

		/**
		*	Joins an IRC Channel
		**/
		static void Join(string Channel);

		/**
		*	Leaves an IRC Channel
		**/
		static void Part(string Channel);

		/**
		*	Sets the topic of an IRC Channel
		**/
		static void setTopic(string channel);

		/**
		*	Gets the topic of an IRC Channel
		**/
		static void getTopic(string Channel);

		/**
		*	Sends a PM to a specific user
		**/
		static void Query(string User, string Message);

		/**
		*	Sends a message to a channel (must be in that channel)
		**/
		static void sendToChannel(string Channel, string Message);

		/**
		*	Send notice to recipient
		**/
		static void sendNotice(string Recipient, string Message);

		/**
		*	Checks if the bot is currently in the specified channel
		**/
		static bool isInChannel(string ChannelName);

		/**
		*	Quits the current server
		**/
		static void Quit(string Reason);

	private:
		static void replyToPing(string Message);
		static void sendRawToServer(string Message);
		
		static bool isIRCNumeric(string Input);
		static bool isPrivMsg(string Input);
		static bool isNotice(string Input);

		static bool handleIRCNumeric(vector<string> Message);
		static bool handleCTCP(vector<string> Message);
		static bool handleRecievedInput(string Input);

	public:
		static Event::EventServer<IRCEvent> onMessageRecieved;
		static Event::EventServer<IRCEvent> onNoticeRecieved;
		static Event::EventServer<IRCEvent> onUserPartChannel;
		static Event::EventServer<IRCEvent> onUserJoinChannel;
		static Event::EventServer<IRCEvent> onPing;
		static Event::EventServer<IRCEvent> onConnect;
		static Event::EventServer<IRCEvent> onDisconnect;
		static Event::EventServer<IRCEvent> onKick;
		static Event::EventServer<IRCEvent> onJoin;
		static Event::EventServer<IRCEvent> onFailedJoin;
		static Event::EventServer<IRCEvent> onTopicChange;	

		static map<string, Channel> Channels;
		static map<string, User> UserList;

	private:
		static string m_User;
		static string m_Nick;
		static string m_HostName;
		static int m_Port;
		static bool isConnected;
		
		
		static const int m_MajorVer = 2;
		static const int m_MinorVer = 0;

		static WSADATA *m_wsa;
		static SOCKET mh_Socket;
		static char m_Msg[1024];
	};
	inline void Tokenise(string str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (string::npos != pos || string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	}
	void ConvertToLowerCase(std::string& str);
}

#endif