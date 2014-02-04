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
	This help table is a mess, but I got better things to do - Igor
*/

struct help_type help_table[] =
{
	{
		"ACTION",
		"<178>Command<078>: #action <178>{<078>search string<178>}<078> <178>{<078>commands<178>}<078> <178>{<078>priority<178>}<078>\n"
		"\n"
		"         Have tintin search for a certain string of text from the mud, if the\n"
		"         string is found it will execute the commands.  Variables %0 to %9\n"
		"         are substituted from the input string, and can be used in the command.\n"
		"         Brackets and pipes can be used for selective keywords.\n"
		"         The third parameter of the command will tell how important tintin\n"
		"         should reguard this action, 0 most important, 9 least important.  If\n"
		"         no priority is given, a default value of 5 is assigned.\n"
		"\n"
		"         If the search string starts with ~, color codes must be matched.\n"
		"         If the search string starts with ^, the start of the line must match.\n"
		"         If the search string ends with ^, the end of the line must match.\n"
		"\n"
		"<178>Example<078>: #act {^You are hungry^} {get bread backpack;eat bread}\n"
		"         Performs 2 commands as you get the hunger message, tintin will not\n"
		"         be fooled by someone saying 'You are hungry'.\n"
		"\n"
		"<178>Example<078>: #act {%0 tells you 'hello'} {tell %0 Hello %0. How are you today?}\n"
		"         This allows you to seem like a nice, attentive, fast typing gentleman.\n"
		"\n"
		"<178>Example<078>: #act {A puppy barks at you, wagging [his|her|its] tail.} {slap puppy}\n"
		"         This allows you to deal with annoying puppies regardless of their sex.\n"
		"\n"
		"<178>Comment<078>: You can remove an action with the #unaction command.\n"
	},
	{
		"ALIAS",
		"<178>Command<078>: #alias <178>{<078>word<178>} {<078>commands<178>}<078>\n"
		"\n"
		"         Defines a word that actually means a different word. Useful for long\n"
		"         commands repeated often.  You can have multiple commands aliased to\n"
		"         a single word, if you enclose the alias in braces.\n"
		"\n"
		"         If the commands include variables %0-9, these are substituted as\n"
		"         part of the parsing.  %0 is set to all text after the word was input,\n"
		"         %1 is set to the first word following the aliased word, and %2 is the\n"
		"         second, and so on.\n"
		"\n"
		"<178>Example<078>: #alias gb get bread bag\n"
		"         Typing gb at the prompt would be sent as 'get bread bag'.\n"
		"\n"
		"<178>Example<078>: #alias ws {wake;stand}\n"
		"         Since ws aliases a multiple command, you must use the braces.\n"
		"\n"
		"<178>Example<078>: #alias heal cast 'heal' %1\n"
		"         Typing 'heal valgar' would be sent as 'cast 'heal' valgar'.\n"
		"\n"
		"<178>Comment<078>: You can remove an alias with the #unalias command.\n"
	},
	{
		"ALL",
		"<178>Command<078>: #all <178>{<078>string<178>}<078>\n"
		"\n"
		"         Sends a command to all active sessions.\n"
		"\n"
		"<178>Example<078>: #all quit\n"
		"         Sends 'quit' to all active sessions.\n"
	},
	{
		"BELL",
		"<178>Command<078>: #bell\n"
		"\n"
		"         Sends a beep to your terminal.\n"
		"\n"
		"<178>Example<078>: #action {tells you} {#bell}\n"
		"         Will chime every time someone gives you a tell.\n"
	},
	{
		"BUFFER",
		"<178>Command<078>: #buffer <178>{<078>home<178>|<078>up<178>|<078>down<178>|<078>end<178>|<078>find<178>|<078>write filename<178>|<078>info<178>}<078>\n"
		"         The buffer command allows you to add macros to scroll in case the\n"
		"         default bindings do not work. The write option allows you to save\n"
		"         the entire scrollback buffer to file.\n"
		"\n"
		"<178>Example<078>: #macro {(press ctrl-v)(press F1)} {#buffer end}\n"
		"         Associates F1 key to 'scroll buffer to its end' command.\n"
	},
	{
		"CHAT",
		"<178>Command<078>: #chat <178>{<078>option<178>}<078> <178>{<078>argument<178>}<078>\n"
		"\n"
		"         #chat {init}       {port}             Initilizes a chat port.\n"
		"         #chat {name}       {name}             Sets your chat name.\n"
		"         #chat {message}    {buddy|all} {text} Sends a chat message\n"
		"\n"
		"         #chat {accept}     {buddy}            Accept a file transfer\n"
		"         #chat {call}       {address} {port}   Connect to a buddy\n"
		"         #chat {cancel}     {buddy}            Cancel a file transfer\n"
		"         #chat {color}      {color names}      Set the default color\n"
		"         #chat {decline}    {buddy}            Decline a file transfer\n"
		"         #chat {dnd}                           Decline new connections\n"
		"         #chat {download}   {directory}        Set your download directory\n"
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
		"<178>Command<078>: #class <178>{<078>name<178>}<078> <178>{<078>open<178>|<078>close<178>|<078>read filename<178>|<078>write filename<178>|<078>kill<178>}<078>\n"
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
		"\n"
		"<178>Example<078>: #class extra kill;#class extra read extra.tin\n"
		"         Deletes all triggers of 'extra' class if any. Read 'extra.tin' file,\n"
		"         all triggers loaded will be assigned to the fresh new 'extra' class.\n"
	},
	{
		"COLORS",
		"<178>Syntax<078>:  <<888>xyz>  with x, y, z being parameters\n"
		"\n"
		"         Parameter 'x': VT102 code\n"
		"\n"
		"         0 - Reset all colors and codes to default\n"
		"         1 - Bold\n"
		"         2 - Dim\n"
		"         4 - Underscore\n"
		"         5 - Blink\n"
		"         7 - Reverse\n"
		"         8 - Skip (use previous code)\n"
		"\n"
		"         Parameter 'y':  Foreground color\n"
		"         Parameter 'z':  Background color\n"
		"\n"
		"         0 - Black                5 - Magenta\n"
		"         1 - Red                  6 - Cyan\n"
		"         2 - Green                7 - White\n"
		"         3 - Yellow               8 - Skip\n"
		"         4 - Blue                 9 - Default\n"
		"\n"
		"<178>Example<078>: #showme <<888>115>t<<888>828>e<<888>838>s<<888>848>t<<888>088> <<888>468>!<<888>568>!<<888>088>\n"
		"         This will show: <115>t<828>e<838>s<848>t<088> <468>!<568>!<088>.\n"
		"\n"
		"<178>Comment<078>: For xterm 256 colors support use <<888>aaa> to <<888>fff> for RGB foreground\n"
		"         colors and <<888>AAA> to <<888>FFF> for RGB background colors.\n"
	},
	{
		"CONFIG",
		"<178>Command<078>: #config <178>{<078>option<178>}<078> <178>{<078>argument<178>}<078>\n"
		"\n"
		"         This allows you to configure various settings, the settings can be\n"
		"         written to file with the #write or #writesession command.\n"
		"\n"
		"         If you configure the global session (the one you see as you start up\n"
		"         tintin) all sessions started will inherite these settings.\n"
		"\n"
		"         It's advised to make a configuration file to read on startup if you\n"
		"         do not like the default settings.\n"
		"\n"
		"         Config options which aren't listed by default:\n"
		"\n"
		"         #CONFIG {CONVERT META} {ON|OFF} Shows color codes and key bindings.\n"
		"         #CONFIG {COLOR PATCH}  {ON|OFF} Fixes color code usage on some muds.\n"
		"         #CONFIG {DEBUG TELNET} {ON|OFF} Shows telnet negotiations y/n.\n"
		"         #CONFIG {LOG LEVEL}  {LOW|HIGH} LOW logs mud output before triggers\n"
	},
	{
		"CR",
		"<178>Command<078>: #cr\n"
		"\n"
		"         Sends a carriage return to the session.  Useful for aliases that need\n"
		"         extra carriage returns.\n"
	},
	{
		"CURSOR",
		"<178>Command<078>: #cursor <178>{<078>option<178>}<078> <178>{<078>argument<178>}<078>\n"
		"\n"
		"         Typing #cursor without an option will show all available cursor\n"
		"         options. The cursor command's primarly goal is adding customizable\n"
		"         input editing with macros.\n"
	},
	{
		"DEBUG",
		"<178>Command<078>: #debug <178>{<078>listname<178>}<078> <178>{<078>on<178>|<078>off<178>|<078>log<178>}<078>\n"
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
		"DELAY",
		"<178>Command<078>: #delay <178>{<078>seconds<178>}<078> <178>{<078>command<178>}<078>\n"
		"<178>Command<078>: #delay <178>{<078>name<178>}<078> <178>{<078>command<178>}<078> <178>{<078>seconds<178>}<078> \n"
		"\n"
		"         Delay allows you to have tintin wait the given amount of seconds\n"
		"         before executing the given command. tintin won't wait before\n"
		"         executing following input commands if any.\n"
		"\n"
		"         Floating point precision for miliseconds is possible.\n"
		"\n"
		"<178>Example<078>: #showme first;#delay {1} {#showme last}\n"
		"         This will print 'first', and 'last' around one second later.\n"
		"\n"
		"<178>Comment<078>: If you want to remove a delay with the #undelay command you can add\n"
		"         a name as the first argument, be aware this changes the syntax.\n"
	},
	{
		"ECHO",
		"<178>Command<078>: #echo <178>{<078>format<178>}<078> <178>{<078>argument1 argument2 etc<178>}<078>\n"
		"\n"
		"         Echo command displays text on the screen with formatting options.\n"
		"\n"
		"<178>Example<078>: #format {time} {%T}\n"
		"         #echo {The current time and date is: %d %t} {$time $time}\n"
		"         Stores the current epoch value in the $time variable, and echoes\n"
		"         the military date and time based on this value. As I write this it\n"
		"         printed: The current time and date is: 10-03-2004 23:34\n"
	},
	{
		"END",
		"<178>Command<078>: #end\n"
		"\n"
		"         Terminates tintin and return to unix.  On most systems, ctrl-c has\n"
		"         the same result.\n"
	},
	{
		"ESCAPE",
		"         You may use the escape character \\ for various special characters.\n"
		"\n"
		"         \\a   will beep the terminal.\n"
		"         \\e   will start an escape sequence.\n"
		"         \\n   will send a new line.\n"
		"         \\r   will send a carriage return.\n"
		"         \\t   will send a tab.\n"
		"         \\x   will print a hexadecimal value, \xFF for example.\n"
		"         \\x7B will send the '{' character.\n"
		"         \\x7D will send the '}' character.\n"
		"\n"
		"         Ending a line with \\ will stop tintin from appending \\r\\n. To escape\n"
		"         arguments in an alias use %%0 %%1 %%2 etc.\n"
	},
	{
		"EVENT",
		"<178>Command<078>: #event <178>{<078>event type<178>}<078>\n"
		"\n"
		"         Events allow you to create triggers for predetermined client events.\n"
		"         Use #event without an argument to see a list of possible events with\n"
		"         a brief description. Use #event * to see the current list of available\n"
		"         events you can use.\n"
		"\n"
		"<178>Example<078>: #event {session connected} {#read mychar.tin}\n"
		"\n"
		"<178>Comment<078>: You can remove an event with the #unevent command.\n"
	},
	{
		"FORALL",
		"<178>Command<078>: #forall <178>{<078>list<178>}<078> <178>{<078>command<178>}<078>\n"
		"\n"
		"         How this works is best shown with an example.\n"
		"\n"
		"<178>Example<078>: #forall {a b c d} {say &0}\n"
		"         This equals: say a;say b;say c;say d;\n"
		"         Useful for whenever it's useful.\n"
	},
	{
		"FORMAT",
		"<178>Command<078>: #format <178>{<078>variable<178>}<078> <178>{<078>format<178>}<078> <178>{{<078>argument1<178>} {<078>argument2<178>} {<078>etc<178>}}<078>\n"
		"\n"
		"         Allows you to store a string into a variable in the exact same way\n"
		"         C's sprintf works with a few enhancements and limitations such as\n"
		"         no integer operations and a maximum of 20 arguments. If you use format\n"
		"         inside an alias or action you must escape the %0-9 like: %+4s.\n"
		"\n"
		"<178>Example<078>: #format {test} {%+9s} {string}  pad string with up to 9 spaces\n"
		"         #format {test} {%-9s} {string}  post pad string with up to 9 spaces\n"
		"         #format {test} {%.8s} {string}  copy at most 8 characters\n"
		"         #format {test} {%a}   {number}  print corresponding ascii character\n"
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
		"         #format {test} {%w}   {string}  wordwrap text\n"
		"         #format {cols} {%C}         {}  store the screen width in {cols}\n"
		"         #format {test} {%D}     {time}  print the current day\n"
		"         #format {test} {%G}   {number}  perform thousand grouping on {number}\n"
		"         #format {test} {%L}   {string}  store the string length in {test}\n"
		"         #format {test} {%M}     {time}  print the current month\n"
		"         #format {rows} {%R}         {}  store the screen height in {rows}\n"
		"         #format {time} {%T}         {}  store the epoch time in {time}\n"
		"         #format {time} {%U}         {}  store the micro epoch time in {time}\n"
		"         #format {test} {%Y}     {time}  print the current year\n"
	},
	{
		"FUNCTION",
		"<178>Command<078>: #function <178>{<078>name<178>}<078> <178>{<078>operation<178>}<078>\n"
		"\n"
		"         Functions allow you to execute a script within a line of text, and\n"
		"         replace the function call with the line of text generated by the\n"
		"         function.\n"
		"\n"
		"         Be aware that each function should set the $result variable.\n"
		"\n"
		"         To use a function use the @ character before the function name.\n"
		"         The function arguments should be placed between braces behind the\n"
		"         function name.\n"
		"\n"
		"         The function itself can use the provided arguments which are stored\n"
		"         in %1 to %9, with %0 holding all arguments.\n"
		"\n"
		"         Currently you can't use a function from within another function\n"
		"\n"
		"<178>Example<078>: #function {rnd} {#math {result} {1 d (%2 - %1 + 1) + %1 - 1}}\n"
		"         #showme A random number between 100 and 200: @rnd{100 200}\n"
		"\n"
		"<178>Example<078>: #function gettime {#format temp %T;#format result %t $temp}\n"
		"         #showme The current time is @gettime{}\n"
		"\n"
		"<178>Comment<078>: You can remove a function with the #unfunction command.\n"
	},
	{
		"GAG",
		"<178>Command<078>: #gag <178>{<078>string<178>}<078>\n"
		"\n"
		"         Removes any line that contains the string.\n"
		"\n"
		"\n"
		"<178>Comment<078>: You can remove a gag with the #ungag command.\n"
	},
	{
		"GREP",
		"<178>Command<078>: #grep <178>[<078>page<178>]<078> <178>{<078>search string<178>}<078>\n"
		"\n"
		"         This command allows you to search for matching lines in your scroll\n"
		"         back buffer. The amount of matches shown equals your screen size. If\n"
		"         you want to search back further use the optional page number. You can\n"
		"         use wildcards for better search results. By default * *'s are placed\n"
		"         around the search string to make things a little easier. Be aware the\n"
		"         search string is case sensitive.\n"
		"\n"
		"<178>Example<078>: #grep Bubba tells you\n"
		"         This will show all occasions where bubba tells you something.\n"
	},
	{
		"HELP",
		"<178>Command<078>: #help <178>{<078>subject<178>}<078>\n"
		"\n"
		"         Without an argument #help will list all available help subjects.\n"
		"\n"
		"         Using #help * will display all help entries.\n"
	},
	{
		"HIGHLIGHT",
		"<178>Command<078>: #highlight <178>{<078>string<178>}<078> <178>{<078>color names<178>}<078>\n"
		"\n"
		"         The highlight command is used to allow you to highlight strings of text\n"
		"         from the mud.  Available color names are:\n"
		"\n"
		"         reset, light, faint, underscore, blink, reverse, dim,\n"
		"         black, red, green, yellow, blue, magenta, cyan, white,\n"
		"         b black, b red, b green, b yellow, b blue, b magenta, b cyan, b white\n"
		"\n"
		"         The %0-9 variables can be used as 'wildcards' that will match with any\n"
		"         text. They are useful for highlighting a complete line.\n"
		"         You may start the string to highlight with a ^ to only highlight text\n"
		"         if it begins the line.\n"
		"\n"
		"         Besides color names also <abc> color codes can be used.\n"
		"\n"
		"<178>Comment<078>: This command is only compatible with ANSI/VT100 terminals or emulators.\n"
		"\n"
		"\n"
		"<178>Comment<078>: You can remove a gag with the #ungag command.\n"
		"<178>Example<078>: #high {Valgar} {reverse}\n"
		"         Prints every occurrence of 'Valgar' in reverse video.\n"
		"\n"
		"<178>Example<078>: #high {^You %0} {bold cyan}\n"
		"         Boldfaces any line that starts with 'You' in cyan.\n"
		"\n"
		"<178>Example<078>: #high {Bubba} {red underscore blink}\n"
		"         Highlights the name Bubba as blinking, red, underscored text\n"
		"\n"
		"<178>Comment<078>: See '#help substitute', for more advanced color substitution.\n"
		"\n"
		"<178>Comment<078>: This command only works with ANSI/VT100 terminals or emulators.\n"
		"\n"
		"<178>Comment<078>: You can remove a highlight with the #unhighlight command.\n"
	},
	{
		"HISTORY",
		"<178>Command<078>: #history <178>{<078>delete<178>}<078> \n"
		"         #history <178>{<078>insert<178>} {<078>command<178>}<078>\n"
		"         #history <178>{<078>read<178>}   {<078>filename<178>}<078>\n"
		"         #history <178>{<078>write<178>}  {<078>filename<178>}<078>\n"
		"\n"
		"         Either deletes the last command, inserts a new last command,\n"
		"         or saves/loads the history from file.\n"
		"\n"
		"         Without an argument your entire command history is shown.\n"
	},
	{
		"IF",
		"<178>Command<078>: #if <178>{<078>conditional<178>}<078> <178>{<078>commands if true<178>}<078> <178>{<078>commands if false<178>}<078>\n"
		"\n"
		"         The 'if' command is one of the most powerful commands added since\n"
		"         TINTIN III. It works similar to an 'if' statement in other languages,\n"
		"         and is strictly based on the way C handles its conditional statements.\n"
		"         When an 'if' command is encountered, the conditional statement is\n"
		"         evaluated, and if TRUE (any non-zero result) the commands are executed.\n"
		"\n"
		"         The 'if' statement is only evaluated if it is read, so you must nest\n"
		"         the 'if' statement inside another statement (most likely an 'action'\n"
		"         command). The conditional is evaluated exactly the same as in the\n"
		"         'math' command only instead of storing the result, the result is used\n"
		"         to determine whether to execute the commands.\n"
		"\n"
		"<178>Example<078>: #action {%0 gives you %1 gold coins.} {#if {%1>5000} {thank %0}}\n"
		"         If someone gives you more than 5000 coins, thank them.\n"
		"\n"
		"<178>Comment<078>: See '#help math', for more information.\n"
	},
	{
		"IGNORE",
		"<178>Command<078>: #ignore <178>{<078>listname<178>}<078> <178>{<078>on<178>|<078>off<178>}<078>\n"
		"\n"
		"         Toggles a list on or off. With no arguments it shows your current\n"
		"         settings, as well as the list names that you can ignore.\n"
		"\n"
		"         If you for example set ACTIONS to OFF actions will no longer trigger.\n"
		"         Not every list can be ignored.\n"
	},
	{
		"INFO",
		"<178>Command<078>: #info\n"
		"\n"
		"         Displays all the settings of every tintin list available.\n"
		"\n"
		"Trivia:  Typing: #info cpu will show information about tintin's cpu usage.\n"
	},
	{
		"KILL",
		"<178>Command<078>: #kill <178>{<078>list<178>|<078>all<178>}<078>\n"
		"\n"
		"         Without an argument, kill commanddeletes all lists.  Useful so you\n"
		"         don't have to exit tintin to load up a new command file.\n"
		"\n"
		"         With an argument a specific list can be killed.\n"
	},
	{
		"LINE",
		"<178>Command<078>: #line <178>{<078>gag<178>|<078>log<178>}<078> <178>{<078>argument<178>}<078>\n"
		"\n"
		"         #line log {filename} {[text]}  Log the next line or the given text to\n"
		"                                        file.\n"
		"         #line gag                      Gag the next line.\n"
	},
	{
		"LIST",
		"<178>Command<078>: #list <178>{<078>variable<178>}<078> <178>{<078>clr<178>|<078>del<178>|<078>ins<178>|<078>get<178>|<078>len<178>|<078>set<178>|<078>srt<178>}<078> <178>{<078>argument<178>}<078>\n"
		"\n"
		"         #list {list} {clr}                     Empty the given list\n"
		"         #list {list} {del} {index}             Delete an item from the list\n"
		"         #list {list} {ins} {index} {string}    Insert {string} at given index\n"
		"         #list {list} {fnd} {string} {variable} Return index if {string} is\n"
		"                                                found\n"
		"         #list {list} {len} {variable}          Copy list length to {variable}\n"
		"         #list {list} {get} {index} {variable}  Copy an item to {variable}\n"
		"         #list {list} {set} {index} {string}    Change an item at the given\n"
		"                                                index\n"
		"         #list {list} {srt} {string}            Insert item in alphabetic order\n"
		"\n"
		"         The index should be between 1 and the list's length. You can also give\n"
		"         a negative value, in which case -1 equals the last item in the list, -2\n"
		"         the second last, etc.\n"
		"\n"
		"         When inserting an item a positive index will prepend the item at the\n"
		"         given index, while a negative index will append the item.\n"
		"\n"
		"         A length of 0 is returned for an empty or non existant list.\n"
	},

	{
		"LOG",
		"<178>Command<078>: #log <178>{<078>append<178>|<078>overwrite<178>}<078> <178>{<078>filename<178>}<078>\n"
		"\n"
		"         Logs session to a file, you can set the data type with config command.\n"
	},

	{
		"LOOP",
		"<178>Command<078>: #loop <178>{<078>start finish<178>}<078> <178>{<078>commands<178>}<078>\n"
		"         #loop {$variable} {commands}\n"
		"\n"
		"         Like a for statement, loop will loop from start to finish incrementing\n"
		"         or decrementing by 1 each time through.  The value of the loop variable\n"
		"         is placed in &0, which you can use in the commands.\n"
		"\n"
		"         If used with only a start the loop will behave like a while loop. As\n"
		"         long as the variable is non zero the loop will run.\n"
		"\n"
		"<178>Example<078>: #loop {1 3} {get all &0.corpse}\n"
		"         This equals to 'get all 1.corpse;get all 2.corpse;get all 3.corpse'.\n"
		"\n"
		"<178>Example<078>: #loop {3 1} {drop &0.key}\n"
		"         This equals to 'drop 3.key;drop 2.key;drop 1.key'.\n"
	},
	{
		"MACRO",
		"<178>Command<078>: #macro <178>{<078>key sequence<178>}<078> <178>{<078>commands<178>}<078>\n"
		"\n"
		"         Macros allow you to make tintin respond to function keys.\n"
		"\n"
		"         The key sequence send to the terminal when pressing a function key\n"
		"         differs for every OS and terminal. To find out what sequence is send\n"
		"         you can enable the CONVERT META config option.\n"
		"\n"
		"         Another option is pressing ctrl-v, which will enable CONVERT META for\n"
		"         the next key pressed.\n"
		"\n"
		"<178>Example<078>: #macro {(press ctrl-v)(press F1)} {shout tintin is the greatest!\n"
		"         The next time you press F1 the entered command should be executed.\n"
		"\n"
		"<178>Comment<078>: Not all terminals properly initialize the keypad key sequences.\n"
		"         If this is the case you can still use the keypad, but instead of the\n"
		"         arrow keys use ctrl b, f, p, and n.\n"
		"\n"
		"<178>Comment<078>: You can remove a macro with the #unmacro command.\n"
	},
	{
		"MAP",
		"<178>Command<078>: #map\n"
		"\n"
		"         The map command is the backbone of the auto mapping feature. In order\n"
		"         to use mapping for muds using 'ne' 'se' 'sw' 'nw' directions, you need\n"
		"         to create a #pathdir for each direction.\n"
		"\n"
		"         #map color: Sets the map color of the current room you are in.\n"
		"\n"
		"         #map create: Creates a new map and room 1, required to start mapping.\n"
		"\n"
		"         #map destroy: Deletes the map.\n"
		"\n"
		"         #map delete <direction>: Deletes the room in the given direction.\n"
		"\n"
		"         #map dig <direction>: Creates an exit in the given direction. If no\n"
		"                  valid direction is given or no existing room is found a new\n"
		"                  room is created. Useful for portal links and other alternative\n"
		"                  forms of transportation.\n"
		"\n"
		"         #map exit <exit name> <exit commands>: Sets the exit command. This is\n"
		"                  useful if you are dealing with a closed door where you would\n"
		"                  set the exit command with: '#map exit e {open east;e}'.\n"
		"                  Keep in mind the exit command is only used when using\n"
		"                  '#map find' and '#map walk'.\n"
		"\n"
		"         #map explore <direction>: Follows the direction until a dead end or an\n"
		"                  intersection is found. The route is stored in #path and can\n"
		"                  subsequently be used with #walk\n"
		"\n"
		"         #map find <room name>: Searches for the given room name. If found the\n"
		"                  shortest path from your current location to the target\n"
		"                  location is calculated. The route is stored in #path and can\n"
		"                  subsequently be used with #walk.\n"
		"\n"
		"         #map flag static: Will make the map static so new rooms are no longer\n"
		"                  created when walking into an unmapped direction. Useful when\n"
		"                  you're done mapping and regularly bump into walls accidentally\n"
		"                  creating a new room.\n"
		"         #map flag vtmap: Will enable the vtmap which is shown in the top split\n"
		"                  screen if you have one. You can create a 10 rows high top\n"
		"                  screen by using '#split 10 1'.\n"
		"\n"
		"         #map goto <room name>: Takes you to the given room name.\n"
		"\n"
		"         #map get <option> <variable>: Store a map value into a variable.\n"
		"\n"
		"         #map info: Gives information about the map and room you are in.\n"
		"\n"
		"         #map insert: Insert a room in the given direction. Useful for\n"
		"                  inserting void rooms.\n"
		"\n"
		"         #map leave: Makes you leave the map. Useful when entering a maze.\n"
		"\n"
		"         #map legenda <symbols>: The legend exists of 17 decimal numbers which\n"
		"                  represent character symbols used for drawing a map. Binary\n"
		"                  order is used with the n e s w representing bit 1 to 4. The\n"
		"                  first number stands for a no exit room. Number 2 to 16 stand\n"
		"                  for n e ne s ns es nes w nw ew new sw nsw esw nesw. The 17th\n"
		"                  number stands for the room the player is current in.\n"
		"                  The legenda is set by default, but can be adjusted to take\n"
		"                  advantage of fonts with line drawing characters. To check your\n"
		"                  font for drawing characters use:\n"
		"                  #loop {32 255} {#echo {%-3s  %a} {&0 &0}}\n"
		"\n"
		"         #map link <direction> <room name>: Links two rooms. If a valid\n"
		"                  direction is given the link is two way.\n"
		"\n"
		"         #map list [search string]: Lists all room names and exits.\n"
		"\n"
		"         #map map <radius>: shows a map of surrounding rooms.\n"
		"\n"
		"         #map move <direction>: This does the same as an actual movement\n"
		"                  command, updating your location on the map and creating new\n"
		"                  rooms. Useful when you are following someone and want the map\n"
		"                  to follow. You will need to create actions using '#map move',\n"
		"                  for this to work.\n"
		"\n"
		"         #map name <room name>: Allows changing the room name, by default rooms\n"
		"                  have the same name as their internal index number.\n"
		"\n"
		"         #map read <filename>: Will load the given map file.\n"
		"\n"
		"         #map roomflag avoid: When set, '#map find' will avoid a route leading\n"
		"                  through that room. Useful when you want to avoid death traps.\n"
		"         #map roomflag hide: When set, '#map' will not display the map beyond\n"
		"                  this room. When mapping overlapping areas or areas that aren't\n"
		"                  build consistently you need this flag as well to stop\n"
		"                  auto-linking.\n"
		"         #map roomflag leave: When entering a room with this flag, you will\n"
		"                  automatically leave the map. Useful when set at the entrance\n"
		"                  of an unmappable maze.\n"
		"         #map roomflag void: When set the room becomes a spacing room that can\n"
		"                  be used to connect otherwise overlapping areas. A void room\n"
		"                  should only have two exits. When entering a void room you are\n"
		"                  moved to the connecting room until you enter a non void room.\n"
		"\n"
		"         #map set <option> <value>: Set a map value for your current room.\n"
		"\n"
		"         #map travel <direction> <delay>: Follows the direction until a dead end\n"
		"                  or an intersection is found. Use braces around the direction\n"
		"                  if you use the delay, which will add the given delay between\n"
		"                  movements\n"
		"\n"
		"         #map undo: Will undo your last move. If this created a room or a link\n"
		"                  they will be deleted, otherwise you'll simply move back a\n"
		"                  room. Useful if you walked into a non existant direction.\n"
		"\n"
		"         #map unlink <direction> [both]: Will remove the exit, this isn't two\n"
		"                  way so you can have the map properly display no exit rooms and\n"
		"                  mazes.\n"
		"                  If you use the both argument the exit is removed two-ways.\n"
		"\n"
		"         #map walk <room name> <delay>: Calculates the shortest path to the\n"
		"                  destination and walks you there. Use braces around the room\n"
		"                  name if you use the delay, which will add the given delay\n"
		"                  between movements.\n"
		"\n"
		"         #map write <filename>: Will save the map.\n"
	},
	{
		"MATH",
		"<178>Command<078>: #math <178>{<078>variable<178>}<078> <178>{<078>expression<178>}<078>\n"
		"\n"
		"         Performs math functions and stored the result in a variable.  The math\n"
		"         follows a C-like precedence, as follows, with the top of the list\n"
		"         having the highest priority.\n"
		"\n"
		"         Operators       Priority     Function\n"
		"         ------------------------------------------------\n"
		"         !               0            logical not\n"
		"         *               1            integer multiply\n"
		"         /               1            integer divide\n"
		"         %               1            integer modulo\n"
		"         d               1            integer random dice roll\n"
		"         +               2            integer addition\n"
		"         -               2            integer subtraction\n"
		"         >               4            logical greater than\n"
		"         >=              4            logical greater than or equal\n"
		"         <               4            logical less than\n"
		"         <=              4            logical less than or equal\n"
		"         ==              5            logical equal (can use wildcards)\n"
		"         !=              5            logical not equal (can use wildcards)\n"
		"         &&              9            logical and\n"
		"         ^^             10            logical xor\n"
		"         ||             11            logical or\n"
		"\n"
		"         True is any non-zero number, and False is zero.  Parentheses () have\n"
		"         highest precedence, so inside the () is always evaluated first.\n"
		"         Strings must be enclosed in \" \" and are evaluated in alphabetic order.\n"
		"\n"
		"<178>Example<078>: #math {heals} {$mana / 40}\n"
		"         Assuming there is a variable $mana, divides its value by 40 and stores\n"
		"         the result in $heals.\n"
		"\n"
		"<178>Example<078>: #action {^You receive %0 experience} {updatexp %0}\n"
		"         #alias updatexp {#math {xpneed} {$xpneed - %0}\n"
		"         Let's say you have a variable which stores xp needed for your next\n"
		"         level.  The above will modify that variable after every kill, showing\n"
		"         the amount still needed.\n"
		"\n"
		"<178>Example<078>: #action {%0 tells %1}\\\n"
		"           {#if {\"\%0\" == \"Bubba\" && $afk} {reply I'm away, my friend.}}\n"
		"         When you are away from keyboard, it will only reply to your friend.\n"
	},
	{
		"MESSAGE",
		"<178>Command<078>: #message <178>{<078>listname<178>}<078> <178>{<078>on<178>|<078>off<178>}<078>\n"
		"\n"
		"         This will show the message status of all your lists if typed without an\n"
		"         argument. If you set for example VARIABLES to OFF you will no longer be\n"
		"         spammed when correctly using the #VARIABLE and #UNVARIABLE commands.\n"
	},
	{
		"NOP",
		"<178>Command<078>: #nop <178>{<078>whatever<178>}<078>\n"
		"\n"
		"         Short for 'no operation', and is ignored by the client.  It is useful\n"
		"         for commenting in your coms file, any text after the nop and before a\n"
		"         semicolon or end of line is ignored. You shouldn't put braces { } in it\n"
		"         though, unless you close them properly.\n"
		"\n"
		"<178>Comment<078>: By using braces you can comment out multiple lines of code in a script\n"
		"         file.\n"
		"\n"
		"         For commenting out an entire trigger and especially large sections of\n"
		"         triggers you would want to use /* text */\n"
		"\n"
		"<178>Example<078>: #nop This is the start of my script file.\n"
	},
	{
		"PARSE",
		"<178>Command<078>: #parse <178>{<078>string<178>}<078> <178>{<078>commands<178>}<078>\n"
		"\n"
		"         Like a for statement, parse will loop from start to finish through the\n"
		"         given string.  The value of the character parse is at will be placed in\n"
		"         the &0 variable.\n"
	},
	{
		"PATH",
		"<178>Command<078>: #path <178>{<078>del<178>|<078>end<178>|<078>ins<178>|<078>load<178>|<078>map<178>|<078>new<178>|<078>run<178>|<078>save<178>|<078>walk<178>}<078> <178>{<078>argument<178>}<078>\n"
		"\n"
		"         #path del:  Will delete the last move of the path.\n"
		"         #path end:  Movement commands are no longer added to the path.\n"
		"         #path ins:  Add the given argument to the path.\n"
		"         #path load: Load the given alias as the new path.\n"
		"         #path map:  Display the so far created path.\n"
		"         #path new:  Movement commands will be added to the path.\n"
		"         #path run:  Enter all the steps of the current path, with\n"
		"                     an optional given delay.\n"
		"         #path save: Save the created path to an alias. You must specify whether\n"
		"                     you want to save the path forward or backward.\n"
		"         #path walk: Take one step forward of the current path. You can walk\n"
		"                     backwards using #path walk backwards.\n"
		"\n"
		"<178>Example<078>: #path ins {unlock n;open n} {unlock s;open s}\n"
	},
	{
		"PATHDIR",
		"<178>Command<078>: #pathdir <178>{<078>dir<178>}<078> <178>{<078>reversed dir<178>}<078> <178>{<078>coord<178>}<078>\n"
		"\n"
		"         By default tintin sets the most commonly used movement commands\n"
		"         meaning you generally don't really have to bother with pathdirs.\n"
		"         Pathdirs are used by the #path and #map commands.\n"
		"\n"
		"         The first argument is a direction, the second argument is the reversed\n"
		"         direction.  The reverse direction of north is south, etc.\n"
		"\n"
		"         The third argument is a spatial coordinate represented as a bitmap.  In\n"
		"         order to use these check the default values given to n e s w u d by\n"
		"         typing #pathdir without an argument.  If your mud uses the UE (up east)\n"
		"         direction you would add the value of U (16) and E (2) and use that as\n"
		"         the value for the coordinate.\n"
		"\n"
		"<178>Example<078>: #pathdir {ue} {dw} {18}\n"
		"         #pathdir {dw} {ue} {40}\n"
		"\n"
		"<178>Comment<078>: You can remove a pathdir with the #unpathdir command.\n"		
	},
	{
		"PROMPT",
		"<178>Command<078>: #prompt <178>{<078>text<178>}<078> <178>{<078>new text<178>}<078> <178>{<078>row #<178>}<078>\n"
		"\n"
		"         Prompt is a feature for split window mode (see #help split), which\n"
		"         will take the status prompt from the mud, and display it on the status\n"
		"         line of the split window.  You would define <string> the same way you\n"
		"         would a substitution (see #help substitute).  The row number is\n"
		"         optional if you use a non standard split mode.  In which case a\n"
		"         positive number draws #row lines above your cursor, and a negative\n"
		"         number draws #row lines below your topline.\n"
		"\n"
		"<178>Comment<078>: You can remove a prompt with the #unprompt command.\n"
	},
	{
		"READ",
		"<178>Command<078>: #read <178>{<078>filename<178>}<078>\n"
		"\n"
		"         Reads a commands file into memory.  The coms file is merged in with\n"
		"         the currently loaded commands.  Duplicate commands are overwritten.\n"
		"\n"
		"         If you uses braces, { and } you can use several lines for 1 commands.\n"
		"         This however means you must always match every { with a } for the read\n"
		"         command to work.\n"
		"\n"
		"         You can comment out triggers using /* text */\n"
	},
	{
		"REGEXP",
		"<178>Command<078>: #regexp <178>{<078>string<178>}<078> <178>{<078>expression<178>}<078> <178>{<078>true<178>}<078> <178>{<078>false<178>}<078>\n"
		"\n"
		"         Compares the string to the given regular expression.\n"
		"\n"
		"         Variables are stored in &1 to &9 with &0 holding the matched substring.\n"
		"\n"
		"<178>Example<078>: #regexp {bli bla blo} {bli (.*) blo} {#showme &1}\n"
	},
	{
		"REPLACESTRING",
		"<178>Command<078>: #replacestring <178>{<078>variable<178>}<078> <178>{<078>oldtext<178>}<078> <178>{<078>newtext<178>}<078>\n"
		"\n"
		"         Searches the variable text replacing each occurance of 'oldtext' with\n"
		"         'newtext'.\n"
	},
	{
		"RETURN",
		"<178>Command<078>: #return <178>{<078>text<178>}<078>\n"
		"\n"
		"         This command can be used in an #if check to break out of a command\n"
		"         string being executed. In a #function you can use #return with an\n"
		"         argument to both break out of the function and set the result variable.\n"
	},
	{
		"RUN",
		"<178>Command<078>: #run <178>{<078>session name<178>}<078> <178>{<078>shell command<178>}<078>\n"
		"\n"
		"         The run command works much like the system command except that it\n"
		"         runs the command in a pseudo terminal. The run command also creates\n"
		"         a session that treats the given shell command as a mud server. This\n"
		"         allows you to run ssh, as well as any other shell application, with\n"
		"         full tintin scripting capabilities\n"
		"\n"
		"<178>Example<078>: #run {somewhere} {ssh someone@somewhere.com}\n"
		"<178>Example<078>: #run {something} {tail -f chats.log}\n"
	},
	{
		"SCAN",
		"<178>Command<078>: #scan <178>{<078>filename<178>}<078>\n"
		"\n"
		"         The scan command reads in a file and sends its content to the screen\n"
		"         as if it was send by a mud. After using scan you can use page-up and\n"
		"         down to view the file.\n"
		"\n"
		"         This command is useful to convert ansi color files to html or viewing\n"
		"         raw log files.\n"
		"\n"
		"         Reads in a file and sends it to the screen as if it was send by a mud.\n"
		"         This is useful to convert ansi color file to html or viewing log file.\n"
		
	},
	{
		"SCRIPT",
		"<178>Command<078>: #script <178>{<078>shell command<178>}<078>\n"
		"\n"
		"         The script command works much like the system command except that it\n"
		"         treats the generated echos as commands.\n"
		"\n"
		"         This is useful for running php, perl, ruby, and python scripts. You\n"
		"         can run these scrips either from file or from within tintin if the\n"
		"         scripting language allows this.\n"
		"\n"
		"<178>Example<078>: #script {ruby -e 'print \"#showme hello world\"'}\n"
		"<178>Example<078>: #script {python -c 'print \"#showme hello world\"'}\n"
		"<178>Example<078>: #script {php -r 'echo \"#showme hello world\"'}\n"
	},

	{
		"SEND",
		"<178>Command<078>: #send <178>{<078>text<178>}<078>\n"
		"\n"
		"         Sends the text directly to the MUD, useful if you want to start with an\n"
		"         escape code.\n"
	},
	{
		"SESSION",
		"<178>Command<078>: #session <178>{<078>name<178>}<078> <178>{<078>host<178>}<078> <178>{<078>port<178>}<078>\n"
		"\n"
		"         Starts a telnet session with the given name, host and port.  The name\n"
		"         can be anything you want, except the name of an already existant\n"
		"         session or a tintin command.\n"
		"\n"
		"         Without an argument #session shows the currently defined sessions.\n"
		"\n"
		"         If you have more than one session, you can use the following commands:\n"
		"         #session {-|+|number}:  Switches between sessions.\n"
		"         #{name}:  Switches to a session with the given name.\n"
		"         #{name} {command}:  Executes a command with the given session without\n"
		"                             changing the active session.\n"
		"\n"
		"         The startup session is named 'gts' and can be used for relog scripts.\n"
		"\n"
		"<178>Example<078>: #act {^#SESSION '%0' DIED.^} {#gts #delay 10 #ses %0 mymud.com 4321}\n"
	},
	{
		"SHOWME",
		"<178>Command<078>: #showme <178>{<078>string<178>}<078> <178>{<078>row<178>}<078>\n"
		"\n"
		"         Display the string to the terminal, do not send to the mud.  Useful for\n"
		"         status, warnings, etc.  The {row} number is optional and works the same\n"
		"         way as the row number of the #prompt trigger.\n"
		"\n"
		"<178>Example<078>: #action {%0 ultraslays you.}\n"
		"           {#showme {###### ARGH! We were ultraslayed by %0 ######};#gagline}\n"
		"         Emphasis ultraslaying.\n"
	},
	{
		"SNOOP",
		"<178>Command<078>: #snoop <178>{<078>session name<178>}<078>\n"
		"\n"
		"         If there are multiple sessions active, this command allows you to see\n"
		"         what is going on the the sessions that are not currently active.  The\n"
		"         line of text from other sessions will be prefixed by 'session name%'.\n"
	},
	{
		"SPEEDWALK",
		"         <078>Speedwalking allows you to type multiple directions not separated by\n"
		"         semicolons, and now it lets you prefix a direction with a number, to\n"
		"         signify how many times to go that direction. You can turn it on/off\n"
		"         with #config.\n"
		"\n"
		"<178>Example<078>: Without speedwalk, you have to type:\n"
		"         s;s;w;w;w;w;w;s;s;s;w;w;w;n;n;w\n"
		"         With speedwalk, you only have to type:\n"
		"         2s5w3s3w2nw\n"
	},
	{
		"SPLIT",
		"<178>Command<078>: #split <178>{<078>top border height<178>}<078> <178>{<078>bottom border height<178>}<078>\n"
		"\n"
		"         This option only works with a VT102 emulator, this allows you to set up\n"
		"         a screen.  The keyboard input will be displayed in the bottom line,\n"
		"         while mud text is displayed in the main window.  This requires a fairly\n"
		"         decent emulator, but works on most I have tested.\n"
		"\n"
		"         | Top border   (can be ommited, height customizable)\n"
		"         | Mud text\n"
		"         | Bottom border   (height customizable, at least one line)\n"
		"         | Input line\n"
		"\n"
		"         If used without an argument, the top border will be ommited, and the\n"
		"         bottom border's height will be set to one line.\n"
		"\n"
		"         If you want a different split setting, because you want a two lines\n"
		"         thick split, or like having a blank line at the top of the screen\n"
		"         to display something you can set the top and bottom line with the\n"
		"         additional arguments.\n"
		"\n"
		"<178>Example<078>: #split\n"
		"         If tintin has determined that you have a screen of 30 rows, it will\n"
		"         set the main window from line #1 to line #28.\n"
		"\n"
		"<178>Comment<078>: You can display text on the split line(s) with the #prompt and\n"
		"         #showme {line} {row} command.\n"
		"\n"
		"<178>Comment<078>: You can remove split mode with the #unsplit command.\n"
	},
	{
		"SUBSTITUTE",
		"<178>Command<078>: #substitute <178>{<078>text<178>}<078> <178>{<078>new text<178>}<078>\n"
		"\n"
		"         Allows you to replace original text from the mud with different text,\n"
		"         or delete it altogether.  This is helpful for if you have a slow modem,\n"
		"         or there is a lot of text sent during battles, and the like.  The %0-9\n"
		"         variables can be used to capture text and use it as part of the new\n"
		"         output, and the ^ char is valid to only check the beginning of the line\n"
		"         for the text specified.\n"
		"\n"
		"         If a . is the only character in the new text argument, if the line is\n"
		"         matched, it will be deleted.\n"
		"\n"
		"         If only one argument is given, all active substitutions that match the\n"
		"         strings are displayed.  The '*' char is valid in this instance.  See\n"
		"         '#help wildcard', for advanced wildcard information.\n"
		"\n"
		"         If no argument is given, all subs are displayed.\n"
		"\n"
		"<178>Example<078>: #sub {leaves} {.}\n"
		"         Gags any line that has the word 'leaves' in it.\n"
		"\n"
		"<178>Example<078>: #sub {^Zoe%0} {ZOE%0}\n"
		"         Any line that starts with the name Zoe will be replaced by a line that\n"
		"         starts with 'ZOE'.\n"
		"\n"
		"<178>Example<078>: #sub {%0massacres%1} {<<888>018>%0<<888>118>MASSACRES<<888>018>%1}\n"
		"         Replaces all occurrences of 'massacres' with 'MASSACRES' in red.\n"
		"\n"
		"<178>Comment<078>: See '#help colors', for more information.\n"
		"\n"
		"<178>Comment<078>: You can remove a substitution with the #unsubstitute command.\n"
	},
	{
		"SUSPEND",
		"<178>Command<078>: #suspend\n"
		"\n"
		"         Temporarily suspends tintin and returns you to your shell.  The\n"
		"         effect of this command is exactly as if you had typed control-z.\n"
		"         To return to tintin, type 'fg' at the shell prompt.\n"
		"\n"
		"         While suspended your tintin sessions will freeze. To keep a\n"
		"         suspended session running use the screen utility program.\n"
	},
	{
		"SYSTEM",
		"<178>Command<078>: #system <178>{<078>command<178>}<078>\n"
		"\n"
		"         Executes the command specified as a shell command.\n"
	},
	{
		"TAB",
		"<178>Command<078>: #tab <178>{<078>word<178>}<078>\n"
		"\n"
		"         Adds a word to the tab completion list, alphabetically sorted.\n"
		"\n"
		"<178>Comment<078>: You can remove a tab with the #untab command.\n"
	},
	{
		"TEXTIN",
		"<178>Command<078>: #textin <178>{<078>filename<178>}<078>\n"
		"\n"
		"         Textin now allows the user to read in a file, and send its contents\n"
		"         directly to the mud.  Useful for doing online creation, or message\n"
		"         writing.\n"
	},
	{
		"TICKER",
		"<178>Command<078>: #ticker <178>{<078>name<178>}<078> <178>{<078>commands<178>}<078> <178>{<078>interval in seconds<178>}<078>\n"
		"\n"
		"         Executes given command every # of seconds.\n"
		"\n"
		"<178>Comment<078>: You can remove a ticker with the #unticker command.\n"
	},
	{
		"VARIABLE",
		"<178>Command<078>: #variable <178>{<078>variable name<178>}<078> <178>{<078>text to fill variable<178>}<078>\n"
		"\n"
		"         Variables differ from the %0-9 in the fact that you could specify\n"
		"         a full word as a variable name, and they stay in memory for the\n"
		"         full session unless they are changed.  They can be saved in the\n"
		"         coms file, and can be set to different values if you have two or\n"
		"         more sessions running at the same time.  One of the best uses for\n"
		"         variables I think is for spellcasters.\n"
		"\n"
		"<178>Example<078>: #alias {target} {#var target %0}\n"
		"         #alias {x}      {flame $target}\n"
		"\n"
		"         The name of a variable must exist of only letters and numbers in\n"
		"         order to be substituted.  If you do not meet these requirements do\n"
		"         not panic, simply encapsulate the variable in braces:\n"
		"\n"
		"         #variable {cool website} {http://tintin.sourceforge.net}\n"
		"         #chat I was on ${cool website} yesterday!.\n"
		"\n"
		"<178>Comment<078>: In order to check if a variable exists you can use regexp:\n"
		"         #if {\"$target\" == \"?target\"} {#showme variable is not defined}\n"
		"\n"
		"<178>Comment<078>: You can remove a variable with the #unvariable command.\n"
	},
	{
		"WILDCARD",
		"<178>Command<078>: #trigger <178>{<078>regexp<178>}<078>\n"
		"\n"
		"         You may use wildcards with certain commands such as #alias, #action,\n"
		"         #substitute, #unalias, etc.  In commands like #alias, wildcards are\n"
		"         only valid when you use exactly one argument.  Wildcards are always\n"
		"         valid for commands like #unalias.  The only wildcard currently\n"
		"         supported is *, which matches any string 0 or more characters long.\n"
		"         The wildcard meaning of * may be escaped using the backslash '\\'.\n"
		"\n"
		"<178>Example<078>: #action {*miss*}\n"
		"         Shows all actions which contain the word miss in them.\n"
		"\n"
		"<178>Example<078>: #unaction {*miss*}\n"
		"         Removes all actions which contain the word miss in themS\n"
		"\n"
		"<178>Example<078>: #unaction {\\*\\*\\* PRESS RETURN:}\n"
		"         Removes the action which triggers on the line '*** PRESS RETURN':\n"
	},
	{
		"WRITE",
		"<178>Command<078>: #write <178>{<078>filename<178>}<078>\n"
		"\n"
		"         Writes all current actions, aliases, subs, highlights, and variables\n"
		"         to a command file, specified by filename.\n"
	},
	{
		"ZAP",
		"<178>Command<078>: #zap\n"
		"\n"
		"         Kill your current session.  If there is no current session, it will\n"
		"         cause the program to terminate.\n"
	},
	{
		"",
		""
	}
};


DO_COMMAND(do_help)
{
	char left[BUFFER_SIZE], add[BUFFER_SIZE], buf[BUFFER_SIZE], *ptf, *pto;
	int cnt, found;

	arg = get_arg_in_braces(arg, left, TRUE);

	if (*left == 0)
	{
		tintin_header(ses, " HELP LIST ");

		for (cnt = add[0] = 0 ; *help_table[cnt].name != 0 ; cnt++)
		{
			if (strlen(add) + 19 > ses->cols)
			{
				tintin_puts2(ses, add);
				add[0] = 0;
			}
			cat_sprintf(add, "%19s", help_table[cnt].name);
		}
		tintin_puts2(ses, add);

		tintin_header(ses, "");
	}
	else
	{
		found = FALSE;

		for (cnt = 0 ; *help_table[cnt].name != 0 ; cnt++)
		{
			if (is_abbrev(left, help_table[cnt].name) || atoi(left) == cnt + 1 || match(ses, help_table[cnt].name, left))
			{
				substitute(ses, help_table[cnt].text, buf, SUB_COL);

				tintin_header(ses, " %s ", help_table[cnt].name);

				tintin_puts2(ses, "");

				pto = buf;

				while (*pto)
				{
					ptf = strchr(pto, '\n');

					if (ptf == NULL)
					{
						break;
					}
					*ptf++ = 0;

					tintin_puts3(ses, pto);

					pto = ptf;
				}
				tintin_puts2(ses, "");

				found = TRUE;

				if (is_abbrev(left, help_table[cnt].name))
				{
					break;
				}
			}
		}
		if (found == FALSE)
		{
			tintin_printf2(ses, "No help found for '%s'", left);
		}
	}
	return ses;
}
