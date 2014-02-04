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
#include "telnet.h"

/* what we see from the remote end */

char iac_do_ttype[]           = { IAC, DO,    TELOPT_TTYPE           };
char iac_do_tspeed[]          = { IAC, DO,    TELOPT_TSPEED          };
char iac_do_xdisploc[]        = { IAC, DO,    TELOPT_XDISPLOC        };
char iac_do_new_environ[]     = { IAC, DO,    TELOPT_NEW_ENVIRON     };
char iac_do_naws[]            = { IAC, DO,    TELOPT_NAWS            };
char iac_will_naws[]          = { IAC, WILL,  TELOPT_NAWS            };
char iac_do_sga[]             = { IAC, DO,    TELOPT_SGA             };
char iac_do_echo[]            = { IAC, DO,    TELOPT_ECHO            };
char iac_will_tspeed[]        = { IAC, WILL,  TELOPT_TSPEED          };
char iac_sb_tspeed[]          = { IAC, SB,    TELOPT_TSPEED, TELQUAL_SEND, IAC, SE };
char iac_sb_ttype[]           = { IAC, SB,    TELOPT_TTYPE, TELQUAL_SEND, IAC, SE  };
char iac_dont_status[]        = { IAC, DONT,  TELOPT_STATUS          };
char iac_will_status[]        = { IAC, WILL,  TELOPT_STATUS          };
char iac_will_sga[]           = { IAC, WILL,  TELOPT_SGA             };
char iac_do_lflow[]           = { IAC, DO,    TELOPT_LFLOW           };
char iac_do_oldenviron[]      = { IAC, DO,    TELOPT_OLD_ENVIRON     };
char iac_wont_echo[]          = { IAC, WONT,  TELOPT_ECHO            };
char iac_will_echo[]          = { IAC, WILL,  TELOPT_ECHO            };
char iac_will_mccp2[]         = { IAC, WILL,  TELOPT_MCCP2           };
char iac_sb_mccp2[]           = { IAC, SB,    TELOPT_MCCP2, IAC, SE  };
char iac_will_eor[]           = { IAC, WILL,  TELOPT_EOR             };
char iac_eor[]                = { IAC, EOR                           };
char iac_ga[]                 = { IAC, GA                            };
char iac_do[]                 = { IAC, DO                            };
char iac_will[]               = { IAC, WILL                          };
char iac_sb_zmp[]             = { IAC, SB,    TELOPT_ZMP             };

struct iac_type
{
	int      size;
	char   * code;
	int  (* func) (struct session *ses, int cplen, unsigned char *cpsrc);
};

struct iac_type iac_table [] =
{
	{   3,  iac_do_sga,           &send_will_sga           },
	{   3,  iac_do_ttype,         &send_will_ttype         },
	{   3,  iac_do_xdisploc,      &send_wont_xdisploc      },
	{   3,  iac_do_new_environ,   &send_wont_new_environ   },
	{   3,  iac_do_naws,          &send_sb_naws            },
	{   3,  iac_will_naws,        &send_sb_naws            },
	{   3,  iac_do_lflow,         &send_wont_lflow         },
	{   3,  iac_do_tspeed,        &send_will_tspeed        },
	{   6,  iac_sb_tspeed,        &send_sb_tspeed          },
	{   6,  iac_sb_ttype,         &send_sb_ttype           },
	{   3,  iac_dont_status,      &send_wont_status        },
	{   3,  iac_will_sga,         &send_do_sga             },
	{   3,  iac_will_status,      &send_dont_status        },
	{   3,  iac_do_oldenviron,    &send_wont_oldenviron    },
	{   3,  iac_do_echo,          &send_echo_will          },
	{   3,  iac_will_echo,        &send_echo_off           },
	{   3,  iac_wont_echo,        &send_echo_on            },
	{   3,  iac_will_mccp2,       &send_do_mccp2           },
	{   5,  iac_sb_mccp2,         &init_mccp               },
	{   3,  iac_will_eor,         &send_do_eor             },
	{   2,  iac_eor,              &mark_prompt             },
	{   2,  iac_ga,               &mark_prompt             },
	{   2,  iac_do,               &send_wont_telopt        },
	{   2,  iac_will,             &send_dont_telopt        },
	{   3,  iac_sb_zmp,           &exec_zmp                },
	{   0,  NULL,                 NULL                     }
};


void telopt_debug(struct session *ses, char *format, ...)
{
	char buf[BUFFER_SIZE];
	va_list args;

	if (HAS_BIT(ses->telopts, TELOPT_FLAG_DEBUG))
	{
		va_start(args, format);
		vsprintf(buf, format, args);
		va_end(args);

		tintin_puts2(buf, ses);
	}
}


void translate_telopts(struct session *ses, unsigned char *src, int cplen)
{
	int skip, cnt, os;
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
				tintin_puts2("\n\r#COMPRESSION END, DISABLING MCCP.", ses);
				cplen = ses->mccp->next_out - gtd->mccp_buf;
				cpsrc = gtd->mccp_buf;
				inflateEnd(ses->mccp);
				free(ses->mccp);
				ses->mccp = NULL;
				break;

			default:
				tintin_puts2("\n\r#COMPRESSION ERROR, RESETTING MCCP.", ses);
				send_dont_mccp2(ses, 0, NULL);
				send_do_mccp2(ses, 0, NULL);
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

	if (HAS_BIT(ses->flags, SES_FLAG_LOGLEVEL) && ses->logfile)
	{
		fwrite(cpsrc, 1, cplen, ses->logfile);
		fflush(ses->logfile);
	}

	while (gtd->mud_output_len + cplen >= gtd->mud_output_max)
	{
		gtd->mud_output_max *= 2;
		gtd->mud_output_buf  = realloc(gtd->mud_output_buf, gtd->mud_output_max);
	}

	cpdst = (unsigned char *) gtd->mud_output_buf + gtd->mud_output_len;

	
	while (cplen > 0)
	{
		if (*cpsrc == IAC || HAS_BIT(ses->telopts, TELOPT_FLAG_IAC))
		{
			if (HAS_BIT(ses->telopts, TELOPT_FLAG_IAC))
			{
				DEL_BIT(ses->telopts, TELOPT_FLAG_IAC);
				os = 0;
			}
			else
			{
				os = 1; /* offset */
			}

			if (HAS_BIT(ses->telopts, TELOPT_FLAG_DEBUG))
			{
				switch(cpsrc[os])
				{
					case NOP:   
					case DM:
					case BREAK: 
					case IP:    
					case AO:    
					case AYT:   
					case EC:    
					case EL:
						tintin_printf2(ses, "RCVD %s", TELCMD(cpsrc[os]));
						break;

					case GA:
					case EOR:
						sprintf((char *) cpdst, "RCVD %s", TELCMD(cpsrc[os])); 
						gtd->mud_output_len += strlen((char *) cpdst);
						cpdst += strlen((char *) cpdst);
						break;

					case DO:
					case DONT:
					case WILL:
					case WONT:
						if (TELOPT_OK(cpsrc[os+1]))
						{
							tintin_printf2(ses, "RCVD %s %s", TELCMD(cpsrc[os]), TELOPT(cpsrc[os+1]));
						}
						else
						{
							tintin_printf2(ses, "RCVD %s %d", TELCMD(cpsrc[os]), cpsrc[os+1]);
						}
						break;

					case SB:
						if (TELOPT_OK(cpsrc[os+1]))
						{
							tintin_printf2(ses, "RCVD IAC SB %s SEND", TELOPT(cpsrc[os+1]));
						}
						else
						{
							tintin_printf2(ses, "RCVD IAC SB %d SEND", cpsrc[2]);
						}
						break;
				}
			}

			for (skip = cnt = 0 ; iac_table[cnt].code != NULL ; cnt++)
			{
				if (cplen >= iac_table[cnt].size && !memcmp(cpsrc, iac_table[cnt].code, iac_table[cnt].size))
				{
					skip = os - 1 + iac_table[cnt].func(ses, cplen, cpsrc);

					if (iac_table[cnt].func == init_mccp)
					{
						translate_telopts(ses, cpsrc + skip, cplen - skip);

						return;
					}
					break;
				}
			}

			if (skip == 0)
			{
				switch (cpsrc[os])
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
					case EOR:
					     skip = os + 1;
					     break;

					case WILL:
					case WONT:
					case DO:
					case DONT:
						skip = os + 2;
						break;

					case SB:
						skip = os + 2;
						break;

					case IAC:
						gtd->mud_output_len++;
						*cpdst++ = 0xFF;
						skip = os + 1;
						break;

					default:
						if (cplen == 1)
						{
							skip  = 0;
							cplen = 0;
							SET_BIT(ses->telopts, TELOPT_FLAG_IAC);
						}
						else
						{
							tintin_puts("#IAC BAD TELOPT", NULL);
							skip = 1;
						}
						break;
				}
			}
			cplen -= skip;
			cpsrc += skip;
		}
#ifdef BIG5
		else if (*cpsrc & 0x80)
		{
			*cpdst++ = *cpsrc++;
			gtd->mud_output_len++;
			cplen--;

			if (cplen)
			{
				*cpdst++ = *cpsrc++;
				gtd->mud_output_len++;
				cplen--;
			}
		}
#endif
		else
		{
			/*
				skip '\0' and '\r' in text input
			*/

			switch (*cpsrc)
			{
				case '\0':
					break;

				case '\r':
					if (cplen > 1 && cpsrc[1] == 0)
					{
						*cpdst++ = *cpsrc;
						gtd->mud_output_len++;
					}
					break;

				case '\n':
					if (HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_PROMPT))
					{
						DEL_BIT(gtd->ses->telopts, TELOPT_FLAG_PROMPT);
					}
					*cpdst++ = *cpsrc;
					gtd->mud_output_len++;
					break;

				default:
					if (HAS_BIT(gtd->ses->flags, SES_FLAG_CONVERTMETA))
					{
						switch (*cpsrc)
						{
							case ESCAPE:
								*cpdst++ = '\\';
								*cpdst++ = 'e';
								gtd->mud_output_len += 2;
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
						if (HAS_BIT(gtd->ses->telopts, TELOPT_FLAG_PROMPT))
						{
							DEL_BIT(gtd->ses->telopts, TELOPT_FLAG_PROMPT);
							*cpdst++ = '\n';
							gtd->mud_output_len++;
						}
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

int send_will_sga(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!HAS_BIT(ses->telopts, TELOPT_FLAG_SGA))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_SGA);

		socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_SGA);

		telopt_debug(ses, "SENT IAC WILL SGA");
	}
	return 3;
}

int send_wont_sga(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (HAS_BIT(ses->telopts, TELOPT_FLAG_SGA))
	{
		DEL_BIT(ses->telopts, TELOPT_FLAG_SGA);

		socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_SGA);

		telopt_debug(ses, "SENT IAC WONT SGA");
	}
	return 3;
}

int send_dont_sga(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_SGA);

	telopt_debug(ses, "SENT IAC DONT SGA");

	return 3;
}

int start_sga(struct session *ses, int cplen, unsigned char *cpsrc)
{
	SET_BIT(ses->telopts, TELOPT_FLAG_INIT_SGA);

	socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_SGA);

	telopt_debug(ses, "SENT IAC DO SGA");

	return 3;
}

int send_do_sga(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!HAS_BIT(ses->telopts, TELOPT_FLAG_INIT_SGA))
	{
		socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_SGA);

		telopt_debug(ses, "SENT IAC DO SGA");
	}
	SET_BIT(ses->telopts, TELOPT_FLAG_SGA);

	return 3;
}

int send_do_eor(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!HAS_BIT(ses->telopts, TELOPT_FLAG_EOR))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_EOR);

		socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_EOR);

		telopt_debug(ses, "SENT IAC DO EOR");
	}
	return 3;
}

int mark_prompt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	SET_BIT(ses->telopts, TELOPT_FLAG_PROMPT);

	return 2;
}

int send_will_ttype(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!HAS_BIT(ses->telopts, TELOPT_FLAG_INIT_TTYPE))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_INIT_TTYPE);

		socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_TTYPE);

		telopt_debug(ses, "SENT WILL TERMINAL TYPE");
	}
	return 3;
}

int send_will_tspeed(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!HAS_BIT(ses->telopts, TELOPT_FLAG_INIT_TSPEED))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_INIT_TSPEED);

		socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_TSPEED);

		telopt_debug(ses, "SENT WILL TSPEED");
	}
	return 3;
}

int send_will_naws(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (!HAS_BIT(ses->telopts, TELOPT_FLAG_INIT_NAWS))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_INIT_NAWS);

		socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_NAWS);

		telopt_debug(ses, "SENT IAC WILL NAWS");
	}
	return 3;
}

int send_wont_xdisploc(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_XDISPLOC);

	telopt_debug(ses, "SENT IAC WONT XDISPLOC");

	return 3;
}

int send_wont_new_environ(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_NEW_ENVIRON);

	telopt_debug(ses, "SENT IAC WONT NEW_ENVIRON");

	return 3;
}

int send_sb_naws(struct session *ses, int cplen, unsigned char *cpsrc)
{
	int rows;

	if (!HAS_BIT(ses->telopts, TELOPT_FLAG_INIT_NAWS))
	{
		SET_BIT(ses->telopts, TELOPT_FLAG_INIT_NAWS);

		socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_NAWS);

		telopt_debug(ses, "SENT IAC WILL NAWS");
	}

	rows = HAS_BIT(ses->flags, SES_FLAG_SPLIT) ? ses->bot_row - ses->top_row + 1 : ses->rows;

	socket_printf(ses, 9, "%c%c%c%c%c%c%c%c%c", IAC, SB, TELOPT_NAWS, 0, ses->cols % 255, 0, rows % 255, IAC, SE);

	SET_BIT(ses->telopts, TELOPT_FLAG_NAWS);

	telopt_debug(ses, "SENT IAC SB NAWS 0 %d 0 %d", ses->cols % 255, ses->rows % 255);

	return 3;
}

int send_wont_lflow(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_LFLOW);

	telopt_debug(ses, "SENT IAC WONT LFLOW");

	return 3;
}


int send_sb_tspeed(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 17, "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TSPEED, 0, "38400,38400", IAC, SE);

	telopt_debug(ses, "SENT IAC SB TSPEED");

	return 6;
}


int send_sb_ttype(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (atoi(ses->port) == TELNET_PORT && getenv("TERM"))
	{
		socket_printf(ses, 6 + strlen(getenv("TERM")), "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TTYPE, 0, getenv("TERM"), IAC, SE);

		telopt_debug(ses, "SENT IAC SB TTYPE %s", getenv("TERM"));
	}
	else
	{
		socket_printf(ses, 14, "%c%c%c%c%s%c%c", IAC, SB, TELOPT_TTYPE, 0, "TINTIN++", IAC, SE);

		telopt_debug(ses, "SENT IAC SB TTYPE %s", "TINTIN++");
	}

	return 6;
}

int send_wont_status(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_STATUS);

	telopt_debug(ses, "SENT IAC WONT STATUS");

	return 3;
}

int send_dont_status(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_STATUS);

	telopt_debug(ses, "SENT IAC DONT STATUS");

	return 3;
}


int send_wont_oldenviron(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, TELOPT_OLD_ENVIRON);

	telopt_debug(ses, "SENT IAC WONT OLD_ENVIRON");

	return 3;
}

int send_echo_on(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_ECHO);

	SET_BIT(ses->telopts, TELOPT_FLAG_ECHO);

	telopt_debug(ses, "SENT IAC DONT ECHO");

	return 3;
}

int send_echo_off(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_ECHO);

	DEL_BIT(ses->telopts, TELOPT_FLAG_ECHO);

	telopt_debug(ses, "SENT IAC DO ECHO");

	return 3;
}

int send_echo_will(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WILL, TELOPT_ECHO);

	DEL_BIT(ses->telopts, TELOPT_FLAG_ECHO);

	telopt_debug(ses, "SENT IAC WILL ECHO");

	return 3;
}

int send_ip(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 5, "%c%c%c%c%c", IAC, IP, IAC, DO, TELOPT_TIMINGMARK);

	telopt_debug(ses, "SENT IAC IP");
	telopt_debug(ses, "SENT IAC DO TIMING MARK");

	return 3;
}

int send_wont_telopt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, WONT, cpsrc[2]);

	if (TELOPT_OK(cpsrc[2]))
	{
		telopt_debug(ses, "SENT IAC WONT %s", TELOPT(cpsrc[2]));
	}
	else
	{
		telopt_debug(ses, "SENT IAC WONT %d", cpsrc[2]);
	}

	return 3;
}

int ignore_telopt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	return cplen;
}

int send_dont_telopt(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, cpsrc[2]);

	if (TELOPT_OK(cpsrc[2]))
	{
		telopt_debug(ses, "SENT IAC DONT %s", TELOPT(cpsrc[2]));
	}
	else
	{
		telopt_debug(ses, "SENT IAC DONT %d", cpsrc[2]);
	}

	return 3;
}

int exec_zmp(struct session *ses, int cplen, unsigned char *cpsrc)
{
	char buf[BUFFER_SIZE];
	int len;

	for (len = 0 ; len < cplen ; len++)
	{
		buf[len] = cpsrc[len];

		if (cpsrc[len] == IAC && cpsrc[len+1] == SE)
		{
			buf[len] = 0;
			len++;
			break;
		}
	}
	return len + 1;
}

int send_do_mccp2(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DO, TELOPT_MCCP2);

	telopt_debug(ses, "SENT IAC DO MCCP2");

	return 3;
}

int send_dont_mccp2(struct session *ses, int cplen, unsigned char *cpsrc)
{
	socket_printf(ses, 3, "%c%c%c", IAC, DONT, TELOPT_MCCP2);

	telopt_debug(ses, "SENT DONT MCCP2");

	return 3;
}


int init_mccp(struct session *ses, int cplen, unsigned char *cpsrc)
{
	if (ses->mccp)
	{
		return 5;
	}

	ses->mccp = calloc(1, sizeof(z_stream));

	ses->mccp->data_type = Z_ASCII;
	ses->mccp->zalloc    = zlib_alloc;
	ses->mccp->zfree     = zlib_free;
	ses->mccp->opaque    = NULL;

	if (inflateInit(ses->mccp) != Z_OK)
	{
		tintin_puts2("#FAILED TO INITIALIZE MCCP2.", ses);
		send_dont_mccp2(ses, 0, NULL);
		free(ses->mccp);
		ses->mccp = NULL;
	}
	else
	{
		telopt_debug(ses, "MCCP2 INITIALIZED.");
	}
	return 5;
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
	start_sga(ses, 0, NULL);
	send_will_ttype(ses, 0, NULL);
	send_will_tspeed(ses, 0, NULL);
	send_will_naws(ses, 0, NULL);
}
