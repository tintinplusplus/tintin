Here's a run down on commands in tintin 1.86. by Yoon-Chan Jhi. They've all
been removed or replaced with other code however.


Added #getwalkdirection command.

Syntax : #getwalkdirection {<variable-name>}[ {<position>}]
-----------------------------------------------------------
Retrieves one pathdir from specified position within the 
walk-path.
<variable-name> specifies the variable to store retrieved 
pathdir.
The optional argument <position> specifies the position from 
which the pathdir is retrieved. If it is omitted, the default 
position for retrieval is the current walk position.

e.g.
	     #walkinfo
	     #Walk Information#
	     -----------------
	     Standstill:FALSE
	      Walk Mode:LOOP
	      Walk Path:w;e;s;n;u;d;e;w
	       Position:      ^

#getwalkdir {dir} {0}
	     #Ok. $dir is now set to {w}.
#va dir
{dir}={w} <--- this shows that the 1st pathdir is w.
			    position starts from zero.

#getwalkdir {dir}
	     #Ok. $dir is now set to {n}.
#va dir
{dir}={n} <--- this shows that the pathdir to be sent by
  the next #walk command is n

Syntax : #walkset {<alias-name>}[ {<mode-string>}]
--------------------------------------------------
<alias-name> is the name of an alias which is to be copied and 
used as a path to follow.
<mode-string> is optional. It specifies how the walk feature 
should act when specific situation has come.
As for now, it specifies tintin's behavior when the 
end/beginning of the walk path is reached while processing 
#walk(#walkback) command. The two modes provided are described 
below :

    - loop : Default mode. Tintin will jump to the other end 
    when the end of the walk_path is reached.

    - noloop : Tintin stops walking and sends nothing to mud 
    when it reached either end of walk_path.

e.g.
#walkset {testpath} {loop}
or,
#walkset {testpath} {noloop}
 

Syntax : #getwalkposition {<variable-name>}
-------------------------------------------
<variable-name> specifies the variable in which tintin stores 
current walk position.
If there's no such a variable, tintin will create one.

e.g.
#getwalkp {pos}
#va pos
{pos}={10} ---> the next #walk command will have tintin
   send the 10th pathdir within the walk_path 
   variable to the mud.

Syntax : #walkreset[ {<num-skip>}]
----------------------------------
An optional argument <num-skip> is the number of pathdirs
to be skipped from the beginning of the path alias.
A negative number will be ignored.

e.g..
#walkreset 10
---> 1. the current position is reset to the beginning.
     2. skip 10 pathdirs.
     3. the pathdir to be sent by the next #walk command
        is the 11th pathdir.


Newly added commands are :
    #walkon  - turn the session's standstill flag off.
    #walkoff - turn the session's standstill flag on.

Newly added commands are :
    #zombion  - changes a session to a zombi session.
    #zombioff - changes a zombi session to a normal 
    session.

Newly added commands are :

    #walkset {<alias-name>}
    - Starts using alias pointed by <alias-name> as a 
    path to walk through. The alias pointed by 
    <alias-name> must be created by #savepath command.

    #walk
    - Advances one step.

    #walkback
    - Backtracks one step.

    #walkinfo
    - Displays informations.

    #walkreset
    - Resets the current walk position to the beginning 
    of the path.