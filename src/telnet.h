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
*******************************************************************************

*******************************************************************************
* telnet.h 1.7 88/08/19 SMI; from UCB 5.1 5/30/85                             *
*                                                                             *
* Copyright (c) 1983 Regents of the University of California.                 *
* All rights reserved.  The Berkeley software License Agreement specifies     *
* the terms and conditions for redistribution.                                *
******************************************************************************/

/*
	Definitions for the TELNET protocol.
*/

#define     IAC     255   /* interpret as command: */
#define     DONT    254   /* you are not to use option */
#define     DO      253   /* please, you use option */
#define     WONT    252   /* I won't use option */
#define     WILL    251   /* I will use option */
#define     SB      250   /* interpret as subnegotiation */
#define     GA      249   /* you may reverse the line */
#define     EL      248   /* erase the current line */
#define     EC      247   /* erase the current character */
#define     AYT     246   /* are you there */
#define     AO      245   /* abort output--but let prog finish */
#define     IP      244   /* interrupt process--permanently */
#define     BREAK   243   /* break */
#define     DM      242   /* data mark--for connect. cleaning */
#define     NOP     241   /* nop */
#define     SE      240   /* end sub negotiation */
#define     EOR     239   /* end of record (transparent mode) */
#define     ABORT   238   /* Abort process */
#define     SUSP    237   /* Suspend process */
#define     xEOF    236   /* End of file: EOF is already used... */

#define     SYNCH   242   /* for telfunc calls */


/*
	telnet options
*/

#define     TELOPT_BINARY         0     /* 8-bit data path */
#define     TELOPT_ECHO           1     /* echo */
#define     TELOPT_RCP            2     /* prepare to reconnect */
#define     TELOPT_SGA            3     /* suppress go ahead */
#define     TELOPT_NAMS           4     /* approximate message size */
#define     TELOPT_STATUS         5     /* give status */
#define     TELOPT_TIMINGMARK     6     /* timing mark */
#define     TELOPT_RCTE           7     /* remote controlled transmission and echo */
#define     TELOPT_NAOL           8     /* negotiate about output line width */
#define     TELOPT_NAOP           9     /* negotiate about output page size */
#define     TELOPT_NAOCRD        10     /* negotiate about CR disposition */
#define     TELOPT_NAOHTS        11     /* negotiate about horizontal tabstops */
#define     TELOPT_NAOHTD        12     /* negotiate about horizontal tab disposition */
#define     TELOPT_NAOFFD        13     /* negotiate about formfeed disposition */
#define     TELOPT_NAOVTS        14     /* negotiate about vertical tab stops */
#define     TELOPT_NAOVTD        15     /* negotiate about vertical tab disposition */
#define     TELOPT_NAOLFD        16     /* negotiate about output LF disposition */
#define     TELOPT_XASCII        17     /* extended ascic character set */
#define     TELOPT_LOGOUT        18     /* force logout */
#define     TELOPT_BM            19     /* byte macro */
#define     TELOPT_DET           20     /* data entry terminal */
#define     TELOPT_SUPDUP        21     /* supdup protocol */
#define     TELOPT_SUPDUPOUTPUT  22     /* supdup output */
#define     TELOPT_SNDLOC        23     /* send location */
#define     TELOPT_TTYPE         24     /* terminal type */
#define     TELOPT_EOR           25     /* end or record */
#define     TELOPT_TUID          26     /* TACACS user identification */
#define     TELOPT_OUTMRK        27     /* output marking */
#define     TELOPT_TTYLOC        28     /* terminal location number */
#define     TELOPT_3270REGIME    29     /* 3270 regime */
#define     TELOPT_X3PAD         30     /* X.3 PAD */
#define     TELOPT_NAWS          31     /* RFC 1073 */
#define     TELOPT_TSPEED        32     /* terminal speed */
#define     TELOPT_LFLOW         33     /* remote flow control */
#define     TELOPT_LINEMODE      34     /* linemode */
#define     TELOPT_XDISPLOC      35     /* X Display Location */
#define     TELOPT_OLD_ENVIRON   36     /* Old - Environment variables */
#define     TELOPT_AUTH          37     /* authentication */
#define     TELOPT_ENCRYPT       38     /* Encryption option */
#define     TELOPT_NEW_ENVIRON   39     /* New - Environment variables */
#define     TELOPT_STARTTLS      46     /* Transport Layer Security */
#define     TELOPT_MCCP          85     /* mud Compression Protocol v1 */
#define     TELOPT_MCCP2         86     /* mud Compression Protocol v2 */
#define     TELOPT_MSP           90     /* mud Sound Protocol */
#define     TELOPT_MXP           91     /* mud Extention Protocol */
#define     TELOPT_ZMP           93     /* Zeniths mud Protocol */
#define     TELOPT_EXOPL        255     /* extended-options-list */

#define     NTELOPTS             94     /* Highest telopt excluding exopl */

char *telcmds[] =
{
        "EOF",    "SUSP",   "ABORT",  "EOR",    "SE",
        "NOP",    "DMARK",  "BRK",    "IP",     "AO",
        "AYT",    "EC",     "EL",     "GA",     "SB",
        "WILL",   "WONT",   "DO",     "DONT",   "IAC",
};

#define  TELCMD_FIRST   xEOF
#define  TELCMD_LAST    IAC
#define  TELCMD_OK(x)   ((x) <= TELCMD_LAST && (x) >= TELCMD_FIRST)
#define  TELCMD(x)      telcmds[(x)-TELCMD_FIRST]


char *telopts[NTELOPTS] =
{
	"BINARY",           "ECHO",             "RCP",
	"SUPPRESS GA",      "NAME",             "STATUS",
	"TIMING MARK",      "RCTE",             "NAOL",
	"NAOP",             "NAOCRD",           "NAOHTS",
	"NAOHTD",           "NAOFFD",           "NAOVTS",
	"NAOVTD",           "NAOLFD",           "EXTEND ASCII",
	"LOGOUT",           "BYTE MACRO",       "DATA ENTRY TERMINAL",
	"SUPDUP",           "SUPDUP OUTPUT",    "SEND LOCATION",
	"TERMINAL TYPE",    "EOR",              "TACACS UID",
	"OUTPUT MARKING",   "TTYLOC",           "3270 REGIME",
	"X.3 PAD",          "NAWS",             "TSPEED",
	"LFLOW",            "LINEMODE",         "XDISPLOC",
	"OLD_ENVIRON",      "AUTH",             "ENCRYPT",
	"NEW_ENVIRON",      "40",               "41",
	"42",               "43",               "44",
	"45",               "STARTTLS",         "47",
	"48",               "49",               "50",
	"51",               "52",               "53",
	"54",               "55",               "56",
	"57",               "58",               "59",
	"60",               "61",               "62",
	"63",               "64",               "65",
	"66",               "67",               "68",
	"69",               "70",               "71",
	"72",               "73",               "74",
	"75",               "76",               "77",
	"78",               "79",               "80",
	"81",               "82",               "83",
	"84",               "MCCP1",            "MCCP2",
	"87",               "88",               "89",
	"MSP",              "MXP",              "92",
	"ZMP"
};

#define  TELOPT_FIRST   TELOPT_BINARY
#define  TELOPT_LAST    TELOPT_ZMP
#define  TELOPT_OK(x)   ((x) <= TELOPT_LAST)
#define  TELOPT(x)      telopts[(x)-TELOPT_FIRST]

/*
	sub-option qualifiers
*/

#define TELQUAL_IS              0       /* option is... */
#define TELQUAL_SEND            1       /* send option */ 
#define TELQUAL_INFO            2       /* ENVIRON: informational version of IS */
#define TELQUAL_REPLY           2       /* AUTHENTICATION: client version of IS */
#define TELQUAL_NAME            3       /* AUTHENTICATION: client version of IS */

#define LFLOW_OFF               0       /* Disable remote flow control */
#define LFLOW_ON                1       /* Enable remote flow control */ 
#define LFLOW_RESTART_ANY       2       /* Restart output on any char */ 
#define LFLOW_RESTART_XON       3       /* Restart output only on XON */ 

/*
	LINEMODE suboptions
*/

#define LM_MODE         1
#define LM_FORWARDMASK  2
#define LM_SLC          3

#define MODE_EDIT       0x01
#define MODE_TRAPSIG    0x02
#define MODE_ACK        0x04
#define MODE_SOFT_TAB   0x08
#define MODE_LIT_ECHO   0x10

#define MODE_MASK       0x1f

/* Not part of protocol, but needed to simplify things... */
#define MODE_FLOW               0x0100                      
#define MODE_ECHO               0x0200
#define MODE_INBIN              0x0400
#define MODE_OUTBIN             0x0800
#define MODE_FORCE              0x1000

#define SLC_SYNCH       1
#define SLC_BRK         2 
#define SLC_IP          3 
#define SLC_AO          4 
#define SLC_AYT         5   
#define SLC_EOR         6   
#define SLC_ABORT       7   
#define SLC_EOF         8   
#define SLC_SUSP        9   
#define SLC_EC          10
#define SLC_EL          11
#define SLC_EW          12
#define SLC_RP          13
#define SLC_LNEXT       14
#define SLC_XON         15
#define SLC_XOFF        16
#define SLC_FORW1       17
#define SLC_FORW2       18

#define NSLC            18

/*
  For backwards compatability, we define SLC_NAMES to be the
  list of names if SLC_NAMES is not defined.
*/

#define SLC_NAMELIST    "0", "SYNCH", "BRK", "IP", "AO", "AYT", "EOR", \
                        "ABORT", "EOF", "SUSP", "EC", "EL", "EW", "RP", \
                        "LNEXT", "XON", "XOFF", "FORW1", "FORW2", 0,
#ifdef  SLC_NAMES
char *slc_names[] = {
        SLC_NAMELIST
};   
#else
extern char *slc_names[];
#define SLC_NAMES SLC_NAMELIST
#endif

#define SLC_NAME_OK(x)  ((unsigned int)(x) <= NSLC)
#define SLC_NAME(x)     slc_names[x]

#define SLC_NOSUPPORT   0   
#define SLC_CANTCHANGE  1   
#define SLC_VARIABLE    2   
#define SLC_DEFAULT     3   
#define SLC_LEVELBITS   0x03

#define SLC_FUNC        0
#define SLC_FLAGS       1
#define SLC_VALUE       2

#define SLC_ACK         0x80
#define SLC_FLUSHIN     0x40
#define SLC_FLUSHOUT    0x20

#define OLD_ENV_VAR     1
#define OLD_ENV_VALUE   0
#define NEW_ENV_VAR     0
#define NEW_ENV_VALUE   1
#define ENV_ESC         2
#define ENV_USERVAR     3

#define ENV_VALUE 0
#define ENV_VAR 1  

/*
 * AUTHENTICATION suboptions
 */
   
/* 
 * Who is authenticating who ...
 */
#define AUTH_WHO_CLIENT         0       /* Client authenticating server */
#define AUTH_WHO_SERVER         1       /* Server authenticating client */
#define AUTH_WHO_MASK           1

/*
 * amount of authentication done
 */
#define AUTH_HOW_ONE_WAY        0
#define AUTH_HOW_MUTUAL         2
#define AUTH_HOW_MASK           2

#define AUTHTYPE_NULL           0
#define AUTHTYPE_KERBEROS_V4    1
#define AUTHTYPE_KERBEROS_V5    2
#define AUTHTYPE_SPX            3
#define AUTHTYPE_MINK           4
#define AUTHTYPE_CNT            5

#define AUTHTYPE_TEST           99

#ifdef  AUTH_NAMES
char *authtype_names[] = {
        "NULL", "KERBEROS_V4", "KERBEROS_V5", "SPX", "MINK", 0,
};
#else
extern char *authtype_names[];
#endif

#define AUTHTYPE_NAME_OK(x)     ((unsigned int)(x) < AUTHTYPE_CNT)
#define AUTHTYPE_NAME(x)        authtype_names[x]

/*
 * ENCRYPTion suboptions
 */
#define ENCRYPT_IS              0       /* I pick encryption type ... */
#define ENCRYPT_SUPPORT         1       /* I support encryption types ... */
#define ENCRYPT_REPLY           2       /* Initial setup response */
#define ENCRYPT_START           3       /* Am starting to send encrypted */
#define ENCRYPT_END             4       /* Am ending encrypted */
#define ENCRYPT_REQSTART        5       /* Request you start encrypting */
#define ENCRYPT_REQEND          6       /* Request you send encrypting */ 
#define ENCRYPT_ENC_KEYID       7
#define ENCRYPT_DEC_KEYID       8
#define ENCRYPT_CNT             9

#define ENCTYPE_ANY             0
#define ENCTYPE_DES_CFB64       1
#define ENCTYPE_DES_OFB64       2
#define ENCTYPE_CNT             3

#ifdef  ENCRYPT_NAMES
char *encrypt_names[] = {
        "IS", "SUPPORT", "REPLY", "START", "END",
        "REQUEST-START", "REQUEST-END", "ENC-KEYID", "DEC-KEYID",
        0,
};
char *enctype_names[] = {
        "ANY", "DES_CFB64",  "DES_OFB64",  0,
};
#else
extern char *encrypt_names[];
extern char *enctype_names[];
#endif


#define ENCRYPT_NAME_OK(x)      ((unsigned int)(x) < ENCRYPT_CNT)
#define ENCRYPT_NAME(x)         encrypt_names[x]

#define ENCTYPE_NAME_OK(x)      ((unsigned int)(x) < ENCTYPE_CNT)
#define ENCTYPE_NAME(x)         enctype_names[x]
