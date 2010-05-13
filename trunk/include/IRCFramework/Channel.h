#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace IRCFramework
{
	// User stored on a per-channel basis
	class User
	{
	public:
		string Name;
		string Mode;
		bool isVoiced;
		bool isOpped;
		bool isAdmin;
		User()
		{
			isVoiced = isOpped = isAdmin = false;
			Name = "";
			Mode = "";
		}
	};


	class Channel
	{
	public:
		string ChannelName;
		string Topic;
		vector<User> UserList;
	};	
}



#endif