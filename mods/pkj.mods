
Wed May 27 18:16:43 1998  Peter Kjellerstedt  <pkj@saur.axis.se>

	* rl.c:
	Fixed so the cursor doesn't end up in the upper windows sometimes.

	* utils.c: Corrected sys_errlist for Linux (will probably give warnings
	or maybe even error on other systems...)

	* net.c, main.c: Include socks.h if SOCKS is defined.

	* Makefile.in: Some minor fixes.

Tue Apr 29 09:05:47 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* utils.c: New versions of strcasecmp() and strncasecmp().

Sun Apr 27 20:51:11 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* ansi.c: Initial revision

	* Makefile.in: Added ansi.o to the object files to make.

	* rl.c: Fixed so printline() ignores ansi codes.

	* action.c: Fixed so action triggers ignores ansi codes.

Wed Apr 23 23:17:49 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* Makefile.in: Removed history.c from the files to compile.
	Added F1 define for USE_ISO_LATIN_ONE.

	* action.c, alias.c, antisub.c, files.c, glob.c, help.c, highlight.c, ivars.c, llist.c, log.c, misc.c, net.c, parse.c, path.c, rltab.c, session.c, substitute.c, text.c, ticks.c, variables.c:
	Added const to string parameters.

	* rl.c: Added possibility to use ISO latin-1 characters by
	defining USE_ISO_LATIN_ONE.
	Fixed so C-d works as it does in emacs.
	Added const to string parameters.

	* rltick.c: Fixed so the tick messages are always shown again.
	The can be gagged if not wanted with split window.

Mon Apr 21 21:11:32 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* text.c: Fixed so read empty lines are replaced with a space.

	* ivars.c: Added message when setting a variable with #math.
	Modified the #if statement to be of the format
	#if {expr} {true clause} [{false clause}] instead of
	#if {expr} {true clause} [else {false clause}] as it was before.

	* parse.c, variables.c: Added the #removestring command.

	* misc.c, parse.c: Added the #forall command.

	* rl.c: Some minor cleanup.

	* main.c: Removed declaration of unused global variable E.

Fri Apr 18 12:54:11 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* Makefile.in: Added some warning message flags to CFLAGS.
	Restored some readline related stuff.

	* configure.in:
	Added checks for sys/ioctl.h, termcap.h, and net/errno.h.
	Added check for strcasecmp.

	* utils.c: Added some const.
	Made is_abbrev() ignore case (and somewhat more robust).
	Added strcasecmp() and strncasecmp() in case they aren't defined.

	* misc.c: Fixed so the message command can take an additional
	argument (on or off).

	* scrsize.c: Include sys/ioctl.h if it exists.

	* net.c: Include errno.h instead of sys/errno.h

	* main.c: Removed the environ argument to main().

Wed Apr 16 17:44:45 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* action.c, alias.c, antisub.c, files.c, glob.c, help.c, highlight.c, history.c, ivars.c, llist.c, log.c, misc.c, net.c, path.c, rl.c, rlhist.c, rltab.c, rltick.c, rlvt100.c, scrsize.c, session.c, substitute.c, text.c, ticks.c, variables.c:
	Added inclusion of *.proto files.

	* utils.c: Renamed is_abrev() as is_abbrev().
	Added inclusion of *.proto files.

	* parse.c: Added do_one_line() (was in main.c).
	Added inclusion of *.proto files.

	* main.c: Moved do_one_line() to parse.c
	Added inclusion of *.proto files.

Sun Apr 13 02:36:34 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* Makefile.in: Added protos to the path for include files.

	* configure.in, files.c, help.c, action.c, antisub.c, highlight.c, history.c, ivars.c, llist.c, misc.c, parse.c, path.c, rl.c, rltab.c, scrsize.c, session.c, substitute.c, text.c, ticks.c, utils.c, variables.c, rlhist.c, rltick.c, rlvt100.c, glob.c, log.c, main.c, alias.c, net.c:
	Added support for config.h

Sat Apr 12 15:22:38 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* Makefile.in: Some minor changes.

	* configure.in: Updated for autoconf v2.10

	* help.c: Fixed so check_file() handles normal files, compressed files
	and gzipped files.

	* scrsize.c, rl.c, net.c, main.c, parse.c: Some cleanup.

Thu Apr 10 17:40:35 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* tintin.h:
	Added SCREEN_HEIGHT, DEFAULT_RESOURCE_FILE, DEFAULT_HELP_FILE,
	DEFAULT_CAT_STR, DEFAULT_COMPRESS_EXT, DEFAULT_UNCOMPRESS_STR,
	DEFAULT_GZIP_EXT and DEFAULT_GUNZIP_STR.

	* rltick.c: Fixed the tickcounter in the splitline some.

Wed Apr  9 16:10:55 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* rltab.c: Fixed so the completion list is build by adding each
	new item last, instead of first.
	Fixed so that tabcompletion returns the first match only.

Mon Apr  7 01:30:50 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* rltick.c: Some minor cleanup.

	* tintin15.txt: Initial revision

	* rltick.c: Added call to tickcounter_in_splitline().

	* rl.c: Commented out linewrapping code.
	Added a \r to each output line (don't know why this is needed).
	Added tickcounter_in_splitline().

	* rltab.c: Made the tab list use five columns and be left justified.

	* main.c: Removed some not needed term stuff.

	* configure.in: Removed configure of readline.

	* variables.c, substitute.c, session.c, parse.c, misc.c, highlight.c, help.c, antisub.c, alias.c, action.c:
	Fixed a few memory errors.

	* Makefile.in: Assumes libreadline.* already exists on the system.

	* .tintinrc, .tt_help.txt, Makefile.org, variables.c, utils.c, tintin.h, ticks.c, text.c, substitute.c, session.c, scrsize.c, rlvt100.c, rltick.c, rltab.c, rlhist.c, rl.c, path.c, parse.c, net.c, misc.c, main.c, log.c, llist.c, ivars.c, install-sh, history.c, highlight.c:
	Initial revision

Sun Apr  6 23:59:53 1997  Peter Kjellerstedt  <pkj@saur.axis.se>

	* help.c, gripe, glob.c, files.c, convert.c, configure.in, antisub.c, alias.c, action.c, Makefile.in, README, INSTALL, CREDITS, CHANGES:
	Initial revision

