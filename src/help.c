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
*   file: help.c - funtions related to the internal help file                 *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                  original help files by various persons                     *
*                   recoded by Igor van den Hoven 2004                        *
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

const struct help_type help_table[81] =
{
	{
		"ALIAS",
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
		"Command: #action {search string} {commands} {priority}\n"
		"\n"
		"         Have tintin search for a certain string of text from the mud, if the\n"
		"         string is found it will execute the commands.  Variables %0 to %9\n"
		"         are substituted from the input string, and can be used in the command.\n"
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
	},
	{
		"ALL",
		"Command: #all {string}\n"
		"\n"
		"Send a command to all active sessions.\n"
		"\n"
		"Example: #all quit\n"
	},
	{
		"ANTISUBSTITUTE",
		"Command: #antisubstitute {string}\n"
		"\n"
		"         Will cause any line that contains the string specified to not be\n"
		"         checked for substitutes and gags.  The string search works the same\n"
		"         way as the #action command.\n"
		"\n"
		"Example: #antisub {^You}\n"
		"         any line starting with 'You' will not be checked for substitutes.\n"
	},
	{
		"BELL",
		"Command: #bell\n"
		"\n"
		"         Sends a beep to your terminal.\n"
		"\n"
		"Example: #action {tells you} {#bell}\n"
		"         Will chime every time someone gives you a tell.\n"
	},
	{
		"BOSS",
		"Command: #boss\n"
		"\n"
		"         Puts some fake text on your screen so nobody knows you're mudding.\n"
		"         No new text will appear untill you press a button.\n"
		"\n"
		"Example: #macro {\\e[11~} {#boss}\n"
		"         Will make you go into boss mode whenever you hit F1\n"
	},
	{
		"BUFFER",
		"Command: #buffer {home|up|down|end|write filename|info}\n"
		"         The buffer command allows you to add macros to scroll in case the\n"
		"         default bindings do not work. The write option allows you to save\n"
		"         the entire scrollback buffer to file.\n"
	},
	{
		"CLASS",
		"Command: #class {name} {open|close|read filename|write filename|kill}\n"
		"\n"
		"         The {open} option will open a class, all triggers added afterwards\n"
		"         will be assigned to that class.\n"
		"         The {close} option will close the given class and make the last\n"
		"         accessed open class active.\n"
		"         The {read} option will only read triggers of the given class from file.\n"
		"         the class when finished.\n"
		"         The {write} option will write all triggers of the given class to file.\n"
		"         The {kill} option will delete all triggers of the given class.\n"
	},
	{
		"COLORS",
		"Format:  <abc>  with a, b, c being parameters\n"
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
		"Example: #showme <<888>115>t<<888>828>e<<888>838>s<<888>848>t<<888>088> <<888>468>!<<888>568>!<<888>088>\n"
		"         This will show: <115>t<828>e<838>s<848>t<088> <468>!<568>!<088>\n"
	},
	{
		"CONFIGURE",
		"Commands: #configure {option} {argument}\n"
		"\n"
		"          This allows you to configure various settings, the settings can be\n"
		"          written to file with the #write or #writesession command.\n"
		"\n"
		"          If you configure the global session (the one you see as you start up\n"
		"          tintin) all sessions started will inherite these settings.\n"
		"\n"
		"          It's advised to make a configuration file to read on startup if you\n"
		"          do not like the default settings.\n"
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
		"<068>    ######################################################################\n"
		"<068>    #<078>                           T I N T I N + +                          <068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<078>            (<068>T<078>)he k(<068>I<078>)cki(<068>N<078>) (<068>T<078>)ickin d(<068>I<078>)kumud clie(<068>N<078>)t            <068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<078>                 Original TINTIN code by Peter Unold                <068>#\n"
		"<068>    #<078>       new code by Bill Reiss, David A. Wagner, Rob Ellsworth,      <068>#\n"
		"<068>    #<078>                 Jeremy C. Jack, Igor van den Hoven                 <068>#\n"
		"<068>    #<078>                              1994,2005                             <068>#\n"
		"<068>    #<078>                                                                    <068>#\n"
		"<068>    #<078>                           Version 1.94.5                           <068>#\n"
		"<068>    ######################################################################<088>\n"
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
		"         You might want to escape the %'s when using echo in an action or alias\n"
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
		"Command: #highlight {string} {color codes}\n"
		"\n"
		"         The highlight command is used to allow you to highlight strings of text\n"
		"         from the mud.  Available color codes are:\n"
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
		"Command: #debug {listname} {on|off}\n"
		"\n"
		"         Toggles a list on or off. With no argument it shows your current\n"
		"         settings, as well as the list names that you can debug.\n"
		"\n"
		"         If you for example set ACTIONS to ON you will get debug information\n"
		"         whenever an action is triggered.\n"
		"\n"
		"         Not every list has debug support yet.\n"
	},
	{
		"FORALL",
		"Command: #forall {list} {command}\n"
		"         How this works is best shown with an example\n"
		"Example: #forall {a b c d} {say $forall}\n"
		"         This equals: say a;say b;say c;say d;\n"
		"         Useful for whenever it's useful.\n"
	},
	{
		"FORMAT",
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
		"         #format {test} {%r}   {string}  reverse text, hiya = ayih\n"
		"         #format {test} {%t}     {time}  print the current military time\n"
		"         #format {test} {%u}   {string}  uppercase text\n"
		"         #format {time} {%T}             store the epoch time in {time}\n"
		"         #format {time} {%U}             store the micro epoch time in {time}\n"
		"         #format {test} {%C}   {colums}  store the screen width in {colums}\n"
		"         #format {test} {%G}   {number}  perform thousand grouping on {number}\n"
		"         #format {test} {%L}   {string}  store the string length in {test}\n"
		"         #format {test} {%R}     {rows}  store the screen height in {rows}\n"
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
		"GETITEMNR",
		"\n"
		"Command: getitemnr - retrieves specified element from a list (NEW)\n"
		"\n"
		"Syntax:  #getitemnr {destination variable} {item number} {list}\n"
		"\n"
		"Example: #geti {dothis} {2} {smile {say Hi!} flip bounce}  <--- sets dothis to 'say Hi!'\n"
		"\n"
		"Example: #getl Length {$rndsocial};#math itemnr {1 d $Length};#geti temp {$itemnr}\n"
		"{$rndsocial};{$temp}\n"
		"         (will perform a random social from the 'rndsocial' list)\n"
	},
	{
		"GETLISTLENGTH",
		"\n"
		"Command: getlistlength - returns the length of a list\n"
		"\n"
		"Syntax:  #getlistlength {dest var} {list}\n"
		"\n"
		"Example: #getl Length {$alignlist}   <--- returns 3 in the Length variable\n"
	},
	{
		"GREP",
		"Command: #grep [page] {search string}\n"
		"         This command allows you to search for matching lines in your scroll\n"
		"         back buffer. The amount of matches shown equals your screen size. If you\n"
		"         want to search back further use a page number. You can use wildcards\n"
		"         for better search results. Default * *'s are placed around the search\n"
		"         string to make life easier.\n"
	},
	{
		"INFO",
		"\n"
		"Command: #info\n"
		"\n"
		"         Displays all the settings of every tintin list available.\n"
	},
	{
		"KILLALL",
		"\n"
		"Command: #killall\n"
		"\n"
		"         Killall deletes all lists.  Useful so you don't have to exit tintin++\n"
		"         to load up a new command file.\n"
	},
	{
		"LOADPATH",
		"Command: #loadpath {alias name}\n"
		"\n"
		"Load a saved path back into the path, useful for the #walk command.\n"
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
		"\n"
		"Like a for statement, loop will loop from start to finish incrementing or\n"
		"decrementing by 1 each time through.  The value of the loop variable is\n"
		"placed in $loop, which you can use in the commands.\n"
		"\n"
		"examples:\n"
		"#loop {1 3} {get all $loop.corpse}\n"
		"equivalent to the following:\n"
		"  get all 1.corpse;get all 2.corpse;get all 3.corpse\n"
		"#loop {3,1} {drop &0.key}\n"
		"equivalent to\n"
		"  drop 3.key;drop 2.key;drop 1.key\n"
	},
	{
		"MACRO",
		"Command: #macro {key sequence} {commands}\n"
		"         This allows you to make tintin respond to a function key.\n"
		"\n"
		"         The key sequence send to the terminal when pressing a function key\n"
		"         differs for every OS and terminal. To find out what sequence is send\n"
		"         you can enable the CONVERT META config option.\n"
		"\n"
		"Example: Type: #config convert on, type #macro {, press the F1 key and the\n"
		"         key sequence will be filled in. Finish the macro and type: #config\n"
		"         convert off, when pressing F1 the entered command should be executed.\n"
	},
	{
		"MAP",
		"\n"
		"Command: #map\n"
		"\n"
		"Will show your current path. See help path.\n"
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
		"#act {%0 tells %1} {#if {\"%0\" == \"Scandum\" && $afk} {reply $afk Sir} {reply $afk dude}}\n"
		"Compare the string and reply appropriately if $afk has been set to\n"
		"'Watching britney spears on MTV' or something of that nature =]\n"
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
		"Short for 'no operation', and is ignored by the client.  It is useful for commenting\n"
		"in your coms file, any text after the nop and before a semicolon or\n"
		"end of line is ignored. You shouldn't put braces { } in it though, unless\n"
		"you close them properly.\n"
		"\n"
		"example:\n"
		"#nop This is the start of my autoactions\n"
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
		"         optional if you use a non standard split mode. In which case a positive\n"
		"         number draws #row lines above your cursor, and a negative number draws\n"
		"         #row lines above your topline\n"
	},
	{
		"READ",
		"Command: #read {filename}\n"
		"\n"
		"Reads a coms file into memory.  The coms file is merged in with the\n"
		"currently loaded commands.  Duplicate commands are overwritten.\n"
	},
	{
		"REPLACESTRING",
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
		"SESSION",
		"\n"
		"Command: #session {name} {host} {port}\n"
		"\n"
		"Starts a telnet session with the given name, host, and port. The name can\n"
		"be anything you want, except the name of an already existant session.\n"
		"\n"
		"Without an argument #session shows the currently active session.\n"
	},

	{
		"SHOWME",
		"\n"
		"Command: #showme {string}\n"
		"\n"
		"Display the string to the terminal, do not send to the mud.  Useful for\n"
		"status, warnings, etc.\n"
		"\n"
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
		"Command: #split {top line} {bottom line}\n"
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
		"#sub {%0massacres%1} {<018>%0<118>MASSACRES<018>%1}\n"
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
		"UNANTISUBSTITUTE",
		"\n"
		"Command: #unantisubstitute {string}\n"
		"               #unantisubstitute string\n"
		"\n"
		"Remove antisub(s) from the antisub list which match {string}.  The\n"
		"'*' character will act as a wildcard and will match any text.\n"
		"See help wildcard.\n"
		"\n"
		"examples:\n"
		"#unantisub {%0tells you%1}\n"
		"remove the antisub for '%0tells you%1'\n"
		"#unantisub *\n"
		"remove all antisubs\n"
		"#unantisub {^You*}\n"
		"remove all antisubs that start with '^You'\n"
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
		"Command: #unmacro {key combination}\n"
		"         This command removes a macro from the list, but doesn't undo the\n"
		"         actual macro because readline doesn't allow it.\n"
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
		"UNRPROMPT",
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
		"Since these are completely new to tintin, and act differently\n"
		"than anything else, I feel should spend some time on them.\n"
		"These variables differ from the %0-9 in the fact that you could\n"
		"specify a full word as a variable name, and they stay in memory for\n"
		"the full session, unless they are changed, and they can be saved in\n"
		"the coms file, and can be set to different values if you have 2 or\n"
		"more sessions running at the same time.  One of the best uses for\n"
		"variables I think is for spellcasters.\n"
		"\n"
		"Currently, you would set up a bunch of aliases like the following.\n"
		"\n"
		"#alias {flame} {cast 'flame strike' %0}\n"
		"#alias {flash} {cast 'call lightning' %0}\n"
		"#alias {harm} {cast 'harm' %0}\n"
		"\n"
		"With the new variables you can do the following:\n"
		"\n"
		"#alias {targ} {#var target %0}\n"
		"#alias {flamet} {flame $target}\n"
		"#alias {flasht} {flash $target}\n"
		"#alias {harmt} {harm $target}\n"
		"\n"
		"these aliases will be  defined just as they are written, the variables\n"
		"are not substituted  for until the  alias is found  in your input  and\n"
		"executed.\n"
		"\n"
		"so, if before a battle, you do a:\n"
		"targ donjonkeeper\n"
		"then $target is set to donjonkeeper, and any alias you set up with\n"
		"$target in it will substitute donjonkeeper for every instance of\n"
		"$target.  Let's say your leader has the following alias set up.\n"
		"\n"
		"#alias {setttarg} {#var {target} {%0};gt target=%0}\n"
		"\n"
		"if he did a settarg lich, it would set his $target to lich, and\n"
		"would send a:\n"
		"\n"
		"<name> tells your group 'target=lich'\n"
		"\n"
		"you could then set an action like so.\n"
		"\n"
		"#action {^%0 tells your group 'target=%1'} {targ %1}\n"
		"\n"
		"then when your leader executed his alias, it would also set your\n"
		"variable to the target.\n"
		"\n"
		"Another use for variables would be to set a variable $buffer to\n"
		"whoever the current buffer is.  This would be useful in fights\n"
		"where the mob switches, or where many rescues are needed.  You\n"
		"could set up healing aliases with $buffer in them, and set the\n"
		"$buffer variable in an action that is triggered by each switch,\n"
		"and each rescue, or just rescues, or whatever.  Then in a\n"
		"confusing battle, you will have a better shot at healing the\n"
		"right person.\n"
	},
	{
		"WALK",
		"Command: #walk\n"
		"         Walks one step of a loaded path.\n"
		"Example: #mark;w;w;w;#savepath backward test;#loadpath test;#3 #walk\n"
		"         Will make you start mapping, map 3 rooms, save the path backwards,\n"
		"         And make you walk back the last 3 steps. Everytime you #walk, the\n"
		"         first element of the loaded path is deleted and executed. You get\n"
		"         a message when the path is emptied.\n"
	},
	{
		"WILDCARD",
		"\n"
		"format:		#command {regexp}\n"
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
		"Writes all current actions, aliases, subs, antisubs, highlights, and\n"
		"variables to a coms file, specified by filename.\n"
	},
	{
		"WRITESESSION",
		"\n"
		"Command: #writesession {filename}\n"
		"               #writesession filename\n"
		"\n"
		"Write all current actions, aliases, subs, antisubs, highlights, and\n"
		"variables that are specific to your current session to a file.  This\n"
		"means actions that were not defined when there was no session active.\n"
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
	char buf[BUFFER_SIZE] = { 0 }, add[BUFFER_SIZE];
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
		for (cnt = 0 ; help_table[cnt].name != 0 ; cnt++)
		{
			if (is_abbrev(arg, help_table[cnt].name))
			{
				substitute(ses, help_table[cnt].text, buf, SUB_COL);

				if (ses != gts)
				{
					tintin_header(ses, " %s ", help_table[cnt].name);
				}
				tintin_printf2(ses, "%s", buf);

				if (ses != gts)
				{
					tintin_header(ses, "");
				}
				break;
			}
		}
		if (help_table[cnt].name == 0)
		{
			tintin_printf2(ses, "No help found for '%s'", arg);
		}
	}
	return ses;
}
