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
*******************************************************************************/

/*********************************************************************/
/* file: function.c - functions related to the functions             */
/* Modified the substitute.c file to create this,   //SN             */
/*********************************************************************/

#include "tintin.h"

/************************************************************************/
/* Usage of functions:                                                  */
/************************************************************************/
/* ex1:                                                                 */
/* #function {rnd} {#format {result} {%R} {%0}}                         */
/* #showme A random number between 1 and 100: @rnd{100}                 */
/************************************************************************/
/* ex2:                                                                 */
/* #function {ifs} {#math result {"%0" == "%1"}}                        */
/* #if {!@ifs{{$name} {$myname}} && $spam} {say Hi $name!}              */
/************************************************************************/
/* In short: functions are treated much like variables. Their value is  */
/* a command-line which is executed, and the functions are substituted  */
/* by the parameter last sent to the #result command                    */
/************************************************************************/
/* last ex:                                                             */
/* #function {lastfuncresult} {#nop}                                    */
/* #showme Last use of a function gave @lastfuncresult{} as result.     */
/************************************************************************/


DO_COMMAND(do_function)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE], *pti;
	struct listroot *root;

	root = ses->list[LIST_FUNCTION];

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 1);

	if (*left == 0)
	{
		show_list(ses, root, LIST_FUNCTION);
	}

	else if (*left && !*right)
	{
		if (show_node_with_wild(ses, left, LIST_FUNCTION) == FALSE)
		{
			if (show_message(ses, LIST_FUNCTION))
			{
				tintin_puts2("#THAT FUNCTION IS NOT DEFINED.", ses);
			}
		}
	}
	else
	{
		for (pti = left ; *pti ; pti++)
		{
			if (!isalnum(*pti))
			{
				tintin_printf2(ses, "#ERROR, {%s} IS NOT A VALID FUNCTION NAME.", left);
			}
			if (pti[1] == DEFAULT_OPEN)
			{
				break;
			}
		}
		substitute(ses, right, temp, SUB_FUN);

		updatenode_list(ses, left, temp, "0", LIST_FUNCTION);

		if (show_message(ses, LIST_FUNCTION))
		{
			tintin_printf2(ses, "#OK. {%s} now gives {%s}.",left, temp);
		}
	}
	return ses;
}


DO_COMMAND(do_unfunction)
{
	char left[BUFFER_SIZE];
	struct listroot *root;
	struct listnode *node;
	int flag = FALSE;

	root = ses->list[LIST_FUNCTION];

	arg = get_arg_in_braces(arg,left,1);

	while ((node = search_node_with_wild(root, left)) != NULL)
	{
		if (show_message(ses, LIST_FUNCTION))
		{
			tintin_printf2(ses, "#Ok. {%s} is no longer a function.", node->left);
		}
		deletenode_list(ses, node, LIST_FUNCTION);
		flag = TRUE;
	}
	if (!flag && show_message(ses, LIST_FUNCTION))
	{
		tintin_puts2("#THAT FUNCTION IS NOT DEFINED.", ses);
	}
	return ses;
}
