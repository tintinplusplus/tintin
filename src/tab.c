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

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2006                       *
******************************************************************************/


#include "tintin.h"


DO_COMMAND(do_tab)
{
	char arg1[BUFFER_SIZE];

	sub_arg_in_braces(ses, arg, arg1, 1, SUB_VAR|SUB_FUN);

	if (*arg1 == 0)
	{
		show_list(ses->list[LIST_TAB], 0);
	}
	else
	{
		update_node_list(ses->list[LIST_TAB], arg1, "", "0");

		show_message(ses, LIST_TAB, "#OK. {%s} IS NOW A TAB.", arg1);
	}
	return ses;
}


DO_COMMAND(do_untab)
{
	delete_node_with_wild(ses, LIST_TAB, arg);

	return ses;
}
