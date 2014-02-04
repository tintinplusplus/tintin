Due to continious improvements old tintin scripts aren't always compatible with
new versions. This document tries to document all common issues.


TINEXP 2.0 (tintin expressions) are as of now clearly defined:

%0 - %99  The percentage sign followed by a number is used for arguments in
          triggers.

$         The dollar sign followed by an alphanumeric string is used for
          variables.

&         The and sign followed by an alphanumeric string is used for
          retrieving the index of a variable.

[ ]       Brackets are used for nested variables which function as an
          associative array.

@         The at sign followed by an alphanumeric string is used for functions.

<000>     Three alphanumeric characters encapsulated by the less- and greater-
          than signs are used for color codes.

\         The back slash is used to escape a character

{ }       Curly brackets are used for seperating multi word command arguments.



" "       Quote characters are used for strings in #math and #if arguments.

&0- &99   The and sign followed by a number is used for arguments in the
          regex command.


If you want to convert your scripts take notice of the following changes:


01) #LOOP, #FORALL, and #PARSE take an additional argument which contains
    the variable used for storing the intermediate values.


02) #HIGHLIGHT is now parsed as: #HIGHLIGHT {string} {color}. The color
    argument can be a color code. See #HELP COLORS.


03) #FORMAT replaces many commands, I'd suggest checking it out.


04) #MESSAGE, #CONFIG, #IGNORE, #DEBUG are all you need to configure
    tintin, changes made with #CONFIG are written to file with #WRITE.


05) The old walk code has been removed, instead you can use the #PATH command.


06) #TICK has been changed quite a bit, new commands: #TICK and #UNTICK, you
    can have as many tickers as you want. The old #TICK command is gone.

    Syntax: #TICK {name} {commands} {seconds to tick}

    The name is added so you can delete it. You will no longer receive spam,
    but with a little bit of creativity you can rebuild the old tintin ticker
    spam.

    Use #READ to load the following ticker which emulates old behavior:

    #tick {oldtick}
    {
            #delay 50 #showme #10 SECONDS TO TICK!!;
            #showme #TICK!!!
    }
    {60}

07) #READ now supports multiline triggers, it also checks the nesting level
    before doing anything so you don't end up with bogus, and trims leading 
    spaces/tabs. Make sure to remove non matching { }'s from #NOP comments,
    cause it will (and should) create error messages with the line numbers
    between which the error was found.


08) You can place $ at the end of an action if you want a perfect match.

    #act {^The trail leads north$} {n} {5} will not trigger on:

    The trail leads northwest.


09) Added #MACRO for macro support.

10) $result is now a default variable that can be used in #FUNCTION.

    #function dosomething {#math result 1 + 1}

    say @dosomething{} will print 2, the #RESULT command has been removed.

    You can still use $result as a normal variable.

11) #FORALL and #LOOP use space separation instead of comma separation, and
    braces can be used for multi-word arguments.

13) #MATH and #IF support 64 floating point arithmatic expressions. They also
    support strings, which must be enclosed in quotes. See #HELP MATH for more
    information.

14) #PATHDIR is now specificly designed to hold a direction, the opposite
    direction, and the vector of said direction for the #MAP command.

15) Defaults are no longer in a default .rc file.

16) #SPLIT supports 2 arguments to set the top and bottomw row.

17) Can now have multiple prompt triggers, the 2nd argument is the substitution
    string that works the same as #SUBSTITUTE. the 3rd argument is the row the
    line should be printed on, if left blank it picks the default split line.

19) #SLEEP has been removed. The #DELAY command can be used instead.

20) All settings are inherited from the startup session when creating a new
    session.

21) In order to make a color trigger start a trigger with a tilda. In order to see
    color codes the way tintin wants you to enter them type: #CONFIG convert on
    when done use: #CONFIG convert off.

    #action {~\0;32m%0 says '%1'} {say I don't like green says.}

22) A scrollback buffer has been added, which can be browsed with page up,
    page down, and the #GREP and #BUFFER command.

23) !<argument> will repeat the last command starting with that argument.
    score
    who all
    area
    !          --> repeats area
    !w         --> repeats who all
    !s         --> repeats score
    !ll        --> sends !ll as a command

24) As of version 1.99.0 tintin supports PCRE (Perl Compatible Regular
    Expressions), see #HELP ACTION for details.

25) As of version 1.99.7 there are several differences in syntax:

    #LOOP {min} {max} {variable} {commands}

    #FOREACH {list} {variable} {commands}

    #PARSE {string} {variable} {commands}

26) #FORALL has been replaced by #FOREACH and will eventually be removed.

27) The argument lists of #echo and #format are no longer nested. So change:
    #format {%t:%t} {{%H}{%M}} to #format {%t} {%H} {%M}

28) #replacestring has been renamed to #replace

29) All commands require embedded regular expressions to be used
    instead of wildcards. See #HELP REGEX for details.

30) Variables can be nested using brackets. Non existant nested variables
    report 0 if the base variable exists and always report 0 when the name
    is encapsulated in braces.

    Using & instead of $ will report the index of a variable.

    Using $variable[+1] will report the first nest, and $variable[-1] will
    report the last nest, $variable[-2] the second last nest, etc.

    To display all nests use $variable[]

    To check if a variable exists use: #if {&{variable}} which will report
    0 if non existent, and the numeric index of the variable is found.

31) #LIST now works with nested variables. Read #HELP VARIABLE.

32) The #SWITCH, #CASE, #DEFAULT, #FOREACH, #BREAK, #CONTINUE, #ELSE, and
    #ELSEIF statements have been added and should work more or less as
    expected.

33) The #regex command now uses the same regular expression syntax as triggers.

34) #if {"%0" == "?test*"} needs to be replaced with #if {"%0" == "%?test%*"}
    as per the regex syntax documented in #HELP ACTION

35) old lists need to be converted as following:
    #var {friends} {{Alicia} {Bubba} {Cathie} {Kayla} {Zorro}}

    to

    #var {friends} {{1}{Alicia} {2}{Bubba} {3}{Cathie} {4}{Kayla} {5}{Zorro}}

    or

    #list friends create {{Alicia} {Bubba} {Cathie} {Kayla} {Zorro}}

