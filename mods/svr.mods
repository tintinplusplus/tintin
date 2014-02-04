List of modifications done by Happy Iny.

*** New and modified commands:

Command: loop - added evaluation of the parameters (MODIFIED)
Syntax:  #loop {a,b} {do this}
Example: The update means that the following line now works
           #loop {$a,$b} {#shomwe %0}
         provided 'a' and 'b' are variables which contain numbers.

Command: ifexists - checks if a variable exists or not (NEW)
Syntax:  #ifexists {varname} {do this} else {do this instead}
Example: #ife {leader} {say My leader is $leader!} else {say I have no leader!}

Command: ifmatch - checks a string for a mask containing text and *'s (NEW)
Syntax:  #ifmatch {mask} {string} {do this} else {do this instead}
Example: #ifm {*Iny*} {This is a test string: Iny!} {#shomwe Works fine!} else {#showme Call 911 - the procedure
         doesn't work!}

Command: ifstrequal - string comparisment (NEW)
Syntax:  #ifstrequal {string1} {string2} {do this} else {do this instead}
Example: #alias {tell} {#ifs {$1} {$whoami} {#showme [Tell to self]: %0} else {Tell %0}}

Command: getvarvalue - get the contents of a variable (NEW)
Syntax:  #getvarvalue {dest var} {source var}
Example: #getv {numberofmobs} {mobThe Spirit Guard}
Info:    The variable name can contain any character; not just alpha chars!
         This function was made more or less redundant when I introduced the
         improved naming of variables by allowing brackets around the name.
         ( ${variablename} ). But it's kept for compability with earlier
	 comfiles.


Command: revstring - reverses a string/stores it backwards (NEW)
Syntax:  #revstring {dest var} {string}
Example: #revs myname Happy Iny <-- stores 'ynI yppaH' in the variable 'myname'

Command: postpad - pads a variable with spaces AFTER given text until variable
                   is of desired length (truncates if too long)  (NEW)
Syntax:  #postpad {dest var} {length} {text}
Example: #postpad mobname 15 A huge, ancient tree, walking on giant roots,
         #postpad myname 15 Iny

Command: postpad - pads a variable with spaces BEFORE given text until variable
                   is of desired length (truncates if too long)  (NEW) 
Syntax: #prepad {dest var} {length} {text}
Syntax: #prepad mobname 15 A huge, ancient tree, walking on giant roots,
        #prepad myname 15 Iny

*** Commands which can be used to simulate lists/arrays:


I've defined a list the following way:
  1. is a textstring
  2. contains any number of elements
  3. each element is either ONE word OR enclosed in a set of brackets

Examples:
#var alignlist {evil neutral good}   <--- creates a list with 3 elements: 1. evil, 2. neutral, 3. good
#var rndsocial {smile {say WHEE!} bounce}  <---- creates a list with 3 elements: 1. smile, 2. say WHEE!, 3. bounce

The following commands have been created to allow manipulations of lists:

Command: getlistlength - returns the length of a list (NEW)
Syntax:  #getlistlength {dest var} {list}
Example: #getl Length {$alignlist}   <--- returns 3 in the Length variable

Command: getitemnr - retrieves specified element from a list (NEW)
Syntax:  #getitemnr {destination variable} {item number} {list}
Example: #geti {dothis} {2} {smile {say Hi!} flip bounce}  <--- sets dothis to 'say Hi!'
Example: #getl Length {$rndsocial};#ran itemnr {1,$Length};#geti temp {$itemnr} {$rndsocial};{$temp}
         (will perform a random social from the 'rndsocial' list)


*** Added features:

IMPROVED VARIABLE NAMING:
Added the possibility to add non-alpha characters (numbers,space,etc.) in variable names.
(Which kind of makes the #getvarvalue command redundant, but it's kept for compatibility).
Usage will hopefully be demonstrated by this example:
#var {spellcost holy sphere} {150}
#ali {holy} {#if {$manaleft < ${spellcost holy sphere}} {say Don't have enough mana to cast holy!}
     else {cas 'holy sphere'}}

OR this more complex one:

----->8---- cut here ---->8-------
#nop GENERAL STUFF
#ali {===}  {#nop}
#ali {clv}  {#var %0 0}
#ali {incv} {#math {%%0} {$%%0+1}}

=== GENERAL LOG THINGIE ===
#var logname Default; === Set the default logname
#var {GETLOGSIZE}   {#ifex {$logname} {#var logsize ${$logname}} else {clv logsize;#var {$$logname} 0}}
#var {SHOWLOGTITLE} {#showme {== LOG: $logname ==  SIZE: $logsize ==}}
#var {SHOWCONTENTS} {#loop {1,$$logsize} {#var logmsg ${$logname %%0};#showme {$logmsg}};#unvar logmsg}

#ali {writelog}     {$$GETLOGSIZE;incv logsize;{#var {$$logname} {$$logsize}};{#var {$$logname $$logsize}
                    {%%%0}}}

#ali {showlog}      {$$GETLOGSIZE;$$SHOWLOGTITLE;#if {$logsize>0} {$$$SHOWCONTENTS}}
#ali {clearlog}     {#unvar {$$logname*};MSG The log "$logname" has been cleared !}

=== MOB COUNTER THINGIE ===
#act {%0 is dead! R.I.P.} {#var ripname %0}

#act {^You receive %1 experience points.} {#ifex {mob $$ripname} {#var MobCount ${mob $ripname};incv MobCount;
     {#var {mob $$ripname} $MobCount}} else {#var {mob $$ripname} 1;#var logname Mobs;writelog $ripname}}

#var {SHOWMOBS} {#loop {1,$$logsize} {#var logmsg ${$logname %%0};
     {#showme {$logmsg - RIPS: ${mob $logmsg}}}};#unvar logmsg}

#ali {showmobs} {#var logname Mobs;$$GETLOGSIZE;#if {$logsize>0} {$$$SHOWMOBS}}
#ali {clearmoblog} {#var logname Mobs;clearlog;#unvar mob *}

=== Clear temporary variables ===
#unvar GETLOGSIZE;#unvar SHOWLOGTITLE;#unvar SHOWCONTENTS
#unvar SHOWMOBS

--->8--- cut here --->8----

Example which uses the commands above:
Let's say the text inside the ---cut--- lines have been loaded and that the following lines
has been received on the mud:
  The Spirit Guard is dead! R.I.P.                       ] occured 3 times
  You receive your share of 354325 experience points.    ]

  The Spirit Sergeant is dead! R.I.P.                    ] occured 2 times
  You receive your share of 345552 experience points.    ]

Using the alias 'showmobs' will then display the following list
(providing the alias 'clearmoblog' was run first):
---
The Spirit Guard - RIPS: 3
The Spirit Sergeant - RIPS: 2
---

TICKCOUNTER:
============
A variable containing the number of seconds to tick from tintin's internal tickcounter
has been implemented.
NB! The variable is only SIMULATED which means it won't be shown in the #variable list.
Example: say There's approximately $secstotick seconds again to the tick.

I also removed the annoying TICK messages from tintin... which means the tickcounter will
run silently.


FUNCTIONS:
==========
Added a new list type which I decided to call functions. These are a mixture of aliases and
variables. The following examples will have to do to explain how to use functions:

Example:
#function {rnd} {#random {temp} {%1,%2};#result {$temp};#unvar temp}
#showme A random number between 0 and 100: @rnd{0 100}

Functions are treated much like variables. Their value is
a command-line which is executed, and the functions are substituted
by the parameter last sent to the #result command

Example:
#function {lastfuncresult} {#nop}
#showme Last use of a function gave @lastfuncresult as result.

NB! Functions are NOT evaluated at all the same places as variables so before
you decide to write a long comfile using functions - check if it works first! :)
I might finish my work on the functions later, right now I'm too lazy to though.

=== FUNCTIONS ARE FOR INSTANCE _NOT_ EVALUATED INSIDE AN IF STATEMENT - YET! ===

*** Function related commands:

Command: result - write a string to the function buffer (NEW)
Syntax:  #result {-result-}
Example: #func {ctof} {#math result %0*9/5+32;{#result $result degrees fahrenheit};#unvar result}
         #showme 30 degrees celsius equals @ctof{30}.
Example: #func {ftoc} {#math result (%0-32)*5/9;{#result $result degrees celsius};#unvar result}
         #showme 80 degrees fahrenheit equals @ftoc{80}.

Command: function - make a function (NEW)
Syntax:  #function {func name} {-function contents- (;#result -function result-)}
Info:    Will display all functions if no parameter is given
Example: (see example for the 'result' command)

Command: unfunction - remove a function (NEW)
Syntax:  #unfunction {func name}

Note: Functions also allow the 'new' format used for variables. that is
      @{name}{params} to allow complex names containing non-alpha chars.


That's all I could think of that I've modified on the 1.61dev version. If you find any bugs or
errors in the docs above please write me a note about it.

I'd also like suggestions to new commands/features or changes which can be made to
already existing commands.


NB! I do not officially support this version in any way! :)


//Sverre is Happy Iny (sverreno@stud.ntnu.no)
  (www.stud.ntnu.no/~sverreno)
