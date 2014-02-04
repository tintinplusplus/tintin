
10/30/01	Changing all toggle-able commands to the following format:
		#<command> -> toggle command on/off.
		#<command> on -> turns the command on.
		#<command> off -> turns the command off.

		So far, only #brokentelnet, #fixedmath, #appendlog has been changed.

		Changed: misc.c, include/misc.h, parse.c -- dsc

7/12/01		Released tintin v1.86 -- dsc

7/12/01		Updated the copyright notice to reflect the GPL license.
		Changed: all files -- dsc

7/8/01		Added COPYING file and offical place tintin under GPL and
		updated the FAQ file to reflect the change.
		Changed: FAQ
		Added: COPYING -- dsc
		
6/27/01		Added the ability to either append to a log file or to
		overwrite the log file (default) as requested.  You can 
		change the behavior of the #log command via the #appendlog 
		command.
		Changed: tintin.h, main.c, parse.c, misc.c, files.c,
			 include/main.h, include/misc.h -- dsc

6/21/01		Release tintin v1.85.  -- dsc

6/8/01		Added a #show_pretick command.  This will toggle on/off
		the #10 SECONDS TO TICK message, but it will still show
		the #TICK message.
		Changed: main.c, rltick.c, tintin.h, include/main.h, 
			include/misc.h, misc.c, parse.c -- dsc

5/13/01		Removed the include/Makefile and include/Makefile.in.
		Deleted: include/Makefile, include/Makefile.in
		Changed: configure.in -- dsc

5/13/01		Added a TINTIN_HISTORY enviroment variable to set the
		location of the .tt_history file.
		Changed: main.c, rl.c -- dsc

5/13/01		Removed the -Iinclude from the Makefile.in and cleaned up
		referances to protos.
		Changed: Makefile.in, all .c files -- dsc

5/13/01		Added a check for an enviroment variable called TINTIN_CONF
		so that tintin will check for the tt.conf file at that
		location before checking for it in your home directory.
		Changed: chat.c -- dsc

5/13/01		Renamed the send_file command to tt_send_file to avoid
		a name conflict under AIX.  Thanks to Yoon-Chan Jhi for
		pointing it out.
		Changed: chat.c, parse.c, include/chat.h -- dsc

4/3/01		Fixed the newline string that tintin sends.  It really
		should be \r\n.  Thanks to Emil Isberg for pointing
		that out.  Left in the broken_telnet thing but I'm
		hoping it isn't needed anymore as tintin was sending
		out the wrong newline sequence.
		Changed: net.c -- dsc

3/28/01		Updated the FAQ.
		Changed: FAQ -- dsc

3/17/01		Renamed all of the protos directory to include and renamed
		all the proto files to .h.  Updated all the .c files to
		reflect the change and updated the Makefile.in.
		Changed: all files -- dsc

3/17/01		Changed the BUFFERSIZE to 2048.  And fixed the hard coded
		values in the protos to use the BUFFERSIZE value.
		Changed: tintin.h, protos/main.proto, protos/function.proto 
			-- dsc

2/25/01		Commented out the SIGWINCH line in main.c that was
		spamming some people (appears to be only sun users).
		Need to investigate this when I some time.
		Changed: main.c -- dsc
	
12/29/00	Fixed some warnings messages by gcc with -Wall enabled.
		Changed: help.c, files.c, ivars.c, main.c, misc.c,
			 path.c, rl.c, ticks.c  -- dsc

12/29/00	Updated the readline.sh slightly to reflect the use
		of --includedir and --libdir instead of having
		to modify the Makefile.
		Changed: readline.sh -- dsc

12/28/00	Fixed a few warnings that appear on sun machines. And
		replaced usage of bzero with memset and assorted clean ups.
		Changed: scrsize.c, chat.c, net.c -- dsc

12/26/00	Released tintin v1.84.  It's been a while since my
		last release.  Pushing this out the door so that
		people can make use of some of the fixes.  There
		are some rough spots that I need to tidy up, but
		there shouldn't be any problems.  -- dsc

12/26/00	Updated the FAQ file.
		Changed: FAQ -- dsc

12/26/00	Added a toggle to enable and disable the saving of
		history.  The command is #savehistory.  This will
		need to be intergrated into the configuration file.

		Changed: main.c, misc.c, rl.c, tintin.h, 
			protos/misc.proto, protos/rl.proto -- dsc

12/26/00	Fixed it so that tt.conf is looked for in the user's
		home directory instead of the current directory.  The
		tt.conf file and the .tintinrc files need to be merged
		at some point so that there is only one configuration
		file.

		Changed: chat.c -- dsc

12/17/00	Removed some debug statements in the #ifstrequal command.
		Changed: ivars.c -- dsc

12/13/00	Added in saving of history.  Tintin will write out a
		.tt_history file to your home directory and read that
		back in the next time you start up tintin.  Keep in mind
		that it will write everything you enter as a command,
		including your character's password.  You can do a 
		#clearhistory to delete all the history as well.  I'll
		probably make the saving of history optional as some
		may be parnoid and some may not have alot of space in their
		home directory to save a history of commands.  Feedback
		on the usefulness of this feature would be welcome.
		Changed: main.c, tintin.h, rl.c -- dsc

11/07/00	Hmm, tintin regressed in respect to the broken_telnet fix.
		Seems that the fix for muds that wanted only \n instead of
		the standard \n\r was lost.  I've reintergrated the fix, but
		I guess it really doesn't affect most users since it's been
		missing since 1.81.
		Changed: net.c -- dsc

11/07/00	Updated the install-sh script to reflect the change in the
		name of the chat.conf file to tt.conf.  tt.conf will be
		a generic tintin configuration file.  It's purpose will
		be to store custom configurations of tintin.  
		Changed: install-sh
		Renamed: chat.conf -- dsc

11/07/00	Slight update to the INSTALL file to make it more clear on
		compiling tintin with non-standard locations of readline.
		Changed: INSTALL -- dsc

11/07/00	Put in a #include for stat.h that is needed for sun machines
		that wasn't needed on my linux box.
		Changed: chat.c -- dsc

10/09/00	Relocated the chat.conf file to the src/support directory
		and updated the install-sh script to reflect that.
		Changed: install-sh
		Moved: chat.conf -- dsc

10/09/00	Fixed the problem with + - in the math function that
		was reported by Kuang-che Wu.  Let me know if the #math
		function does anything strange.  The original (buggy) math
		is still available via the #fixedmath toggle, but by default
		it will use correct math. 
		Changed: ivars.c -- dsc

10/03/00	Updated the install-sh script so that it will install the
		chat.conf file in the same location as your tintin helpfile.
		Changed: install-sh -- dsc

10/03/00	Updated the FAQ and added some comments to the chat.conf
		file.
		Changed: FAQ, chat.conf -- dsc

10/02/00	Added a new configure option, --enable-big5.  This allows
		users to select Big5 encoding without needing to manually
		edit the Makefile.
		Changed: configure.in, Makefile.in, INSTALL -- dsc

10/02/00	Small update to the FAQ file.
		Changed: FAQ -- dsc

09/27/00	Cleaned up chat.c some.  Removed some un-needed header
		files.  Mac OS X can compile tintin now.
		Changed: chat.c -- dsc

09/27/00	Added misc patches by Kuang-che Wu.
		Changed: misc.c, rl.c, net.c -- dsc

09/27/00	Added support for Big5 encoding.  This has not been
		tested by myself nor do I have any method of testing it.
		This was sent to me by Kuang-che Wu.  Current, you need
		to manually edit the Makefile to have a -DBIG5 to the DEFINES
		line.  If you have language specific patches, I will be 
		willing to add them as well, but you must provide me with 
		the code to be be added.
		Changed: action.c, parse.c, variables.c, functions.c -- dsc

09/16/00	Fixed the typo with the checking for ncurses and curses.
		Changed: configure.in -- dsc

09/10/00	Replaced DST_NONE with 0 as not all machines have that
		defined.
		Changed: chat.c -- dsc

09/07/00	Released tintin v1.83 -- dsc

09/07/00	Made it so that tintin will check first for termcap, then
		check for curse and then for ncurse.  Hopefully that will
		resolve the problem some people having with the tgetent.
		Changed: configure.in -- dsc

09/07/00	tintin will check for a TINTIN_HELP environment variable.
		This will allow you to get tintin to check a shared tintin
		help file.  All you need to do is to set the TINTIN_HELP 
		variable to the directory your help file is located.
		Changed: help.c -- dsc

09/06/00	Found a error with the install-sh script.  The script would
		FAIL.  Fixed the problem with it.
		Changed: install-sh -- dsc

09/03/00	Corrected a bug with the #random function.  asd@scene-hu.com
		provided the one line fix.
		Changed: misc.c -- dsc

08/26/00	Corrected a bug with the install.sh script.  There was
		a typo in the installing gzip version of the help files.
		Decided that the dl directory isn't needed in the tintin++
		directory.  Users should create a dl or change the download
		location in the chat.conf file.
		Changed: install.sh
		Deleted: dl directory -- dsc

08/25/00	Fixed the problem of HP-UX not linking because of the
		order of the libraries.  Put -lreadline before everything
		else.  Also put in -lhistory for the history stuff.  It
		seems that my compiler isn't as picky as others and allowed
		me to link without the -lhistory.
		Changed: Makefile.in -- dsc

08/16/00	Released tintin v1.82 -- dsc

08/16/00	Movied the MODIFICATIONS and MOD.ycjhi files into the docs
		directory.
		Moved: MODIFICATIONS, MOD.ycjhi -- dsc

08/16/00	Added a FAQ file.  Hopefully it will answer some of the most
		commonly asked questions.
		Added: FAQ -- dsc

08/12/00	Added an ENABLE field to the chat.conf file.  It is enabled
		by default, but you can turn off the chat feature by setting
		the value of ENABLE to 0.  There still isn't all that much
		error checking with the chat features, which will be 
		corrected in the future.
		Changed: main.c, rl.c, chat.c -- dsc

07/25/00	Cleaned up all the warnings in chat.c.
		Changed: chat.c -- dsc

07/21/00	Updated the .tt_help.txt file with the new chat commands.
		Also added in a helpversion entry, this just lists the date
		the helpfile was last modified to track which version of
		the helpfile that is installed.  Make sure you do a make 
		install to update your help files.
		Changed: support/.tt.help.txt.Z -- dsc

07/20/00	With the addition of chat, the encryption code that I hacked
		together is really not needed.  I've removed the code for it
		as chat looks to be a nicer command than my ugly encrypt code.
		If people really want it, let me know and I'll put it back
		in, but I don't think it's really needed.
		Changed: misc.c, protos/misc.proto, parse.c -- dsc

07/20/00	chat has been added.  This is very similiar to using an instant
		messager program or using irc.  The chat feature is compatiable
		with mudmaster (or it should be).  The code is by Sean Butler.
		I tried to contact him about the use of his code, but could
		not contact him.  And I am assuming he would allow for the use
		of his code unless notified otherwise.  Added him to the
		credit file.  The code needs alot of clean up and modifications
		still, but the basic functionality is all there.  Currently,
		it will try to figure out your IP address, unless you set
		it in your chat.conf file.  To start a chat, you would do
		a #call <ip address>.  Once you have connected, you can do a
		#chat <person's name> <message>.  You can also do a #chatall
		to send a message to everybody your connected to.  Bug me to
		write up some docs on it, but the commands should be pretty
		straight forward (take a peek in the chat.c for a list of 
		commands).  You can also transfer files (I personally
		haven't tested it).
		Added: chat.c -- dsc
		Created: protos/chat.proto -- dsc
		Changed: CREDITS, main.c, rl.c, parse.c -- dsc

07/16/00	Added a --libdir to configure.  This is needed if you don't
		have your libraries (readline) in standard library locates.
		This change is kind of a hack since I need to hardcore
		the -lreadline in the Makefile.in, but since you can't really
		use tintin without readline, having it hardcoded isn't a
		problem.  Need to play with autoconf more to see if there
		is a better solution.
		Changed: configure.in, Makefile.in, INSTALL -- dsc

07/06/00	Added a #clearhistory command.  This will clear out your
		history of commands entered.
		Changed: parse.c -- dsc

07/06/00	Added a BUG and TODO file.  This is to let everybody know
		what I think are bugs and what I plan on working.  If you have
		any input on bugs or things you would like to see added to 
		tintin, feel free to let me know.
		Added: BUGS, TODO -- dsc

07/05/00	Worked some on the bindkey command.  It doesn't do anything
		really, but I spent some time playing with readline and now
		have a better idea of how to get it to work.
		Changed: bindkey.c, protos/bindkey.proto -- dsc

06/23/00	Fixed a bug with the #read and #newread.  They both
		would fail if your command file contain any blank lines.
		Changed: files.c  -- dsc

06/23/00	I've decided to move the #read command now to be
		#newread and move the #oldread back as the default #read
		command.  If you wish to use the multi-line code, use the
		#newread command.  After it's been tested a while, I will
		make it the default #read command.
		Changed: parse.c, files.c, protos/files.proto -- dsc

06/23/00	Added in the fixes for functions submitted by Kaung-che Wu.
		Functions still need work though.
		Changed: variables.c, protos/variables.proto, function.c -- dsc

06/22/00	Cleaned up the install-sh script a bit.
		Changed: install-sh -- dsc

06/22/00	Played more and now you can select the default directory
		tt++ is put.  It will default to the src directory.  Like
		the --includedir, you change the bindir by doing a
		./configure --bindir=<where you want it>.

		Example:
		You want to put tt++ in ~/bin

		./configure --bindir=~/bin
		make
		make install
		Changed: Makefile.in, configure.in, INSTALL -- dsc

06/22/00	I've played with the autoconf some and made some improvements
		to it.  If you put readline in a non-standard directory
		(ie not in /usr/local/include), you can specify that with
		./configure --includedir=<location>.  
		For example, if you put the readline library in say
		tintin/src/readline. Then in the tintin/src directory,
		you can do a ./configure --includedir=. (current directory).

		Another example, if you put readline in /tmp/readline,
		you can do a ./configure --includedir=/tmp

		As you need to put in the parent directory of the readline
		directory.
		Changed: Makefile.in, configure.in, INSTALL -- dsc

06/19/00	I've added some missing code for functions that
		was submitted by Kuang-Che Wu.
		Changed: llist.c, files.c -- dsc

06/18/00	I've enabled the fixed_math by default now.  This probably
		will not affect most tintin users, but if you use / or *
		you may be affected and may want to either change your
		script or to use the #fixedmath to disable it.
		Changed: tintin.h -- dsc

06/18/00	Put in a fix to a crash bug with verbatim when you don't
		have a session.  Thanks to Kuang-che Wu for the bug
		report and fix.
		Changed: parse.c -- dsc

06/18/00	Made a fix to the #message command.  It was not listing out
		the different message toggle-ables and made some slight
		improvements to it.
		Changed: misc.c -- dsc

06/18/00	Did further testing and I found a work around.  I'll look
		into this problem further when I get a chance, but the
		working will allow people to use the #crypt and #decrypt
		functions. Added an example of how to make use of it
		in the docs/example1.script file.  Somebody probably can
		write a something better, but it does work.  If anybody
		wants to write better examples (with explainations) I
		would be happy to include them.  Also if you use it, you
		may want to disable the variable messages (#message variable 
		off) to reduce the spam (it is on by default). -- dsc
		
06/09/00	Modified the #crypt and #decrypt functions.  They currently
		used a fixed key, but will change that soon.  I had to change
		the way it was set up because I was trying to do it the lazy
		way and use the ASCII character set table.  The old function
		mostly worked, but for some keys it would give you funky 
		results.

		The work around noted in the previous entry does not work.
		I will need to fix the bug for these commands to be useful.
		The problem is in the parser.
		Changes: misc.c -- dsc

06/03/00	Added a #crypt and a #decrypt function for "encrypting" text.
		I remember seeing a tintin script that did this ages ago
		on the newsgroup.  I decided to implement the code into
		tintin.  Although shortly this code will not be so useful
		with the incoming chat feature, but this feature might be
		fun to use to annoy imms with *polishes his halo* :)
		It's extremely simple, but it should be more than enough to
		ensure nobody that you don't want to read the text will.
		The syntax is: 
		#crypt <text to encrypt> <variable to store result>
		#decrypt <text to decrypt> <variable to store result>
		Currently, the key used to encrypt the data is a fixed key,
		but will add in a feature to change the key.

		This works fine except I seem to hit a bug when testing
		the feature. The bug is that tintin will substitutue
		everything in an alias with multiple commands
		#alias {ereply} {#crypt {$$0} {bleh}; reply $bleh}
		however
		#alias {ereply} 
		   {#unvar {$bleh} #crypt {$$0} {bleh}; reply $bleh}
		will work fine.  

		I will probably fix this bug at a future date, but it looks
		to be a bit mess/large change to fix in a critical function.
		And it would be far to easy to introduce new bugs there.
		Changed: misc.c, protos/misc.proto, parse.c -- dsc
	
05/29/00	Added the option of stripping the binary in the install-sh
		script.  Striping will produce a smaller execuatable, but it
		makes it harder to debug.  But most people will not need to
		debug tintin and would probably want to strip tt++.
		Changed: install-sh -- dsc

05/29/00	*cough* hacked a*cough* Fix to the issue #2 (see 5/5/00 entry) 
		with the new read_command().  The read_command function will
		ignore any line that starts with a "#nop" and will not parse it
		so it will not care about unbalanced braces.  I probably will
		look for a better fix/rewrite the code in the future.  This
		should resolve all the problems with the read_command.
		Changed: files.c -- dsc

05/27/00	Put in Yoon-Chan Jhi's fix to the syserr().  This should solve
		solve the problem some people have with compiling tintin.
		Changed: utils.c -- dsc

05/23/00	Made a few some modifications to the configure.in script to
		hopefully make it more informative when running ./configure.
		Changed: configure.in -- dsc

05/05/00	Fixed the problem with the read_command() not taking
		commands with no arguments (ie #fixedmath).
		Changed: files.c -- dsc

05/05/00	Ran dos2unix on the example2.script.  This solves part
		of the problem, with the new read_command(), but still
		have the following issues.  1) it will not allow
		unbalanced braces in #nop.  2) it will not take commands 
		with no arguments (ie #fixedmath).
		Changed: docs/example2.script -- dsc

05/05/00	Modified the translate_telnet_protocol() to be a void
		function.  There really isn't a need for it to return
		anything.
		Changed: net.c, protos/net.proto -- dsc

04/30/00	Released tintin v1.81.  I want to push it out now or else
		it would probably be another 3+ weeks and I've promised to
		put this out within a week or two (and it's been like 2 1/2 
		weeks now). -- dsc

04/30/00	Tracked down a problem I was having connecting to a mud
		server.  tintin was just kicking me out at the same point.
		This was due to a change in the read_mud_buffer to use
		the new translate_telnet_protocol().  I've put in a fix for
		it, but I'll probably redo the fix later.
		Changed: net.c -- dsc

04/30/00	Made a bug fix to the read_command function.  If your
		command file only contained a "#ses {l} {localhost 4000}"
		It wasn't connecting when you started up tintin or when
		you #read your command file.  
		Changed: files.c -- dsc

04/29/00	I've included the sample tintin command file that Greg
		Milford sent me as it's a nice demostration of how to use
		tintin.  This may be useful to people in learning how to
		make the best use of tintin.
		Created: docs/example2.script (the script by Greg)
		Renamed: docs/example.script to docs/example1.script -- dsc

04/29/00	I've intergrated a change to the read_command function by
		Greg Milford.  The change allows for multi-line command
		files.  See the docs/example.script for how it will look.
		The only downside to the change is that you must use braces
		ie #alias a {say blah} will not work, you need to do 
		#alias {a} {say blah}.  The read_command will try to recover
		from commands that are not written correct by ignore it and
		everything after the error and then return to tintin.
		So if you have 10 aliases and tintin says that it read in 5
		aliases, then tintin didn't like your 6th aliases you wrote
		and you should go check it.  I've kept the old #read command
		as #oldread if you really want to use that, but it will
		probably be removed at a future date.
		Changed: files.c, protos/files.proto, CREDITS, 
			 docs/example.script -- dsc

04/29/00	I've updated the tintin help file so that it now includes
		all the tintin commands.  Some of the tintin help files are
		a *cough* bit sparse, but they are now listed :)  I still
		need to update the manual and some of the entries in the 
		tintin help file needs to be expanding (just a tinnie bit ;))
		Changed: support/.tt_help.Z -- dsc

04/12/00	Put in Yoon-Chan Jhi's change to #ifmatch.  The only real
		change is that it no longer has an else keyword.  This
		matchs the syntax of other tintin commands.
		Changed: ivars.c -- dsc

04/09/00	Cleaned up some warning.
		Changed: walk.c, session.c -- dsc

04/09/00	I've added an example.script in the tintin/docs directory.
		This includes some basic examples (which I will expand on
		when I have more time) and it includes Yoon-Chan Jhi's
		auto reconnect script.
		Created: docs/example.script -- dsc

04/09/00	I've put in all of Yoon-Chan Jhi's changes which includes
		a zMud-like slow walk function (I don't know, I never used
		zMud before), an auto reconnect feature.  I'm testing them 
		to make sure I've gotten everything that he submitted to me 
		working correctly  There were alot of changes and hopefully 
		some changes didn't slip by me.  I've made some minor
		modifications to his code, but they were very limited
		at this point.  (Don't ask me what they were, cuz I don't
		remember :))
		Created: walk.c, protos/walk.proto
		Changed: net.c, session.c, tintin.h, parse.c -- dsc

04/08/00	Put in Yoon-Chan Jhi's change to connect_mud so that the
		error messages would be more helpful than just couldn't
		connect.  Didn't know about the strerror() till now.
		Learned something :)
		Changes: net.c -- dsc

04/08/00 	It's amazing what you can learn if you read :)  According,
		to man (3) bcopy, bcopy has been deprecated.  So that explains
		why some compilers give you those problems when compiling
		with the -O2 flag.  This was chitchat's suggestion on how
		to solve the problem, but I wasn't comfort just commenting
		out the use of a function to silence a compiler warning.  But
		on further research saw that bcopy was deprecated.  In light 
		of that, I've added back in -O2 flag and I've removed the 
		use of bcopy. I've also updated the configure script to
		stop checking for bcopy since it isn't used anymore.
		Changes: Makefile.in, tintin.h, configure -- dsc

04/08/00	Added chitchat and Yoon-Chan Jhi to the credits file for
		their contributions to tintin.
		Changes: CREDITS -- dsc

04/05/00	I've added in ycjhi's change to read_buffer_mud, which
		fixes the problem with tintin sometimes eating a
		character after getting an GA.   As a note the GA 
		sequence will send a \n in tintin to solve an 
		occassion problem with actions not being triggered.
		The code, except for minor modifications, were done by 
		Yoon-Chan Jhi.
		Changes: net.c, protos/net.c -- dsc

04/05/00	Changed the tickset command so that the code and
		the manual both agree with each other.  According
		to the manual tickset will turn on the timer if it's
		not on, but the code doesn't do that.  Now it will.
		This is minor, but the code and manual should agree
		with each other, or what's the point of the manual if
		it's wrong?
		Changed: ticks.c -- dsc

04/03/00	Changed the ticksize command so that you will need to
		enter a number between 1 and 1000.  You previous could
		crash tintin because it would try to divide by 0.  There
		isn't any reason why you would want to set your ticksize
		to zero unless you wanted to crash tintin :P  So 1 to 1000
		should be a suitable range.  Let me know if there is a reason
		why you would want to have anything outside that range.
		Changed: ticks.c -- dsc

03/08/00	Added a bindkey.proto and a bindkey.c file for the
		future bindkey command.  This will be for binding keys to
		a command.  ie #bindkey {f1} {<command>}.  This doesn't do
		anything yet.
		Created: bindkey.proto and bindkey.c -- dsc

02/23/00	Fixed a bug with the parsing of input.  It would display the
		wrong value, but the result is correct.
		If you did the following:
		#var {a} {1}
		#math {a} {$a+1};#showme $a
		This would show you 1 and not 2.  The value stored in $a is
		correct, but it would display 1.  It will now show the correct
		value 2.  Thanks to aladrin@elven.comm.au for the bug report.
		Changed: parse.c, protos/parse.proto -- dsc
		
02/23/00	Removed the -ansi flag from the Makefile and removed the
		declaration of popen and pclose and made asorted cleanup 
		of things that were hacks/bad ideas.
		Changed: Makefile.in, protos/help.proto, misc.c -- dsc

02/19/00	Added in the brokentelnet value to the #info command.

02/12/00	Released tintin v1.80 -- dsc

02/12/00	Verified that the fix for CR/LF worked on the test mud listed
		on the webboard.

02/09/00	I think I corrected the problem with CR/LF with certain muds.
		Please let me know if I did/or didn't.  Do a #brokentelnet 
		to enable it to test.
		Changed: net.c, misc.c, main.c, parse.c, protos/misc.proto, 
		protos/main.proto -- dsc

12/26/99	Updated the README and the doc/sgi files
		Changed: README, doc/sgi -- dsc

12/26/99	Increased the buffer for #textin for people that need to
		read in a large text file.  Didn't increase all buffers 
		since that may be a waste of memory, but for #textin you
		may need more than 1024 so I've increased it to 4096.  
		Problem was brought to my attention by chitchat's ytin.
		Changed: text.c -- dsc

12/26/99	Fixed the math command completely (I think, let me know
		if i'm wrong).  You *MUST* have #fixedmath 1 set for
		you to use the new math, or else it will continue to use
		the old broken math.  Eventually, this will be the default
		but for now, you will have to enable it.  Fix came
		from chitchat's ytin.  With the fixed math, you will
		get 4 from #math {a} {6/3*2}.  With the old math, you
		will get 1 from #math {a} {6/3*2}.
		Changed: ivars.c, protos/ivars.proto -- dsc

12/21/99	Ahh, gotta love short-circuit evalation :)  Fixed a
		crash bug.  If you did a #message 1, it will crash.
		It's fixed now.  Who says it doesn't pay to be lazy? :)
		Changed: misc.c -- dsc

12/19/99	Added Fixed Math to the #info output.
		Changed: misc.c -- dsc

12/18/99	Added a new command to enable and disable the math
		code (will also be used for priority on math symbols when
		I add in that code).  THE DEFAULT IS TO USE THE BROKEN
		CODE.  To enable the fixed math code do a #fixedmath.
		Eventually, I will make it default to fixedmath on, but
		for now, I'll keep it at off.
		Changed: main.c, parse.c, misc.c, protos/main.proto,
			 protos/misc.proto -- dsc

12/18/99	Looked at the #math {x} {$x+1} problem.  I've decided
		to just return an error instead of initializing the
		undefined variable to 0.  No answer is better than a wrong
		answer in my opinion.  If tintin automatically creates a 
		variable, typos would create new variables and people would 
		wonder why it kept failing.  This way, there is no doubt, 
		that something is wrong with the math expression.  This 
		should save people time trying to debug their scripts 
		looking for why their math call always returns the same 
		(wrong) value.  
		
		#math {x} {$x+1} -> old math will create and set x to zero
				 -> new math will error and not create a 
					     variable
				
		Changed: ivars.c, protos/ivars.proto -- dsc

12/17/99	Added a sanity check for values entred for #sleep, #timetry
		#tries.
		Changed: misc.c -- dsc

12/17/99	Added in the code for #sleep.  The syntax is #sleep <time
		in seconds>.  I thought there was a command like this but
		I guess I was wrong.  Idea came from chitchat's version
		of tintin, but the implementation is different.
		Changed: misc.c parse.c protos/misc.proto --dsc

12/12/99	Added #tries and #timetry for changing the default values
		of the number of tries tintin will make and the time between
		tries.  The defualt is still 10 tries at 10 seconds apart.
		Also fixed some wrong messages with the reconnect code.
		Changed: parse.c, main.c, session.c, misc.c,
			protos/main.proto, protos/misc.proto -- dsc

12/12/99	Removed the TODO and ChangeLog files from the src directory.
		Appended ChangeLog file to CHANGES.
		Deleted: TODO and ChangeLog -- dsc

12/12/99	Slight update to INSTALL doc.
		Change: INSTALL -- dsc

12/9/99		Added externs for popen and pclose.  These should not be
		needed, but for some reason the compiler doesn't think that
		these two functions have been defined yet.  It's odd since
		these are defined in system header files.  I've put in a
		hack that removes the warning messages, but I shouldn't
		have had those warning messages in the first place since
		the header file was included.  Will look into that some
		more later. This also makes MIPS Pro C compiler (IRIX) 
		happier when trying to compile tintin.
		Changed: protos/help.proto -- dsc

12/3/99		Made a change so that if you have two different string
		header files (string.h and strings.h) it will default to
		string.h and not try use both.  
		Changed: variables.c -- dsc

11/28/99	Check to see if ctype.h is found in the configure script.
		Changed: configure.in and all files that use ctype.h -- dsc

11/28/99	Released tintin v1.79 -- dsc

11/27/99	Made some more changes to the install-sh script.  The
		tintin archive will only contain a .Z help file but if you
		have gzip or bzip2 in your path, it will convert the helpfile
		to that compressed format and install that in your home
		directory.
		Changed: install-sh -- dsc

11/21/99	Made some changes to the make install so that it will
		select the help file to install based on if you have bzip2,
		gzip, or uncompress.
		Changed: install-sh -- dsc

11/21/99	Added support for bzip2 help files.  It will check for help
		files in the following order: uncompressed, .bz2, .gz, .Z
		Bzip2 has better compression than .gz but is slightly slower.
		So if you want the help files and are short on space, bz2 is
		the way to go.
		Changed: tintin.h help.c -- dsc

11/20/99	Minor change to the help command so that it reads press
		return and not any key when the help file will not fit
		all of the help section in the screen.
		Changed: help.c -- dsc

11/20/99	Restructuring some of the code and still is a work in 
		progress.
		Changed: most if not all files -- dsc

11/19/99	Fixed some warning messages.
		Changed: function.c, variables, rl.c -- dsc

11/17/99	Added a reconnect feature the tintin.  If tintin can not
		connect to a mud, it will continue to retry 10 times every
		10 seconds.  The retry amount and time between retries
		can be adjusted.  The idea came from tintin v1.5pl9,
		which came from Snarf.  However, it was coded differently 
		from v1.5pl9.  
		Changed: session.c -- dsc

11/15/99	Released tintin v1.78 -- dsc

11/15/99	Fixed a bonehead (and I mean a really bonehead) crash bug.
		No idea how long it's been there.  But #unvar will stop 
		crashing tintin now.
		Changed: variables.c -- dsc

11/14/99	Made a minor changes to the Makefile.in so that it is more
		friendly to non gnu make.
		Changed: Makefile.in -- dsc

11/08/99	Released tintin v1.77 -- dsc

10/28/99	Fixed a warning about main not returning an int.
		Changed: main.c protos/main.proto -- dsc

10/28/99        Fixed the void value not ignored as it ought to be 
 		message.  There seems to be a problem with gcc and -O
		that is generating this problem.  Decided to remove the
		optimizing for now.  Will research into this more when
		I get a chance.
		Changed: Makefile.in -- dsc

10/24/99	Fixed the bug with having to put quotation marks around
		everything you typed in.  Fixed the bug by reverting to
		strncpy, strncat instead of using memcpy.
		Changed: parse.c -- dsc

10/23/99	Fixed a problem with an extra new line being added to the
		prompt if you don't use split mode.
		Changed: rl.c  -- dsc
