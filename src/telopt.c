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
*   file: telopt.c - funtions related to telnet negotations                   *
*           (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t ++ 2.00              *
*                     coded by peter unold 1992                               *
*                     new code by Igor van den Hoven 2004                     *
*******************************************************************************/

#include "tintin.h"

#define TELOPTS
#define TELCMDS
#include <arpa/telnet.h>

/* what we see from the remote end */

char iac_do_ttype[]           = { IAC, DO,    TELOPT_TTYPE           };
char iac_do_tspeed[]          = { IAC, DO,    TELOPT_TSPEED          };
char iac_do_xdisploc[]        = { IAC, DO,    TELOPT_XDISPLOC        };
char iac_do_new_environ[]     = { IAC, DO,    TELOPT_NEW_ENVIRON     };
char iac_do_naws[]            = { IAC, DO,    TELOPT_NAWS            };
char iac_do_sga[]             = { IAC, DO,    TELOPT_SGA             };
char iac_do_echo[]            = { IAC, DO,    TELOPT_ECHO            };
char iac_sb_tspeed[]          = { IAC, SB,    TELOPT_TSPEED, IAC, SE };
char iac_sb_ttype[]           = { IAC, SB,    TELOPT_TTYPE, IAC, SE  };
char iac_dont_status[]        = { IAC, DONT,  TELOPT_STATUS          };
char iac_will_status[]        = { IAC, WILL,  TELOPT_STATUS          };
char iac_will_sga[]           = { IAC, WILL,  TELOPT_SGA             };
char iac_do_lflow[]           = { IAC, DO,    TELOPT_LFLOW           };
char iac_do_oldenviron[]      = { IAC, DO,    TELOPT_OLD_ENVIRON     };
char iac_wont_echo[]          = { IAC, WONT,  TELOPT_ECHO            };
char iac_will_echo[]          = { IAC, WILL,  TELOPT_ECHO            };
char iac_will_mccp2[]         = { IAC, WILL,  TELOPT_MCCP2           };
char iac_sb_mccp2[]           = { IAC, SB,    TELOPT_MCCP2, IAC, SE  };


struct iac_type
{
	int    size;
	char * code;
	void (*func) (struct session *);
};


/*
	despite the fact the first three are written, they should not run in an
	ideal world, after we have session initialization code. - jdoiel

	some muds use ttype - Scandum
*/

const struct iac_type iac_table [TELOPT_MAX] =
{
	{   3,  iac_do_sga,           NULL                     },
	{   3,  iac_do_ttype,         &send_sb_ttype           },
	{   3,  iac_do_tspeed,        NULL                     },
	{   3,  iac_do_xdisploc,      &send_wont_xdisploc      },
	{   3,  iac_do_new_environ,   &send_wont_new_environ   },
	{   3,  iac_do_naws,          &send_sb_naws            },
	{   3,  iac_do_lflow,         &send_wont_lflow       	},
	{   3,  iac_sb_tspeed,        &send_sb_tspeed          },
	{   3,  iac_sb_ttype,         &send_sb_ttype 		},
	{   3,  iac_dont_status,      &send_wont_status      	},
	{   3,  iac_will_sga,         &send_do_sga             },
	{   3,  iac_will_status,      &send_dont_status      	},
	{   3,  iac_do_oldenviron,    &send_wont_oldenviron    },
	{   3,  iac_do_echo,          &send_echo_will          },
	{   3,  iac_will_echo,        &send_echo_off           },
	{   3,  iac_wont_echo,        &send_echo_on            },
	{   3,  iac_will_mccp2,       &send_do_mccp2           },
	{   5,  iac_sb_mccp2,         &init_mccp               }
};


void telopt_debug(struct session *ses, char *msg)
{
	if (HAS_BIT(ses->flags, SES_FLAG_DEBUGTELNET))
	{
		tintin_puts2(msg, ses);
	}
}


void translate_telopts(struct session *ses, unsigned char *src, int cplen)
{
	int skip, cnt;
	unsigned char *cpdst;
	unsigned char *cpsrc;

	if (cplen == 0)
	{
		return;
	}

	if (ses->mccp)
	{
		ses->mccp->next_in   = src;
		ses->mccp->avail_in  = cplen;

		ses->mccp->next_out  = gtd->mccp_buf;
		ses->mccp->avail_out = gtd->mccp_buf_max;

		inflate:

		switch (inflate(ses->mccp, Z_SYNC_FLUSH))
		{
			case Z_OK:
				if (ses->mccp->avail_out == 0)
				{
					gtd->mccp_buf_max    *= 2;
					gtd->mccp_buf         = realloc(gtd->mccp_buf, gtd->mccp_buf_max);

					ses->mccp->avail_out  = gtd->mccp_buf_max / 2;
					ses->mccp->next_out   = gtd->mccp_buf + gtd->mccp_buf_max / 2;

					goto inflate;
				}
				cplen = gtd->mccp_buf_max - ses->mccp->avail_out;
				cpsrc = gtd->mccp_buf;
				break;

			case Z_STREAM_END:
				cplen = ses->mccp->next_out - gtd->mccp_buf;
				cpsrc = gtd->mccp_buf;
				inflateEnd(ses->mccp);
				free(ses->mccp);
				ses->mccp = NULL;
				break;

			default:
				tintin_puts2("\n\r#COMPRESSION ERROR, RESETTING MCCP.", ses);
				send_dont_mccp2(ses);
				send_do_mccp2(ses);
				inflateEnd(ses->mccp);
				free(ses->mccp);
				ses->mccp = NULL;
				cpsrc = src;
				cplen = 0;
				break;
		}
	}
	else
	{
		cpsrc = src;
	}

	while (gtd->mud_output_len + cplen >= gtd->mud_output_max)
	{
		gtd->mud_output_max *= 2;
		gtd->mud_output_buf  = realloc(gtd->mud_output_buf, gtd->mud_output_max);
	}

	cpdst = (unsigned char *) gtd->mud_output_buf + gtd->mud_output_len;

	while (cplen > 0)
	{
		if (*cpsrc == IAC)
		{
			if (HAS_BIT(ses->flags, SES_FLAG_DEBUGTELNET))
			{
				switch(cpsrc[1])
				{
					case NOP:   
					case DM:
					case BREAK: 
					case IP:    
					case AO:    
					case AYT:   
					case EC:    
					case EL:
						tintin_printf2(ses, "RCVD %s", TELCMD(cpsrc[1]));
						break;

					case DO:
					case DONT:
					case WILL:
					case WONT:
						if (TELOPT_OK(cpsrc[2]))
						{
							tintin_printf2(ses, "RCVD %s %s", TELCMD(cpsrc[1]), TELOPT(cpsrc[2]));
						}
						else
						{
							tintin_printf2(ses, "RCVD %s %d", TELCMD(cpsrc[1]), cpsrc[2]);
						}
						break;

					case SB:
						if (TELOPT_OK(cpsrc[2]))
						{
							tintin_printf2(ses, "RCVD IAC SB %s SEND", TELOPT(cpsrc[2]));
						}
						else
						{
							tintin_printf2(ses, "RCVD IAC SB %d SEND", cpsrc[2]);
						}
						break;
				}
			}

			for (cnt = 0 ; cnt < TELOPT_MAX ; cnt++)
			{
				if (cplen >= iac_table[cnt].size && !memcmp(cpsrc, iac_table[cnt].code, iac_table[cnt].size))
				{
					skip = iac_table[cnt].size;

					if (iac_table[cnt].func != NULL)
					{
						iac_table[cnt].func(ses);
					}

					if (iac_table[cnt].func == init_mccp)
					{
						translate_telopts(ses, cpsrc + skip, cplen - skip);

						return;
					}
				}
			}

			switch (cpsrc[1])
			{
				case SE:
				case NOP:
				case DM:
				case BREAK:
				case IP:
				case AO:
				case AYT:
				case EC:
				case EL:
				case GA:
				     skip = 2;
				     break;

				case WILL:
				case WONT:
				case DO:
				case DONT:
					skip = 3;
					break;

				case SB:
					skip = 3;
					break;

				case IAC:
					gtd->mud_output_len++;
					*cpdst++ = 0xFF;
					skip = 2;
					break;

				default:
					tintin_puts("#IAC BAD TELOPT", NULL);
					skip = 1;
					break;
			}
			cplen -= skip;
			cpsrc += skip;
		}
		else
		{
			/*
				skip '\0' and '\r' in text input
			*/

			switch (*cpsrc)
			{
				case '\0':
				case '\r':
					break;

				default:
					if (HAS_BIT(gtd->ses->flags, SES_FLAG_CONVERTMETA))
					{
						switch (*cpsrc)
						{
							case '\e':
								*cpdst++ = '\\';
								*cpdst++ = 'e';
								gtd->mud_output_len += 2;
								break;

							case '\n':
								*cpdst++ = *cpsrc;
								gtd->mud_output_len++;
								break;

							default:
								if (*cpsrc < 27)
								{
									*cpdst++ = '\\';
									*cpdst++ = 'C';
									*cpdst++ = '-';
									*cpdst++ = 'a' + *cpsrc - 1;
									gtd->mud_output_len += 4;
								}
								else
								{
									*cpdst++ = *cpsrc;
									gtd->mud_output_len++;
								}
								break;
						}
					}
					else
					{
						*cpdst++ = *cpsrc;
						gtd->mud_output_len++;
					}
					break;
			}
			cpsrc++;
			cplen--;
		}
	}
}

void send_will_sga(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_SGA);

	telopt_debug(ses, "SENT IAC WILL SGA");
}

void send_will_ttype(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_TTYPE);

	telopt_debug(ses, "SENT WILL TERMINAL TYPE");
}

void send_will_tspeed(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_TSPEED);

	telopt_debug(ses, "SENT WILL TERMINAL SPEED");
}

void send_will_naws(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_NAWS);

	telopt_debug(ses, "SENT IAC WILL NAWS");
}

void send_wont_xdisploc(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_XDISPLOC);

	telopt_debug(ses, "SENT IAC WONT XDISPLOC");
}

void send_wont_new_environ(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_NEW_ENVIRON);

	telopt_debug(ses, "SENT IAC WONT NEW_ENVIRON");
}

void send_sb_naws(struct session *ses)
{
	socket_printf(ses, 9, "%c%c%c%c%c%c%c%c%c", IAC, SB, TELOPT_NAWS, 0, ses->cols % 255, 0, ses->rows % 255, IAC, SE);

	SET_BIT(ses->flags, SES_FLAG_NAWS);

	telopt_debug(ses, "SENT IAC SB NAWS");
}

void send_wont_lflow(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_LFLOW);

	telopt_debug(ses, "SENT IAC WONT LFLOW");
}


void send_sb_tspeed(struct session *ses)
{
	socket_printf(ses, 17, "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TSPEED, 0, "38400,38400", IAC, SE);

	telopt_debug(ses, "SENT IAC SB TSPEED");
}


void send_sb_ttype(struct session *ses)
{
	if (atoi(ses->port) == TELNET_PORT && getenv("TERM"))
	{
		socket_printf(ses, 6 + strlen(getenv("TERM")), "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TTYPE, 0, getenv("TERM"), IAC, SE);
	}
	else
	{
		socket_printf(ses, 14, "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TTYPE, 0, "TINTIN++", IAC, SE);
	}

	telopt_debug(ses, "SENT IAC SB TTYPE");
}

void send_wont_status(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_STATUS);

	telopt_debug(ses, "SENT IAC WONT STATUS");
}

void send_dont_status(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_STATUS);

	telopt_debug(ses, "SENT IAC DONT STATUS");
}

void send_do_sga(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_SGA);

	telopt_debug(ses, "SENT IAC DO SGA");
}

void send_wont_oldenviron(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_OLD_ENVIRON);

	telopt_debug(ses, "SENT IAC WONT OLD_ENVIRON");
}

void send_echo_on(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_ECHO);

	SET_BIT(ses->flags, SES_FLAG_LOCALECHO);

	readline_echoing_p = TRUE;

	telopt_debug(ses, "SENT IAC DONT ECHO");
}

void send_echo_off(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_ECHO);

	DEL_BIT(ses->flags, SES_FLAG_LOCALECHO);

	readline_echoing_p = FALSE;

	telopt_debug(ses, "SENT IAC DO ECHO");
}

void send_echo_will(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_ECHO);

	DEL_BIT(ses->flags, SES_FLAG_LOCALECHO);

	telopt_debug(ses, "SENT IAC WILL ECHO");
}

void send_do_mccp2(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_MCCP2);

	telopt_debug(ses, "SENT IAC DO MMCP2");
}

void send_dont_mccp2(struct session *ses)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_MCCP2);

	telopt_debug(ses, "SENT DONT MCCP2");
}


void init_mccp(struct session *ses)
{
	if (ses->mccp)
	{
		return;
	}

	ses->mccp = calloc(1, sizeof(z_stream));

	ses->mccp->data_type = Z_ASCII;
	ses->mccp->zalloc    = zlib_alloc;
	ses->mccp->zfree     = zlib_free;
	ses->mccp->opaque    = NULL;

	if (inflateInit(ses->mccp) != Z_OK)
	{
		tintin_puts2("#FAILED TO INITIALIZE MCCP.", ses);
		send_dont_mccp2(ses);
		free(ses->mccp);
		ses->mccp = NULL;
	}
	else
	{
		telopt_debug(ses, "#MCCP INITIALIZED.");
	}
}


void *zlib_alloc(void *opaque, unsigned int items, unsigned int size)
{
	return calloc(items, size);
}


void zlib_free(void *opaque, void *address)
{
	free(address);
}

void init_telnet_session(struct session *ses)
{
	send_do_sga(ses);
	send_will_ttype(ses);
	send_will_tspeed(ses);
	send_will_naws(ses);

	return;
}
