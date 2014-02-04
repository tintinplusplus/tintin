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


void printline(struct session *ses, char *str, int prompt)
{
	char wrapped_str[STRING_SIZE];

	push_call("printline(%p,%p,%d)",ses,str,prompt);

	if (ses->scroll_line != -1 && HAS_BIT(ses->flags, SES_FLAG_SCROLLLOCK))
	{
		pop_call();
		return;
	}

	if (HAS_BIT(ses->flags, SES_FLAG_SCAN) && !HAS_BIT(ses->flags, SES_FLAG_VERBOSE))
	{
		pop_call();
		return;
	}

	if (HAS_BIT(ses->flags, SES_FLAG_CONVERTMETA))
	{
		convert_meta(str, wrapped_str);
		strcpy(str, wrapped_str);
	}

	if (HAS_BIT(ses->flags, SES_FLAG_WORDWRAP))
	{
		word_wrap(ses, str, wrapped_str, TRUE);
	}
	else
	{
		strcpy(wrapped_str, str);
	}

	if (prompt)
	{
		printf("%s", wrapped_str);
	}
	else
	{
		printf("%s\n", wrapped_str);
	}
	pop_call();
	return;
}

/*
	Word wrapper, only wraps scrolling region, returns nr of lines - Igor
*/

int word_wrap(struct session *ses, char *textin, char *textout, int display)
{
	char *pti, *pto, *lis, *los, *chi, *cho;
	int skip = 0, cnt = 0;

	push_call("word_wrap(%s,%p,%p)",ses->name, textin,textout);

	pti = chi = lis = textin;
	pto = cho = los = textout;

	ses->cur_col = 1;

	while (*pti != 0)
	{
		if (skip_vt102_codes(pti))
		{
			if (display)
			{
				interpret_vt102_codes(ses, pti, TRUE);
			}

			for (skip = skip_vt102_codes(pti) ; skip > 0 ; skip--)
			{
				*pto++ = *pti++;
			}
			continue;
		}

		if (*pti == '\n')
		{
			*pto++ = *pti++;
			cnt = cnt + 1;
			los = pto;
			lis = pti;

			ses->cur_col = 1;

			continue;
		}

		if (*pti == ' ')
		{
			los = pto;
			lis = pti;
		}

		if (ses->cur_col > ses->cols)
		{
			cnt++;
			ses->cur_col = 1;

			if (HAS_BIT(ses->flags, SES_FLAG_WORDWRAP))
			{
				if (pto - los > 15 || !SCROLL(ses))
				{
					*pto++ = '\n';
					los = pto;
					lis = pti;
				}
				else if (lis != chi) // infinite VT loop detection
				{
					pto = los;
					*pto++ = '\n';
					pti = chi = lis;
					pti++;
				}
				else if (los != cho)
				{
					pto = cho = los;
					pto++;
					pti = chi = lis;
					pti++;
				}
			}
		}
		else
		{
			if (HAS_BIT(ses->flags, SES_FLAG_BIG5) && *pti & 128 && pti[1] != 0)
			{
				*pto++ = *pti++;
				*pto++ = *pti++;
			}
			else if (HAS_BIT(ses->flags, SES_FLAG_UTF8) && (*pti & 192) == 192)
			{
				*pto++ = *pti++;

				while ((*pti & 192) == 128)
				{
					*pto++ = *pti++;
				}
			}
			else
			{
				*pto++ = *pti++;
			}
			ses->cur_col++;
		}
	}
	*pto = 0;

	pop_call();
	return (cnt + 1);
}

// store whatever falls inbetween skip and keep.

int word_wrap_split(struct session *ses, char *textin, char *textout, int skip, int keep)
{
	char *pti, *pto, *lis, *los, *chi, *cho, *ptb;
	int i = 0, cnt = 0;

	push_call("word_wrap_split(%s,%p,%p,%d,%d)",ses->name, textin,textout, skip, keep);

	pti = chi = lis = textin;
	pto = cho = los = textout;

	ses->cur_col = 1;

	while (*pti != 0)
	{
		if (skip_vt102_codes(pti))
		{
			if (cnt >= skip && cnt < keep)
			{
				for (i = skip_vt102_codes(pti) ; i > 0 ; i--)
				{
					*pto++ = *pti++;
				}
			}
			else
			{
				pti += skip_vt102_codes(pti);
			}
			continue;
		}

		if (*pti == '\n')
		{
			if (cnt >= skip && cnt < keep)
			{
				*pto++ = *pti++;
			}
			else
			{
				pti++;
			}
			cnt = cnt + 1;
			los = pto;
			lis = pti;

			ses->cur_col = 1;

			continue;
		}

		if (*pti == ' ')
		{
			los = pto;
			lis = pti;
		}

		if (ses->cur_col > ses->cols)
		{
			cnt++;
			ses->cur_col = 1;

			if (HAS_BIT(ses->flags, SES_FLAG_WORDWRAP))
			{
				if (pto - los > 15 || !SCROLL(ses))
				{
					if (cnt >= skip && cnt < keep)
					{
						*pto++ = '\n';
					}
					los = pto;
					lis = pti;
				}
				else if (lis != chi) // infinite VT loop detection
				{
					if (cnt >= skip && cnt < keep)
					{
						pto = los;
						*pto++ = '\n';
					}
					pti = chi = lis;
					pti++;
				}
				else if (los != cho)
				{
					if (cnt >= skip && cnt < keep)
					{
						pto = cho = los;
						pto++;
					}
					else
					{
						cho = los;
					}
					pti = chi = lis;
					pti++;
				}
			}
		}
		else
		{
			ptb = pto;

			if (HAS_BIT(ses->flags, SES_FLAG_BIG5) && *pti & 128 && pti[1] != 0)
			{
				*pto++ = *pti++;
				*pto++ = *pti++;
			}
			else if (HAS_BIT(ses->flags, SES_FLAG_UTF8) && (*pti & 192) == 192)
			{
				*pto++ = *pti++;

				while ((*pti & 192) == 128)
				{
					*pto++ = *pti++;
				}
			}
			else
			{
				*pto++ = *pti++;
			}

			if (cnt < skip || cnt >= keep)
			{
				pto = ptb;
			}

			ses->cur_col++;
		}
	}
	*pto = 0;

	pop_call();
	return (cnt + 1);
}
