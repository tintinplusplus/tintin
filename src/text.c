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
/* file: text.c - functions for text manipulation                    */
/*                             TINTIN + +                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*                    New code by Joann Ellsworth                    */
/*********************************************************************/

#include "tintin.h"


/*
	Word wrapper, only wraps scrolling region, returns nr of lines - Scandum
*/

int word_wrap(struct session *ses, const char *textin, char *textout, int scroll)
{
	char *pti, *pto, *lis, *los;
	int len = 0, skip = 0, cnt = 0;

	pti = lis = (char *) textin;
	pto = los = (char *) textout;

	while (*pti != 0)
	{
		if (*pti == ESCAPE)
		{
			interpret_vt102_codes(ses, pti);

			for (skip = skip_vt102_codes(pti) ; skip > 0 ; skip--)
			{
				*pto = *pti;
				pto++;
				pti++;
			}
			continue;
		}

		*pto = *pti;

		if (*pti == '\n')
		{
			pto++;
			pti++;
			*pto++ = '\r';
			cnt = cnt + 1;
			len = 0;
			los = pto;
			lis = pti;

			continue;
		}

		if (*pti == ' ')
		{
			los = pto;
			lis = pti;
		}
		pto++;
		pti++;
		len++;

		if (len >= ses->cols)
		{
			if (!SCROLL(ses) && scroll)
			{
				len = 0;
				los = pto;
				lis = pti;

				continue;
			}

			while (*pti == ESCAPE)
			{
				interpret_vt102_codes(ses, pti);

				for (skip = skip_vt102_codes(pti) ; skip > 0 ; skip--)
				{
					*pto = *pti;
					pto++;
					pti++;
				}
			}

			if (*pti == '\0')
			{
				break;
			}

			if (*pti == ' ')
			{
				los = pto;
				lis = pti;
			}

			if (pto - los > 15 || !HAS_BIT(ses->flags, SES_FLAG_WORDWRAP))
			{
				*pto = '\n';
				pto++;
				*pto = '\r';
				pto++;
			}
			else
			{
				 pto = los;
				 *pto = '\n';
				 pto++;
				 *pto = '\r';
				 pto++;
				 pti = lis;
				 pti++;
			}
			cnt = cnt + 1;
			len = 0;
			los = pto;
			lis = pti;
		}
	}
	*pto = 0;

	return (cnt + 1);
}
