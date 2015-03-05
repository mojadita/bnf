/* $Id: bnf_main.h,v 1.8 2012/09/21 18:13:55 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sat Jul 21 22:59:36 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#ifndef ESQ_MAIN_H
#define ESQ_MAIN_H

#define D(x) __FILE__":%d:%s:" x, __LINE__, __func__

#define FLAG_SCANNER		    1
#define FLAG_PARSER			    2
#define FLAG_NOTER_LIST		    4
#define FLAG_TER_LIST		    8
#define FLAG_NOCOLOR		   16
#define FLAG_XML			   32
#define FLAG_xml			   64
#define FLAG_HEADER			  128
#define FLAG_CMODULE		  256
#define FLAG_GENYACC		  512

extern int					flags;
extern char*				filename;
extern char*				m4_binary;

static char BNF_MAIN_H_CVS_ID[] = "\n$Id: bnf_main.h,v 1.8 2012/09/21 18:13:55 luis Exp $\n";

#endif /* BNF_MAIN_H */
/* $Id: bnf_main.h,v 1.8 2012/09/21 18:13:55 luis Exp $ */
