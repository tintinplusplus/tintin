Chat Additions, Bug Fixes and Additions for 1.7
-----------------------------------------------
Sean Butler - Xin - sbutler@deveast.com


1/13/98         Version 1.7
		FIXED:
		* All the #un commands no longer crash tintin.
		* "#end"ing tintin with a session open does not crash it.
                * Basically I fixed lots of linked list bugs.
                * SIGPIPE is now ignored.
		* Took out code that breaks lines if the user doesn't have
		  a terminal that does it for him.  This just caused horrible
		  problems with color muds.
		* Made actions parse more quickly.
		* Rewrote all socket IO to use send/recv also made it more
		  efficient.
                CHANGED:
		* split mode now senses the size of your terminal and prints
                  a line as wide as the terminal.  Works for autodetected
                  screen size changes.

		ADDED:
		* Mudmaster compatible chat: #call, #chat, #chatall, #chatpriv,
		  #chattransfer, #cinfo, #filecancel, #ping, #request, 
                  #sendfile, #unchat, #filestat
                  See help file for details on their use and function.
                  chat.c was added for the above new functions.
		* Color codes.  Using the syntax 
		  &<letter>
                  You can now include color codes in your actions and aliases.
                  Valid letters are:
		  capital = bright, lowercase = normal
		  R = Red, G = Green, Y = Yellow, C = Cyan, M = magenta
                  B = Blue, W = white, n = normal
		(SB)

chat.c

This module by: Sean Butler, sbutler@deveast.com - January 10, 1998

An extension to Tintin that allows users to connect their clients together in
a chat session.  Multiple sessions are supported.  The chat protocol used is
compatible with MudMaster chat and was designed by the maker of MudMaster.

All code is however of my creation and therefore direct all complaints, 
comments or praise to me.

The following new commands have been added to tintin for chat purposes:

#call               #chat            #chatall         #emote
#emoteall           #unchat          #sendfile        #filecancel
#request            #chattransfer    #ping

The scheme used to implement this is a linked list of structures
containing data about each connection.  This structure was placed
in tintin.h  


This is an alpha release and is not yet complete.  There are many
holes in the parsing of commands.  The most glaring is the failure
to adhere to the standard tintin syntax.  In this release you may
not use {} around the arguments to a chat command.  ie.

#chat {soandso} {Hi there}

Next release this problem will be fixed.

The only code that needed to be changed to add this module were these:

parse.c  -- to add the user commands
rl.c     -- added a call to chat_process_connections() and added
            the chat sockets to the call to select() in bait.
main.c   -- added the call to init_chat()
misc.c   -- added cleanup call in function end_command.

Kindly accept my appologies for those of you who are forced to use
80 column displays, since I regularly go over 80 columns when writing
this code.
