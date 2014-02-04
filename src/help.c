/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2004 (See CREDITS file)                                     *
*                                                                             *
*   This program is protected under the GNU GPL (See COPYING)                 *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
******************************************************************************/

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2004                       *
******************************************************************************/

#include "tintin.h"


struct help_type
{
	char                  * name;
	char                  * text;
};

/*
	This help table is a mess, but I got better things to do - Scandum
*/

const struct help_type help_table[] =
{
	{
		"ALIAS",
		"\n"
		"Command: #alias {word} {commands}\n"
		"\n"
		"         Defines a word that actually means a different word. Useful for long\n"
		"         commands repeated often.  You can have multiple commands aliased to\n"
		"         to a single word, if you enclose the alias in {}.\n"
		"\n"
		"         If the commands include variables %0-9, these are substituted as\n"
		"         part of the parsing.  %0 is set to all text after the word was input,\n"
    		"         %1 is set to the first word following the aliased word, and %2 is the\n"
		"         second, and so on.\n"
		"\n"
		"Example: #alias gb get bread bag\n"
		"         typing gb at the prompt would be sent as 'get bread bag'.\n"
		"Example: #alias ws {wake;stand}\n"
		"         since ws aliases a multiple command, you must use the braces.\n"
		"Example: #alias heal cast 'heal' %1\n"
		"         typing 'heal valgar' would be sent as 'cast 'heal' valgar'\n"
	},
	{
		"ACTION",
		"\n"
		"Command: #action {search string} {commands} {priority}\n"
		"\n"
		"         Have tintin search for a certain string of text from the mud, if the\n"
		"         string is found it will execute the commands.  Variables %0 to %9\n"
		"         are substituted from the input string, and can be used in the command.\n"
		"         Brackets can be used for selective keywords\n"
		"         The third parameter of the command will tell how important tintin\n"
		"         should reguard this action, 0 most important, 9 least important.  If\n"
		"         no priority is given, a default value of 5 is assigned.\n"
		"\n"
		"         If the search string starts with ~, color codes must be matched.\n"
		"         If the search string starts with ^, the start of the line must match.\n"
		"         If the search string ends with ^, the end of the line must match.\n"
		"\n"
		"Example: #act {^You are hungry^} {get bread backpack;eat bread}\n"
		"         Performs 2 commands as you get the hunger message, tintin will not be\n"
		"         fooled by someone saying 'You are hungry'.\n"
		"Example: #act {%0 tells you 'hello'} {tell %0 Hello %0. How are you today?}\n"
		"         This allows you to seem like a nice, attentive, fast typing gentleman.\n"
		"Example: #act {A puppy barks at you, wagging [his|her|its] tail.} {slap puppy}\n"
		"         This allows you to deal with annoy puppies regardless of their sex.\n"
	},
	{
		"ALL",
		"\n"
		"Command: #all {string}\n"
		"\n"
		"Send a command to all active sessions.\n"
		"\n"
		"Example: #all quit\n"
	},
	{
		"BELL",
		"\n"
		"Command: #bell\n"
		"\n"
		"         Sends a beep to your terminal.\n"
		"\n"
		"Example: #action {tells you} {#bell}\n"
		"         Will chime every time someone gives you a tell.\n"
	},
	{
		"BUFFER",
		"\n"
		"Command: #buffer {home|up|down|end|find|write filename|info}\n"
		"         The buffer command allows you to add macros to scroll in case the\n"
		"         default bindings do not work. The write option allows you to save\n"
		"         the entire scrollback buffer to file.\n"
	},
	{
		"CHAT",
		"\n"
		"Command: #chat {option} {argument}\n"
		"\n"
		"         #chat {init}       {port}             Initilizes a chat port.\n"
		"         #chat {name}       {name}             Sets your chat name.\n"
		"         #chat {message}    {buddy|all} {text} Sends a chat message\n"
		"\n"
		"         #chat {accept}     {buddy} {boosted}  Accept a file transfer\n"
		"         #chat {call}       {address} {port}   Connect to a buddy\n"
		"         #chat {cancel}     {buddy}            Cancel a file transfer\n"
		"         #chat {color}      {color names}      Set the default color\n"
		"         #chat {decline}    {buddy}            Decline a file transfer\n"
		"         #chat {dnd}                           Decline new connections\n"
		"         #chat {download}   {buddy}            Set your download dir\n"
		"         #chat {emote}      {buddy|all} {text} Send an emote message\n"
		"         #chat {forward}    {buddy}            Forward all chat messages\n"
		"         #chat {forwardall} {buddy}            Forward all session output\n"
		"         #chat {filestat}   {buddy}            Show file transfer data\n"
		"         #chat {group}      {buddy} {name}     Assign a chat group\n"
		"         #chat {ignore}     {buddy}            Ignores someone\n"
		"         #chat {info}                          Displays your info\n"
		"         #chat {ip}         {address}          Changes your IP address\n"
		"         #chat {paste}      {buddy|all} {text} Pastes a block of text\n"
		"         #chat {peek}       {buddy}            Show one's public connections\n"
		"         #chat {ping}       {buddy}            Display response time\n"
		"         #chat {private}    {buddy|all}        Make a connection private\n"
		"         #chat {public}     {buddy|all}        Make a connection public\n"
		"         #chat {reply}      {text}             Reply to last private message\n"
		"         #chat {request}    {buddy}            Request one's public connections\n"
		"         #chat {send}       {buddy|all} {text} Sends a raw data string\n"
		"         #chat {sendfile}   {buddy} {filename} Start a file transfer\n"
		"         #chat {serve}      {buddy}            Forward all public chat messages\n"
		"         #chat {who}                           Show all connections\n"
		"         #chat {zap}        {buddy}            Close a connection\n"
	},
	{
		"CLASS",
		"\n"
		"Command: #class {name} {open|close|read filename|write filename|kill}\n"
		"\n"
		"         The {open} option will open a class, closing a previously opened\n"
		"         class. All triggers added afterwards are assigned to this class.\n"
		"         The {close} option will close the given class.\n"
		"         The {read} option will open the class, read, and close afterwards.\n"
		"         The {write} option will write all triggers of the given class to file.\n"
		"         The {kill} option will delete all triggers of the given class.\n"
		"\n"
		"         Keep in mind that the kill and read option are very fast allowing\n"
		"         them to be used to enable and disable classes.\n"
	},
	{
		"COLORS",
		"\n"
		"Format:  <<888>abc>  with a, b, c being parameters\n"
		"\n"
		"         Parameter 'a': VT102 code\n"
		"\n"
		"         0 - Reset all colors and codes to default\n"
		"         1 - Bold\n"
		"         2 - Dim\n"
		"         4 - Underscore\n"
		"         5 - Blink\n"
		"         7 - Reverse\n"
		"         8 - Skip (use previous code)\n"
		"\n"
		"         Parameter 'b':  Foreground color\n"
		"         Parameter 'c':  Background color\n"
		"\n"
		"         0 - Black                5 - Magenta\n"
		"         1 - Red                  6 - Cyan\n"
		"         2 - Green                7 - White\n"
		"         3 - Yellow               8 - Skip\n"
		"         4 - Blue                 9 - Default\n"
		"\n"
		"For xterm 256 color support use <<888>aaa> to <<888>fff> for rgb foreground colors and\n"
		"<<888>AAA> to <<888>FFF> for rgb background colors.\n"
		"\n"
		"Example: #showme <<888>115>t<<888>828>e<<888>838>s<<888>848>t<<888>088> <<888>468>!<<888>568>!<<888>088>\n"
		"         This will show: <115>t<828>e<838>s<848>t<088> <468>!<568>!<088>\n"
	},
	{
		"CONFIG",
		"\n"
		"Commands: #config {option} {argument}\n"
		"\n"
		"          This allows you to configure various settings, the settings can be\n"
		"          written to file with the #write or #writesession command.\n"
		"\n"
		"          If you configure the global session (the one you see as you start up\n"
		"          tintin) all sessions started will inherite these settings.\n"
		"\n"
		"          It's advised to make a configuration file to read on startup if you\n"
		"          do not like the default settings.\n"
		"\n"
		"          Config options which aren't listed by default:\n"
		"\n"
		"          #CONFIG {CONVERT META} {ON|OFF} Shows color codes and key bindings.\n"
		"          #CONFIG {DEBUG TELNET} {ON|OFF} Shows telnet negotiations y/n.\n"
		"          #CONFIG {LOG LEVEL}  {LOW|HIGH} LOW logs mud output before triggers\n"
	},
	{
		"CR",
		"\n"
		"format:	      #cr\n"
		"\n"
		"Sends a carriage return to the session.  Useful for aliases that need extra\n"
		"carriage returns.\n"
	},
	{
		"CREDITS",
		"\n"
		"<068>    #<068>####################################################################<068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<078>                           T I N T I N + +                          <068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<078>            (<068>T<078>)he k(<068>I<078>)cki(<068>N<078>) (<068>T<078>)ickin d(<068>I<078>)kumud clie(<068>N<078>)t            <068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<078>                 Original TINTIN code by Peter Unold                <068>#\n"
		"<068>    #<078>       new code by Bill Reiss, David A. Wagner, Rob Ellsworth,      <068>#\n"
		"<068>    #<078>                 Jeremy C. Jack, Igor van den Hoven                 <068>#\n"
		"<068>    #<078>                             1992, 2006                             <068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<078>                               "VERSION_NUM"                               <068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<068>####################################################################<068>#<088>\n\n"
	},
	{
		"CURSOR",
		"\n"
		"Command: #cursor {option}\n"
		"\n"
		"         Typing #cursor without an option will show all available cursor\n"
		"         options. The cursor command's primarly goal is adding customizable\n"
		"         input editing with macros.\n"
	},
	{
		"DELAY",
		"\n"
		"Command: #delay {seconds} {command}\n"
		"\n"
		"         Delay allows you to have tintin wait the given amount of seconds\n"
		"         before executing the given command. Floating point precision for\n"
		"         miliseconds is possible.\n"
	},

	{
		"ECHO",
		"\n"
		"Command: #echo {format} {argument1 argument2 etc}\n"
		"\n"
		"         Echo displays text on the screen with formatting options.\n"
		"\n"
		"Example: #format {time} {%T}\n"
		"         #echo {The current time and date is: %d %t} {$time $time}\n"
		"         Stores the current epoch value in the $time variable, and echoes\n"
		"         the military date and time based on this value. As I write this it\n"
		"         printed: The current time and date is: 10-03-2004 23:34\n"
	},
	{
		"END",
		"\n"
		"Command: #end\n"
		"\n"
		"         Terminate tintin++ and return to unix.  On most systems, ctrl-c has\n"
		"         the same result.\n"
	},
	{
		"GAG",
		"\n"
		"format:        #gag {string}\n"
		"\n"
		"Just like substitute, but the gag char is added for you.  Removes any line that\n"
		"contains the string.\n"
	},
	{
		"GAGLINE",
		"\n"
		"Command: #gagline\n"
		"\n"
		"When called from within an action the trigger string is not displayed.\n"
	},
	{
		"HELP",
		"\n"
		"Command: #help [subject]\n"
		"\n"
		"         Without an argument #help will list all available help subjects.\n"
	},
	{
		"HIGHLIGHT",
		"\n"
		"Command: #highlight {string} {color names}\n"
		"\n"
		"         The highlight command is used to allow you to highlight strings of text\n"
		"         from the mud.  Available color names are:\n"
		"\n"
		"         default, light, faint, underscore, blink, reverse, dim, black, red,\n"
		"         green, yellow, blue, magenta, cyan, white, b black, b red, b green,\n"
		"         b yellow, b blue, b magenta, b cyan, b white\n"
		"\n"
		"         The %0-9 variables can be used as 'wildcards' that will match with any\n"
		"         text. They are useful for highlighting a complete line.\n"
		"         You may start the string to highlight with a ^ to only highlight text\n"
		"         if it begins the line.\n"
		"\n"
		"Warning: This command is only compatible with ANSI/VT100 terminals or emulators.\n"
		"\n"
		"Example: #high {Valgar} {reverse}\n"
		"         Print every occurrence of 'Valgar' in reverse video\n"
		"Example: #high {^You %0} {bold cyan}\n"
		"         boldface any 'You' that starts a line in cyan\n"
		"Example: #high {Scandum} {red underscore blink\n"
		"         highlight the name Scandum as blinking, red, underscored text\n"
		"\n"
		"Notice:  See #help substitute, for more advanced color substitution.\n"
	},
	{
		"HISTORY",
		"\n"
		"Command: #history {delete} \n"
		"         #history {insert} {command}\n"
		"\n"
		"         Either deletes the last command or inserts a new last command.\n"
		"         Without an argument your entire command history is shown.\n"
	},
	{
		"IF",
		"\n"
		"Command: #if {conditional} {commands if true} {commands if false}\n"
		"\n"
		"         The if command is one of the most powerful commands added since\n"
		"         TINTIN III. It works similar to an if statement in other languages, and is\n"
		"         strictly based on the way C handles its conditional statements.\n"
		"         When an if command is encountered, the conditional statement is evaluated,\n"
		"         and if TRUE (any non-zero result) the commands are executed.  The if\n"
		"         statement is only evaluated if it is read, so you must nest the if statement\n"
		"         inside another statement (most likely an action command).  The conditional\n"
		"         is evaluated exactly the same as in the math command, only instead of\n"
		"         storing the result, the result is used to determine whether to execute the\n"
		"         commands.  See #help math, for more information.\n"
		"\n"
		"Example: #action {%0 gives you %1 gold coins.} {#if {%1>5000} {thank %0}}\n"
		"         If someone gives you more than 5000 coins, thank them.\n"
	},
	{
		"IGNORE",
		"\n"
		"Command: #ignore {listname} {on|off}\n"
		"\n"
		"         Toggles a list on or off. With no arguments it shows your current\n"
		"         settings, as well as the list names that you can ignore.\n"
		"\n"
		"         If you for example set ACTIONS to OFF actions will no longer trigger.\n"
		"         Not every list can be ignored.\n"
	},
	{
		"DEBUG",
		"\n"
		"Command: #debug {listname} {on|off|log}\n"
		"\n"
		"         Toggles a list on or off. With no argument it shows your current\n"
		"         settings, as well as the list names that you can debug.\n"
		"\n"
		"         If you for example set ACTIONS to ON you will get debug information\n"
		"         whenever an action is triggered.\n"
		"\n"
		"         #debug {listname} {log} will silently write debugging information to\n"
		"         the log file, you must be logging in order for this to work.\n"
		"\n"
		"         Not every list has debug support yet.\n"
	},
	{
		"ESCAPE",
		"\n"
		"You may use the escape character \\ for various special characters.\n"
		"\n"
		"\\a   will beep the terminal.\n"
		"\\e   will start an escape sequence.\n"
		"\\n   will send a new line.\n"
		"\\r   will send a carriage return.\n"
		"\\t   will send a tab.\n"
		"\\x   will print a hexadecimal value, \xFF for example.\n"
		"\\x7B will send the '{' character.\n"
		"\\x7D will send the '}' character.\n"
		"\n"
		"Ending a line with \\ will stop tintin from appending \\r\\n. To escape\n"
		"arguments in an alias use %%90 %%91 %%92 etc.\n"
	},
	{
		"FORALL",
		"\n"
		"Command: #forall {list} {command}\n"
		"         How this works is best shown with an example\n"
		"Example: #forall {a b c d} {say &0}\n"
		"         This equals: say a;say b;say c;say d;\n"
		"         Useful for whenever it's useful.\n"
	},
	{
		"FORMAT",
		"\n"
		"Command: #format {variable} {format} {argument1 argument2 etc}\n"
		"         Allows you to store a string into a variable in the exact same way\n"
		"         C's sprintf works with a few enhancements and limitations such as\n"
		"         no integer operations and a maximum of 20 arguments. If you use format\n"
		"         inside an alias or action you must escape the %0-9 like: %+4s.\n"
		"\n"
		"Example: #format {test} {%+9s} {string}  pad string with up to 9 spaces\n"
		"         #format {test} {%-9s} {string}  post pad string with up to 9 spaces\n"
		"         #format {test} {%.8s} {string}  copy at most 8 characters\n"
		"         #format {test} {%c}   {string}  turn string into a color code\n"
		"         #format {test} {%d}     {time}  print the current military day\n"
		"         #format {test} {%h}   {string}  turn text into a header line\n"
		"         #format {test} {%l}   {string}  lowercase text\n"
		"         #format {test} {%m}   {string}  perform mathematical calculation\n"
		"         #format {test} {%n}     {name}  capitalize the first letter\n"
		"         #format {test} {%p}   {string}  strip leading and trailing spaces\n"
		"         #format {test} {%r}   {string}  reverse text, hiya = ayih\n"
		"         #format {test} {%t}     {time}  print the current military time\n"
		"         #format {test} {%u}   {string}  uppercase text\n"
		"         #format {test} {%C}   {colums}  store the screen width in {colums}\n"
		"         #format {test} {%D}     {time}  print the current day\n"
		"         #format {test} {%G}   {number}  perform thousand grouping on {number}\n"
		"         #format {test} {%L}   {string}  store the string length in {test}\n"
		"         #format {test} {%M}     {time}  print the current month\n"
		"         #format {test} {%R}     {rows}  store the screen height in {rows}\n"
		"         #format {time} {%T}             store the epoch time in {time}\n"
		"         #format {time} {%U}             store the micro epoch time in {time}\n"
		"         #format {time} {%Y}     {time}  print the current year\n"
	},
	{
		"FUNCTION",
		"\n"
		"Command: #function {name} {operation}\n"
		"\n"
		"Functions are a mixture of aliases and variables. The following examples will\n"
		"have to do to explain how to use functions, result is a special variable you\n"
		"can use for functions, and if used will place the result at the place where\n"
		"the function was found.\n"
		"\n"
		"Example:\n"
		"#function {rnd} {#math result &1 - &0;#format result %R $result;#math result $result + &0}\n"
		"#showme A random number between 100 and 200: @rnd{100 200}\n"
		"\n"
		"Functions are treated much like variables. Their value is\n"
		"a command-line which is executed, and the functions are substituted\n"
		"by the parameter last sent to the #result command.\n"
		"\n"
		"Example:\n"
		"#function gettime {#format temp %T;#format result %t $temp;#unvar temp}\n"
		"#showme The time is @gettime{}\n"
		"This will print the current military time.\n"
	},
	{
		"GREP",
		"\n"
		"Command: #grep [page] {search string}\n"
		"         This command allows you to search for matching lines in your scroll\n"
		"         back buffer. The amount of matches shown equals your screen size. If\n"
		"         you want to search back further use a page number. You can use\n"
		"         wildcards for better search results. Default * *'s are placed around\n"
		"         the search string to make things a little easier.\n"
	},
	{
		"HISTORY",
		"\n"
		"Command: #history {insert|delete}\n"
		"\n"
		"         Insert will insert the given command as the last issued command.\n"
		"         Delete will delete the last issued command from the history list.\n"
		"\n"
		"         Without an argument the entire command history list will be shown\n"
	},
	{
		"INFO",
		"\n"
		"Command: #info\n"
		"\n"
		"         Displays all the settings of every tintin list available.\n"
	},
	{
		"KILL",
		"\n"
		"Command: #kill {list|all}\n"
		"\n"
		"         Without an argument kill deletes all lists.  Useful so you don't\n"
		"         have to exit tintin++ to load up a new command file.\n"
		"\n"
		"         With an argument a specific list can be killed.\n"
	},
	{
		"LIST",
		"\n"
		"Command: #list {variable} {del|ins|get|set|len} {argument}\n"
		"\n"
		"#list {list} {del} {index}             Delete an item from the list\n"
		"#list {list} {ins} {index} {string}    Insert {string} at given index\n"
		"#list {list} {fnd} {string} {variable} Return index if {string} is found\n"
		"#list {list} {get} {index} {variable}  Copy an item to {variable}\n"
		"#list {list} {set} {index} {string}    Change an item at the given index\n"
		"#list {list} {len} {variable}          Copy list length to {variable}\n"
		"\n"
		"The index should be between 1 and the list's length. You can also give\n"
		"a negative value, in which case -1 equals the last item in the list, -2\n"
		"the second last, etc.\n"
		"\n"
		"When inserting an item a positive index will prepend the item at the given\n"
		"index, while a negative index will append the item.\n"
		"\n"
		"A length of 0 is returned for an empty or non existant list."
	},
	{
		"LOADPATH",
		"\n"
		"Command: #loadpath {alias name}\n"
		"\n"
		"Load a saved path alias back into the path, useful for the #walk command.\n"
	},
	{
		"LOG",
		"\n"
		"Command: #log {append|overwrite} {filename}\n"
		"\n"
		"         Log session to a file, you can set the data type with #config.\n"
	},
	{
		"LOGLINE",
		"\n"
		"Command: #logline {filename} {text}\n"
		"\n"
		"        If used in an an action it will log that line to the given file.\n"
		"        The {text} argument is optional, and if used it will instead log\n"
		"        the given text instead.\n"
	},
	{
		"LOOP",
		"\n"
		"Command: #loop {start finish} {commands}\n"
		"Command: #loop {$variable} {commands}\n"
		"\n"
		"Like a for statement, loop will loop from start to finish incrementing or\n"
		"decrementing by 1 each time through.  The value of the loop variable is\n"
		"placed in &0, which you can use in the commands.\n"
		"\n"
		"If used with only a start the loop will behave like a while loop. As long\n"
		"as the variable is non zero the loop will run.\n"
		"\n"
		"examples:\n"
		"\n"
		"#loop {1 3} {get all &0.corpse}\n"
		"equivalent to the following:\n"
		"  get all 1.corpse;get all 2.corpse;get all 3.corpse\n"
		"\n"
		"#loop {3,1} {drop &0.key}\n"
		"equivalent to\n"
		"  drop 3.key;drop 2.key;drop 1.key\n"
	},
	{
		"MACRO",
		"\n"
		"Command: #macro {key sequence} {commands}\n"
		"\n"
		"         Macros allows you to make tintin respond to function keys.\n"
		"\n"
		"         The key sequence send to the terminal when pressing a function key\n"
		"         differs for every OS and terminal. To find out what sequence is send\n"
		"         you can enable the CONVERT META config option.\n"
		"\n"
		"         Another option is pressing ctrl-v, which will enable CONVERT META for\n"
		"         the next key pressed.\n"
		"\n"
		"Example: #macro {(press ctrl-v)(press F1)} {shout TinTin++ is the greatest!\n"
		"\n"
		"         The next time you press F1 the entered command should be executed.\n"
		"\n"
		"Keypad:  Not all terminals properly initialize the keypad key sequences.\n"
		"         If this is the case you can still use the keypad, but instead of the\n"
		"         arrow keys use ctrl b, f, p, and n.\n"
	},
	{
		"MAP",
		"\n"
		"Command: #map\n"
		"\n"
		"The map command is the backbone of the auto mapping feature. In order to use\n"
		"mapping for muds using ne se sw nw you need to create a #pathdir for each.\n"
		"\n"
		"#map color: Sets the map color of the current room you are in.\n"
		"\n"
		"#map create: Creates a new map and room 1, required to start mapping.\n"
		"\n"
		"#map destroy: Deletes the map.\n"
		"\n"
		"#map delete <direction>: Deletes the room in the given direction.\n"
		"\n"
		"#map dig <direction>: Create an exit in the given direction. If no valid\n"
		"direction is given or no existing room is found a new room is created.\n"
		"Useful for portal links and other alternative forms of transportation.\n"
		"\n"
		"#map exit <exit name> <exit commands>: Set the exit command. This is useful if\n"
		"you are dealing with a closed door where you would set the exit command with:\n"
		"#map exit e {open east;e}. Keep in mind the exit command is only used when\n"
		"using #map find and #map walk.\n"
		"\n"
		"#map find <room name>: Searches for the given room name. If found the shortest\n"
		"path from your current location to the target location is calculated. The route\n"
		"is stored in #path and can subsequently be used with #walk.\n"
		"\n"
		"#map flag static: Will make the map static so new rooms are no longer created\n"
		"when walking into an unmapped direction. Useful when you're done mapping and\n"
		"regularly bump into walls accidentally creating a new room.\n"
		"\n"
		"#map flag vtmap: Will enable the vtmap which is shown in the top split screen\n"
		"if you have one. You can create a 10 rows high top screen by using #split 10 1.\n"
		"\n"
		"#map goto <room name>: Takes you to the given room name.\n"
		"\n"
		"#map info: Gives information about the map and room you are in.\n"
		"\n"
		"#map leave: Makes you leave the map. Useful when entering a maze.\n"
		"\n"
		"#map legenda <symbols>: The legend exists of 17 decimal numbers which represent\n"
		"character symbols used for drawing a map. Binary order is used with the n e s w"
		"representing bit 1 to 4. Number 2 to 16 stand for: n e ne s ns es nes w nw ew\n"
		"new sw nsw esw nesw. Number 1 stands for a no exit room, and number 17 for the\n"
		"room the player is currently in. The legenda is set by default, but can be\n"
		"adjusted to take advantage of fonts with line drawing characters. To check your\n"
		"font for drawing characters use: #loop {32 255} {#echo {%-3s  %a} {&0 &0}}\n"
		"\n"
		"#map link <direction> <room name>: Links two rooms. If a valid direction is\n"
		"given the link is two way.\n"
		"\n"
		"#map list: Lists all room names and exits.\n"
		"\n"
		"#map map <radius>: shows a map of surrounding rooms.\n"
		"\n"
		"#map move <direction> This does the same as an actual movement command, updating\n"
		"your location on the map and creating new rooms. Useful when you are following\n"
		"someone and want the map to follow. You will need to create actions using\n"
		"#map move, for this to work.\n"
		"\n"
		"#map name <room name>: Allows changing the room name, by default rooms have the\n"
		"same name as their internal index number\n"
		"\n"
		"#map read <filename>: Will load the given map file.\n"
		"\n"
		"#map roomflag avoid: When set #map find will avoid a route leading through that\n"
		"room. Useful when you want to avoid aggressive mobs.\n"
		"\n"
		"#map roomflag hide: When set #map will not display the map beyond this room.\n"
		"When mapping overlapping areas or areas that aren't build consistently you need\n"
		"this flag as well to stop auto-linking.\n"
		"\n"
		"#map roomflag leave: When entering a room with this flag you will automatically\n"
		"leave the map. Useful when set at the entrance of an unmappable maze.\n"
		"\n"
		"#map undo: Will delete the room you are in and move you out. Useful if you\n"
		"walked into a non existant direction.\n"
		"\n"
		"#map unlink <direction>: Will remove the exit, this is never two ways so you\n"
		"can have the map properly display no exit rooms and mazes.\n"
		"\n"
		"#map walk <room name>: Calculates the shortest path to the destination and\n"
		"walks you there.\n"
		"\n"
		"#map write <filename>: Will save the map.\n"
	},
	{
		"MARK",
		"\n"
		"Command: #mark\n"
		"\n"
		"Clear the path list and start the new path at your current location.\n"
	},
	{
		"MATH",
		"\n"
		"Command: #math {variable} {expression}\n"
		"\n"
		"Performs math functions and stored the result in a variable.  The math\n"
		"follows a C-like precedence, as follows, with the top of the list having\n"
		"the highest priority.\n"
		"\n"
		"Operators       Priority     Function\n"
		"------------------------------------------------\n"
		"!               0            logical not\n"
		"*               1            integer multiply\n"
		"/               1            integer divide\n"
		"%               1            integer modulo\n"
		"d               1            integer random dice roll\n"
		"+               2            integer addition\n"
		"-               2            integer subtraction\n"
		">               4            logical greater than\n"
		">=              4            logical greater than or equal\n"
		"<               4            logical less than\n"
		"<=              4            logical less than or equal\n"
		"==              5            logical equal (can use wildcards)\n"
		"!=              5            logical not equal (can use wildcards)\n"
		"&&              9            logical and\n"
		"^^             10            logical xor\n"
		"||             11            logical or\n"
		"\n"
		"True is any non-zero number, and False is zero.  Parentheses () have highest\n"
		"precedence, so inside the parentheses is always evaluated first.\n"
		"Strings must be enclosed in \" \" and are evaluated in alphabetic order.\n"
		"\n"
		"examples:\n"
		"#math {heals} {$mana / 40}\n"
		"Assuming there is a variable $mana, divide its value by 40 and store the\n"
		"result in $heals.\n"
		"#action {^You receive %0 experience} {updatexp %0}\n"
		"#alias updatexp {#math {xpneed} {$xpneed - %0}\n"
		"Let's say you have a variable which stores xp needed for your next\n"
		"level.  The above will modify that variable after every kill, showing\n"
		"the amount still needed.\n"
		"#act {%0 tells %1} {#if {\"\%0\" == \"Scandum\" && $afk} {reply $afk Sir} {reply $afk dude}}\n"
	},
	{
		"MESSAGE",
		"\n"
		"Command: #message {listname} {on|off}\n"
		"\n"
		"This will show the message status of all your lists if typed without an\n"
		"argument. If you set for example VARIABLES to OFF you will no longer be spammed\n"
		"when correctly using the #VARIABLE and #UNVARIABLE commands.\n"
	},
	{
		"NOP",
		"\n"
		"Command: #nop [string]\n"
		"\n"
		"Short for 'no operation', and is ignored by the client.  It is useful for\n"
		"commenting in your coms file, any text after the nop and before a semicolon\n"
		"or end of line is ignored. You shouldn't put braces { } in it though, unless\n"
		"you close them properly.\n"
		"\n"
		"example:\n"
		"#nop This is the start of my autoactions\n"
	},
	{
		"PARSE",
		"\n"
		"Command: #parse {string} {commands}\n"
		"\n"
		"Like a for statement, parse will loop from start to finish through the\n"
		"given string. The value of the character parse is at will be placed in\n"
		"the &0 variable.\n"
	},
	{
		"PATH",
		"\n"
		"Command: #path {command1} {command2}\n"
		"\n"
		"Allows to add additional directions to your path.\n"
		"Example:\n"
		"#path {unlock n;open n} {unlock s;open s}\n"
	},
	{
		"PATHDIR",
		"\n"
		"format:	       #pathdir {dir} {reversed dir}\n"
		"\n"
		"This command only works for the #mark command currently.\n"
		"\n"
		"example:\n"
		"#pathdir {NW} {SE}\n"
		"each time {NW} is excountered while marking a path tintin++ will add {NW} {SE to\n"
		"the walk stack. Notice NW isn't seen as a speedwalk due to capitalization.\n"
	},
	{
		"PROMPT",
		"\n"
		"Command: #prompt {text} {new text} {row #}\n"
		"\n"
		"         Prompt is a feature for split window mode (see #help split), which\n"
		"         will take the status prompt from the mud, and display it on the status\n"
		"         line of the split window.  You would define <string> the same way you\n"
		"         would a substitution (see #help substitution). The row number is\n"
		"         optional if you use a non standard split mode. In which case a\n"
		"         positive number draws #row lines above your cursor, and a negative\n"
		"         number draws #row lines below your topline\n"
	},
	{
		"READ",
		"\n"
		"Command: #read {filename}\n"
		"\n"
		"         Reads a coms file into memory.  The coms file is merged in with the\n"
		"         currently loaded commands.  Duplicate commands are overwritten.\n"
		"\n"
		"         If you uses braces, { and } you can use several lines for 1 commands.\n"
		"         This however means you must always match every { with a } for the read\n"
		"         command to work.\n"
		"\n"
		"         You can comment text using /* test */\n"
	},

	{
		"REPLACESTRING",
		"\n"
		"Command: #replacestring {variable} {oldtext} {newtext}\n"
		"\n"
		"Searches the variable text replacing each occurance of 'oldtext' with 'newtext'\n"
	},
	{
		"SAVEPATH",
		"\n"
		"Command: #savepath {forward|backward} alias name}\n"
		"\n"
		"Save the current path to an alias, which can then be saved to a coms file.\n"
		"The alias name provided will be the new name of the alias to represent\n"
		"that path.  Useful for after mapping to be able to get back to the place\n"
		"again.\n"
	},
	{
		"RETURN",
		"\n"
		"Command: #return {text}\n"
		"\n"
		"The return command can be used in an #if check to break out of a command\n"
		"string being executed. In a #function you can use #return with an argument\n"
		"to both break out of the function and set the result variable.\n"
	},
	{
		"SCAN",
		"\n"
		"Command: #scan {filename}\n"
		"\n"
		"Reads in a file and sends it to the screen as if it was send by a mud. This is\n"
		"useful to convert ansi color files to html or viewing log files.\n"
	},
	{
		"SEND",
		"\n"
		"Command: #send {text}\n"
		"\n"
		"Sends the text directly to the MUD, useful if you want to start with an\n"
		"escape code.\n"
	},
	{
		"SESSION",
		"\n"
		"Command: #session {name} {host port}\n"
		"\n"
		"Starts a telnet session with the given name, host, and port. The name can\n"
		"be anything you want, except the name of an already existant session.\n"
		"\n"
		"Without an argument #session shows the currently active session.\n"
		"\n"
		"If you have more than 1 session you can use #session {-|+|number} to switch\n"
		"between them. Using #{name} allows switching to a session with the given name.\n"
		"\n"
		"#{name} {command} allows executing a command with the given session without\n"
		"changing the active session.\n"
		"The startup session is named 'gts' and can be used for auto relog scripts.\n"
	},

	{
		"SHOWME",
		"\n"
		"Command: #showme {string} {row}\n"
		"\n"
		"Display the string to the terminal, do not send to the mud.  Useful for\n"
		"status, warnings, etc. The {row} number is optional and works the same way as\n"
		"the row number of the #prompt trigger.\n"
		"example:\n"
		"#action {%0 ultraslays you} {#showme {###### argh! we were ultraslayed ######}}\n"
	},
	{
		"SNOOP",
		"\n"
		"Command: #snoop {session name}\n"
		"\n"
		"If there are multiple sessions active, this command allows you to see what\n"
		"is going on the the sessions that are not currently active.  The lines\n"
		"of text from other sessions will be prefixed by 'session name%'.\n"
	},
	{
		"SPEEDWALK",
		"\n"
		"Speedwalking allows you to type multiple directions not seperated by\n"
		"semicolons, and now it lets you prefix a direction with a number, to signify\n"
		"how many times to go that direction. You can turn it on/off with #config.\n"
		"\n"
		"example:\n"
		"without speedwalk:\n"
		"s;s;w;w;w;w;w;s;s;s;w;w;w;n;n;w\n"
		"with speedwalk:\n"
		"2s5w3s3w2nw\n"
	},
	{
		"SPLIT",
		"\n"
		"Command: #split {top lines} {bottom lines}\n"
		"\n"
		"This option only works with a vt102 emulator, this will allow you to set up a\n"
		"screen.  The keyboard input will be displayed in the bottom window,\n"
		"while mud text is displayed in the center window.  This requires a fairly\n"
		"decent emulator, but works on most I have tested.\n"
		"\n"
		"If used without an argument the screen will be split in a top and bottom window.\n"
		"If you want a different split setting, because you want a 2 lines thick split, or\n"
		"like having a blank line at the top of the screen to display something you can\n"
		"set the top and bottom line with the additional arguments.\n"
		"\n"
		"example:\n"
		"#split\n"
		"If tintin has determined you have a screen of 30 rows, it will set the top line\n"
		"to 1 and the bottom line to 28.\n"
		"You can display text on the split line(s) with the #prompt command.\n"
	},
	{
		"SUBSTITUTE",
		"\n"
		"Command: #substitute {text} {new text}\n"
		"\n"
		"Allows you to replace original text from the mud with different text, or\n"
		"delete it altogether.  This is helpful for if you have a slow modem, or\n"
		"there is a lot of text sent during battles, and the like.  the %0-9\n"
		"variables can be used to capture text and use it as part of the new output,\n"
		"and the ^ char is valid to only check the beginning of the line for the\n"
		"text specified.\n"
		"\n"
		"If a . is the only character in the new text argument, if the line is\n"
		"matched, it will be deleted.\n"
		"\n"
		"If only one argument is given, all subs that match the string are displayed.\n"
		"The * char is valid in this instance.  See help wildcard.\n"
		"\n"
		"If no argument is given, all subs are displayed.\n"
		"\n"
		"examples:\n"
		"#sub {leaves} {.}\n"
		"gag any line that has the word 'leaves' in it.\n"
		"#sub {^Zoe%0} {ZOE%0}\n"
		"any line that starts with the name Zoe will be replaced by a line that\n"
		"starts with 'ZOE'\n"
		"#sub {%0massacres%1} {<<888>018>%0<<888>118>MASSACRES<<888>018>%1}\n"
		"replace all occurrences of 'massacres' with 'MASSACRES' and change the colors,\n"
		"see #help colors, for more information.\n"
	},
	{
		"SUSPEND",
		"\n"
		"Command: #suspend\n"
		"\n"
		"Temporarily suspends tintin++ and returns you to your shell.  The\n"
		"effect of this command is exactly as if you had typed control-z.\n"
		"To return to tintin++, type 'fg' at the shell prompt.\n"
	},
	{
		"SYSTEM",
		"\n"
		"Command: #system {command}\n"
		"\n"
		"Executes the command specified as a shell command.\n"
	},
	{
		"TAB",
		"\n"
		"Command: #tab {word}\n"
		"\n"
		"Adds a word to the tab completion list, alphabetically sorted.\n"
	},
	{
		"TEXTIN",
		"\n"
		"format:		#textin {filename}\n"
		"\n"
		"Textin now allows the user to read in a file, and send its contents directly\n"
		"to the mud.  Useful for doing online creation, or message writting.\n"
	},
	{
		"TICK",
		"\n"
		"Command: #tick {name} {commands} {interval in seconds}\n"
		"\n"
		"Execute given command every # of seconds.\n"
		"The epoch time (see help on format) is stored in %0\n"
	},
	{
		"UNACTION",
		"\n"
		"Command: #unaction {string}\n"
		"\n"
		"Remove action(s) from the action list which match {string}.  The\n"
		"'*' character will act as a wildcard and will match any text.\n"
		"See help wildcard.\n"
		"\n"
		"examples:\n"
		"#unaction {%0tells you%1}\n"
		"remove the action that is triggered by '%0tells you%1'\n"
		"#unaction *\n"
		"remove all actions\n"
		"#unaction {*massacre*}\n"
		"remove all actions that contain the word 'massacre'\n"
	},
	{
		"UNALIAS",
		"\n"
		"Command: #unalias {word}\n"
		"               #unalias word\n"
		"\n"
		"Remove alias(es) from the alias list which match {word}.  The\n"
		"'*' character will act as a wildcard and will match any text.\n"
		"See help wildcard.\n"
		"\n"
		"examples:\n"
		"#unalias {bbb}\n"
		"remove the alias 'bbb'\n"
		"#unalias *\n"
		"remove all aliases\n"
		"#unalias {*go*}\n"
		"remove all aliases that contain the fragment 'go'\n"
	},
	{
		"UNCLASS",
		"\n"
		"Command: #unclass {class}\n"
		"\n"
		"Removes class(es) from the class list which match {class}. All items\n"
		"associated to the class will be removed as well.\n"
	},

	{
		"UNFUNCTION",
		"\n"
		"Command: unfunction - remove a function\n"
		"\n"
		"Syntax:  #unfunction {func name}\n"
	},
	{
		"UNGAG",
		"\n"
		"Command: #ungag {string}\n"
		"               #ungag string\n"
		"\n"
		"Exactly the same as #unsubstitute, '#help unsubstitute' for info.\n"
	},
	{
		"UNHIGHLIGHT",
		"\n"
		"Command: #unhighlight {string}\n"
		"               #unhighlight string\n"
		"\n"
		"Remove highlight(s) from the highlight list which match {string}.  The\n"
		"'*' character will act as a wildcard and will match any text.\n"
		"See help wildcard.\n"
		"\n"
		"examples:\n"
		"#unhigh {%0tells you%1}\n"
		"remove the highlight for '%0tells you%1'\n"
		"#unhigh *\n"
		"remove all highlights\n"
		"#unhigh {^You*}\n"
		"remove all highlights that start with '^You'\n"
	},
	{
		"UNMACRO",
		"\n"
		"Command: #unmacro {key combination}\n"
		"\n"
		"         This command removes a macro from the list.\n"
	},
	{
		"UNPATH",
		"\n"
		"Command: #unpath\n"
		"\n"
		"Pops the last move off the top of the path\n"
		"Example\n"
		"#act {You cannot go that way.} {#unpath}\n"
	},
	{
		"UNPROMPT",
		"\n"
		"format: 	#unprompt\n"
		"\n"
		"unprompt will remove a prompt from the list.\n"
	},
	{
		"UNSPLIT",
		"\n"
		"Command: #unsplit\n"
		"\n"
		"Exit split mode, and set the screen back to its default terminal settings.\n"
	},
	{
		"UNSUBSTITUTE",
		"\n"
		"Command: #unsubstitute {string}\n"
		"               #unsubstitute string\n"
		"\n"
		"Remove substitute(s) from the substitute list which match {string}.  The\n"
		"'*' character will act as a wildcard and will match any text.\n"
		"See help wildcard.\n"
		"\n"
		"examples:\n"
		"#unsub {%0tells you%1}\n"
		"remove the sub for '%0tells you%1'\n"
		"#unsub *\n"
		"remove all subs\n"
		"#unsub {^You*}\n"
		"remove all subs that start with '^You'\n"
	},
	{
		"UNTAB",
		"\n"
		"format:	       #untab {word}\n"
		"\n"
		"Deletes a word from the tab completion list.\n"
	},
	{
		"UNTICK",
		"\n"
		"Command: #untick {name}\n"
		"\n"
		"Removes a ticker from the ticker list.\n"
	},
	{
		"UNVARIABLE",
		"\n"
		"Command: #unvariable {variable name}\n"
		"               #unalias variable name\n"
		"\n"
		"Remove variable(s) from the variable list which match {variable name}.  The\n"
		"'*' character will act as a wildcard and will match any text.\n"
		"See help wildcard.\n"
		"\n"
		"examples:\n"
		"#unvar {hp}\n"
		"remove the variable $hp\n"
		"#unvar *\n"
		"remove all variables\n"
		"#unvariable {xx*}\n"
		"remove all variables that start with 'xx'\n"
	},
	{
		"VARIABLE",
		"\n"
		"Command: #variable {<variable_name>} {<text to fill variable>}\n"
		"\n"
		"Variables differ from the %0-9 in the fact that you could specify\n"
		"a full word as a variable name, and they stay in memory for the\n"
		"full session unless they are changed. They can be saved in the\n"
		"coms file, and can be set to different values if you have two or\n"
		"more sessions running at the same time.  One of the best uses for\n"
		"variables I think is for spellcasters.\n"
		"\n"
		"#alias {target} {#var target %0}\n"
		"#alias {x}      {flame $target}\n"
		"\n"
		"The name of a variable must exist of only letters and numbers in\n"
		"order to be substituted. If you do not meet these requirements do\n"
		"not panic, simply encapsulate the variable in braces:\n"
		"\n"
		"#variable {cool website} {http://tintin.sourceforge.net}\n"
		"#chat I was on ${cool website} yesterday!.\n"
		"\n"
		"In order to check if a variable exists you can use regexp:\n"
		"\n"
		"#if {\"$target\" == \"?target\"} {#showme variable is not defined}\n"
	},
	{
		"WALK",
		"\n"
		"Command: #walk {forward|backward}\n"
		"         Walks one step of a current path, forward if no argument is given.\n"
		"Example: #mark;w;w;w;mark;#savepath backward test;#loadpath test;#3 #walk\n"
		"         Will make you start mapping, map 3 rooms, save the path backwards,\n"
		"         And make you walk back the last 3 steps. Everytime you #walk, the\n"
		"         first element of the loaded path is deleted and executed. You get\n"
		"         a message when the path is emptied.\n"
	},
	{
		"WILDCARD",
		"\n"
		"format:		#trigger {regexp}\n"
		"\n"
		"You may use wildcards with certain commands such as #alias, #action,\n"
		"#substitute, #unalias, etc.  In commands like #alias, wildcards are\n"
		"only valid when you use exactly one argument.  Wildcards are always\n"
		"valid for commands like #unalias.  The only wildcard currently\n"
		"supported is *, which matches any string 0 or more characters long.\n"
		"The wildcard meaning of * may be escaped using the backslash, \\.\n"
		"\n"
		"Examples:\n"
		"\n"
		"#action {*miss*}\n"
		"        shows all actions which contain the word miss in them\n"
		"#unaction {*miss*}\n"
		"        removes all actions which contain the word miss in them\n"
		"#unaction {\\*\\*\\* PRESS RETURN:}\n"
		"        removes the action which triggers on the line *** PRESS RETURN:\n"
		"\n"
		"Many thanks to Spencer Sun for letting me steal some code for this...\n"
		"If anyone is interested in having full ls-style regexps, email me\n"
		"and maybe it'll be included in 2.0.\n"
	},
	{
		"WRITE",
		"\n"
		"Command: #write {filename}\n"
		"               #write filename\n"
		"\n"
		"Writes all current actions, aliases, subs, highlights, and variables\n"
		"to a command file, specified by filename.\n"
	},
	{
		"ZAP",
		"\n"
		"Command: #zap\n"
		"\n"
		"Kill your current session.  If there is no current session, it will cause\n"
		"the program to terminate.\n"
	},
	{
		"",
		""
	}
};


DO_COMMAND(do_help)
{
	char buf[BUFFER_SIZE] = { 0 }, add[BUFFER_SIZE], *ptf, *pto;
	int cnt;

	if (*arg == 0)
	{
		tintin_header(ses, " HELP LIST ");

		for (cnt = 0 ; *help_table[cnt].name != 0 ; cnt++)
		{
			if (strlen(buf) + 19 > ses->cols)
			{
				tintin_puts2(buf, ses);
				buf[0] = 0;
			}
			sprintf(add, "%19s", help_table[cnt].name);
			strcat(buf, add);
		}
		tintin_puts2(buf, ses);
		tintin_header(ses, "");
	}
	else
	{
		for (cnt = 0 ; *help_table[cnt].name != 0 ; cnt++)
		{
			if (is_abbrev(arg, help_table[cnt].name) || atoi(arg) == cnt + 1)
			{
				substitute(ses, help_table[cnt].text, buf, SUB_COL);

				if (ses != gts)
				{
					tintin_header(ses, " %s ", help_table[cnt].name);
				}

				for (pto = buf ; *pto ; pto = ptf)
				{
					ptf = strchr(pto, '\n');

					if (ptf == NULL)
					{
						break;
					}
					*ptf++ = 0;

					tintin_printf2(ses, "%s", pto);
				}

				if (ses != gts)
				{
					tintin_header(ses, "");
				}
				break;
			}
		}
		if (*help_table[cnt].name == 0)
		{
			tintin_printf2(ses, "No help found for '%s'", arg);
		}
	}
	return ses;
}
