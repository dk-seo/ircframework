#include "IRC.h"
#include <iostream>
#include <cstring>

namespace IRCFramework {

	WSADATA *IRC::m_wsa;
	SOCKET IRC::mh_Socket;
	string IRC::m_User;
	string IRC::m_Nick;
	string IRC::m_HostName;
	int IRC::m_Port;
	bool IRC::isConnected = false;
	char IRC::m_Msg[1024];

	map<string, Channel> IRC::Channels;
	map<string, User> IRC::UserList;

	Event::EventServer<IRCEvent> IRC::onMessageRecieved(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onNoticeRecieved(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onUserPartChannel(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onUserJoinChannel(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onPing(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onConnect(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onDisconnect(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onKick(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onJoin(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onFailedJoin(&IRC::handleRecievedInput);
	Event::EventServer<IRCEvent> IRC::onTopicChange(&IRC::handleRecievedInput);
	
	void IRC::Init()
	{
		///////////////////////
		// Handle WSAStartup //
		///////////////////////
		if(WSAStartup( MAKEWORD(m_MajorVer, m_MinorVer), ((LPWSADATA)&m_wsa) ) != 0) 
		{
			cout << "[ERROR] Failed WSAStartup.\r\n" << endl;
			onDisconnect.Fire(IRCEvent("Unable to initialise WinSock"));
			// Handle errors here.
		}
		///////////////////
		// Handle Socket //
		///////////////////
		mh_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(mh_Socket == INVALID_SOCKET)
		{
			cout << "[ERROR] Failed to create socket. Exiting" << endl;
			
			// Handle errors here.
		}
	}

	void IRC::Tick()
	{
		int ReturnValue = recv(mh_Socket, m_Msg, sizeof(m_Msg), 0);
		if(ReturnValue > 0)
		{
			vector<string> Messages;
			Tokenise(string(m_Msg), Messages, "\r\n");
			bool Handled = false;
			for(unsigned int i=0; i<Messages.size(); i++)
			{
				Handled = handleRecievedInput(Messages[i]);
				if(!Handled)
				{
					cout << "[RECV] " << Messages[i] << endl;
				}
				Handled = false;
			}
		}
		else if(ReturnValue == 0)
		{
			/**
			*	If recv returns 0 it means the socket is d/c'ed
			**/
			onDisconnect.Fire(IRCEvent(m_HostName));
			return;
		}
		/**
		*	Reset the m_Msg array so that there aren't the remains of last ticks 
		*	message in it for next time around
		**/
		for(int i=0; i<1024; i++)
		{
			m_Msg[i] = '\0';
		}
	}
	
	void IRC::Connect(string HostName, int Port) 
	{
		m_HostName = HostName;
		m_Port = Port;
		hostent *host;
		sockaddr_in sockAddr;
		
		host = gethostbyname(m_HostName.c_str());
		
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(m_Port);
		sockAddr.sin_addr.S_un.S_addr = ((in_addr *)(host->h_addr))->s_addr;
		
		/////////////////////
		// Connection Loop //
		/////////////////////
		while(connect(mh_Socket, (sockaddr *) &sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
			printf("[ERR] Connection failed, retrying...\r\n");
		}
	
		
		
		IRC::sendRawToServer("USER "+m_Nick+" 0 0 :"+m_Nick+"\r\n");
		IRC::sendRawToServer("NICK "+m_Nick+"\r\n");

		printf("[SUCCESS] Attempting Registration.\r\n");
	}

	void IRC::setUserInformation(string Nick, string User) 
	{
		m_Nick = Nick;
		m_User = User;
	}

	void IRC::Join(string Channel) 
	{
		std::transform(Channel.begin(), Channel.end(), Channel.begin(), tolower);
		/**
		*	This checks that the channel is stored in our current channel array.
		**/
		if(Channels[Channel].ChannelName == "")
		{
			IRCFramework::Channel newChannel;
			newChannel.ChannelName = Channel;
			sendRawToServer("JOIN "+Channel+"\r\n");
			Channels[Channel] = newChannel;
			return;
		}
		cout << "Already in channel" << endl;
	}
	void IRC::Part(string Channel) 
	{
		ConvertToLowerCase(Channel);
		if(Channels[Channel].ChannelName != "")
		{
			Channels[Channel].ChannelName = "";
			UserList.empty();
			sendRawToServer("PART "+Channel+"\r\n");
		}
	}
	void IRC::setTopic(string Channel) { }
	void IRC::getTopic(string Channel) { }
	void IRC::Query(string User, string Message) { }
	void IRC::sendToChannel(string Channel, string Message)
	{
		sendRawToServer("PRIVMSG "+Channel+" :"+Message+"\r\n");
	}
	
	void IRC::sendNotice(string Recipient, string Message)
	{
		sendRawToServer("NOTICE " + Recipient + " :" + Message + "\r\n");
	}

	bool IRC::isInChannel(string ChannelName) 
	{
		ConvertToLowerCase(ChannelName);
		if(Channels[ChannelName].ChannelName != "")
		{
			return true;
		}
		return false;
	}

	void IRC::Quit(string Reason)
	{
		sendRawToServer("QUIT :"+Reason+"\r\n");
		onDisconnect.Fire(IRCEvent("QUIT"));
		WSACleanup();
	}

	/*=====| Private Functions |=====*/
	void IRC::replyToPing(string Message) 
	{
		sendRawToServer("PONG"+Message+"\r\n");
		onPing.Fire(IRCEvent(Message));
	}

	void IRC::sendRawToServer(string Message) 
	{	
		send(mh_Socket, Message.c_str(), Message.length(), 0);
	}

	bool IRC::handleRecievedInput(string Input)
	{
		if(Input.find("PING") == 0)
		{
			replyToPing(Input.substr(4));
			return true;
		}
		else if(isIRCNumeric(Input))
		{
			return true;
		}
		else if(isPrivMsg(Input))
		{
			return true;
		}
		else if(isNotice(Input))
		{
			return true;
		}
		return false;
	}
	bool IRC::isIRCNumeric(string Input)
	{
		//:Adam-PC 001 IndiXIRCBot :Welcome
		vector<string> Message;
		Tokenise(Input, Message, " ");
		if(Message.size() > 3)
		{
			const char Comparison = ':';
			if(Message[0][0] == Comparison)
			{
				if(Message[2] == m_Nick)
				{
					return handleIRCNumeric(Message);
				}
			}
		}
		return false;
	}
	bool IRC::handleIRCNumeric(vector<string> Message)
	{
		if(Message[1] == "001")
		{
			//IRC Client Accepted
			onConnect.Fire(IRCEvent(m_HostName));
			return true;
		}
		else if(Message[1] == "353")
		{
			if(Message.size() < 6)
			{
				cout << "Recieved malformed irc raw 353." << endl;
				return false;
			}
			string Channel = Message[4];
			ConvertToLowerCase(Channel);
			for(unsigned int i = 5; i<Message.size(); i++)
			{
				User tempUser;
				if(i == 5)
				{	
					tempUser.Name = Message[i].substr(1);
				}
				else
				{
					tempUser.Name = Message[i];
				}
				if(tempUser.Name[0] == '@')
				{
					tempUser.Name = tempUser.Name.substr(1);
					tempUser.isOpped = true;
				}
				else if(tempUser.Name[0] == '+')
				{
					tempUser.Name = tempUser.Name.substr(1);
					tempUser.isVoiced = true;
				}
				if(UserList[tempUser.Name].Name == "")
				{
					UserList[tempUser.Name] = tempUser;
				}
			}
		}
		//[RECV] akenet.org 353 IndixBot = #indix :IndixBot ShadowStarshine Zoner7 @Dotmister LordSputnik DamegedSpy JensKiil_ TomCatFort|afk @Q

		return false;
	}
	bool IRC::isPrivMsg(string Input)
	{
		vector<string> vecMessage;
		Tokenise(Input, vecMessage, " ");
		if(vecMessage.size() > 3)
		{
			if(vecMessage[0][0] == ':')
			{
				if(vecMessage[1] == "PRIVMSG")
				{
					if(vecMessage[3][1] == '☺')
					{
						//CTCP Message
						return handleCTCP(vecMessage);
					}
					string strSender, Message, Channel;
					strSender = vecMessage[0].substr(1, vecMessage[0].find("!") - 1);
					for(unsigned int i = 3; i<vecMessage.size(); i++)
					{
						Message +=vecMessage[i] + " ";
					}
					Message = Message.substr(1, Message.length() - 2);
					Channel = vecMessage[2];
					ConvertToLowerCase(Channel);

					User *Sender;
					Sender = NULL;
					for(map<string, User>::iterator it = UserList.begin(); it != UserList.end(); ++it)
					{
						if(it->second.Name == strSender)
						{
							Sender = &(it->second);
							cout << "Matched User" << endl;
							break;
						}
					}
					if(Sender == NULL)
					{
						sendToChannel(Channel, "User is NULL, message ignored");
					}
					else
					{
						onMessageRecieved.Fire(IRCEvent(Message, Sender, Channel));
					}
					return false;
				}
			}
		}
		return false;
	}
	bool IRC::isNotice(string Input)
	{
		vector<string> vecMessage;
		Tokenise(Input, vecMessage, " ");
		if(vecMessage.size() > 3)
		{
			if(vecMessage[0][0] == ':')
			{
				if(vecMessage[1] == "NOTICE")
				{
					string strSender, Message, Channel;
					strSender = vecMessage[0].substr(1, vecMessage[0].find("!") - 1);
					for(unsigned int i = 3; i<vecMessage.size(); i++)
					{
						Message +=vecMessage[i] + " ";
					}
					Message = Message.substr(1, Message.length() - 2);
					Channel = vecMessage[2];
					ConvertToLowerCase(Channel);

					User *Sender;
					Sender = NULL;
					for(map<string, User>::iterator it = UserList.begin(); it != UserList.end(); ++it)
					{
						if(it->second.Name == strSender)
						{
							Sender = &(it->second);
							break;
						}
					}
					if(Sender == NULL)
					{
						sendToChannel(Channel, "User is NULL, notice ignored");
					}
					else
					{
						onMessageRecieved.Fire(IRCEvent(Message, Sender, Channel));
					}
					return false;
				}
			}
		}
		return false;
	}

	bool IRC::handleCTCP(vector<string> Message)
	{
		Message[3] = Message[3].substr(2, Message[3].length()-3);

		if(Message[3] == "VERSION")
		{
			sendToChannel("#indix", "Recieved Version CTCP");
			return false;
		}
		return false;
	}

	void ConvertToLowerCase(std::string& str)
	{
		std::transform(str.begin(),	str.end(), str.begin(), tolower);
	}
}