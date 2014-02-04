
03/24/99 	Well.. I kinda had that split line bug fixed.  Thanks to Magnus
		Oberg, I was able to hopefully fix it completely.  We'll just
		have to see how this works.
		CHANGED: rl.c  --rse

03/17/99	Happy St. Pat's Day!!!   Well, I think I got that line split bug
		by the @#$@ this time.  It was a bug in how I handle the 
		multiplexing of file descriptors.  Well.. Try this and please
		let me know how it goes for you.
		Changed: rl.c	-- rse

3/16/99		Fixed a bug in the prompt on split line code.
		Changed: rl.c  parse.c	-- rse

3/7/99	        Well, I've decided to take Happy's mods, and add almost all of
		them.  Some were re-coded by myself, the rest was just ported
		from his version to mine.  Thanks to Sverre Normann for his 
		modifications, and allowing me to use them.  I've included his
		MODIFICATIONS file, for explainations on what he's done. 
	        Changed:  main.c, variables.c, function.c, parse.c, ivars.c
		misc.c  --rse	

3/5/99		Started to make additions that Happy (Iny), made to his mod'ed
		version of v1.61.  This go-around I just added code for the
		#ifstrequal command.  This is how it'll work... (Quoted from
		Happy's Modification page)

   		Command: ifstrequal - string comparisment (NEW)
		Syntax:  #ifstrequal {str1} {str2} {dothis} else {dothis2}
                Example: #alias {tell} {#ifs {$1} {$whoami} {#showme [Tell 
			 to self]: %0} else {Tell %0}}

3/3/99		Addition #2 - Added the ability to have a mud prompt be 
		displayed on the status line of a split session. 
		Good for those fights that scroll the screen like a bat out
		of hell.
		To set the prompt, it'll search incoming text like an action.
		#setprompt {^HP: %1  MV: %2  MANA: %3} would be used to set
		a trigger to grab mud prompts like:
		HP: 15/100  MV: 99/105  MANA: 94/117

		It will display that prompt on the status line.

		#clearprompt removes the printing of the prompt.

		If a prompt was set, it will be saved with ANY coms file you
		write.  The only way a prompt will not be saved with a coms
		file is if you issue a #clearprompt first.

		Changed: main.c, files.c, parse.c, misc.c, rl.c  -- rse	

3/3/99		Added a #random function.  The syntax will be:
		#random {variable name} {x}
		
		It will generate a random # from 1 to x, and store that value
		in {variable name}.  Now I have to work on, if it's possible, 
		#case statement so that you can do random things off this
		result.
		Changed:  parse.c, misc.c, tintin.h -- rse

2/27/99		Yes, once again, time flew, but with the resurgance of a
		healthy laptop, and work finally slowing down, things are 
		beginning to look good for progress on the project once
		again.  I've made some changes to the Makefile.. It should
		now compile just fine under Redhat 5.2.  I've decided to
		remove the Readline v2.0 library that was packaged with v1.65.
		Most Unicies have Readline installed.  If for some reason it
		isn't, you can FTP if from any GCC archive.  Gripe me if you
		can't find readline, and install it.  I will try to help as
		best I can.
		Changed:  Makefile.in, removed readline  -- rse

7/27/98		My lord..  Yes.. Time flies.. Well, work has begun on the
		Tintin++ project again.  I've got a fix here that'll 
		hopefully help out those linux users who would have the
		program core if you did a #end in the middle of a session.
		Changed:  rl.c		-- rse
