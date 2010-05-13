io.write("Loaded Lua\n")

function onMessage(Message, Channel, Sender)

	if(Sender == "Dotmister" and Message == "!Quit") then
		io.write("Quitting...")
		ircQuit("Quitting...")
		return
	end
	if(Message == "Hello DotBot") then
		ircSendToChannel(Channel, "Hello "..Sender.."!")
	end
	io.write("[MSG From " .. Sender .. " In " .. Channel .. "] " .. Message)
end

function onLoad()
	io.write("onLoad\n")
	ircSetUserInformation("DotBot", "IRC C++/Lua FRAMEWORK")
	ircConnect("irc.gtanet.com", 6667)
end

function onConnect(HostName)
	io.write("onConnect\n")
	ircJoin("#ivmp.bot")
end