/* $Id: pass0.c,v 1.4 2012/09/15 20:24:55 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sun Jul 22 17:58:22 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <string.h>
#include <stdlib.h>

#include "bnf_main.h"
#include "bnf_tabsim.h"

#include "pass1.h"

#ifndef FALSE
#define FALSE							0
#define TRUE							(!FALSE)
#endif

#ifndef	DEBUG
#define DEBUG							0
#endif

#ifndef PRINT_SYMBOLS
#define PRINT_SYMBOLS					1
#endif

static char PASS1_C_RCSId[] =
"\n$Id: pass0.c,v 1.4 2012/09/15 20:24:55 luis Exp $\n";


/* PROTOTYPES */
static void pass0_pre(
		int								passno,
		enum pass_type					ptyp,
		enum syntax_node_type			ntyp,
		union syntax_node				node,
		void*							arg);

#define PASS			0

struct datos {
	struct rule*					rul;
	struct alternative*				alt;
	struct term*					trm;
};

void pass0(void)
{
	struct datos datos = {
		.rul = NULL,
		.alt = NULL,
		.trm = NULL,
	};

	pass_grammar(PASS, parsed_grammar, pass0_pre, NULL, &datos);

	if (flags & FLAG_TER_LIST) {
		printf("PASS#%dA: LISTA DE SIMBOLOS TERMINALES(%d simbolos):\n",
			PASS, avl_tree_size(tab_terminales));
#if PRINT_SYMBOLS
		print_symbols(tab_terminales,
				flags & FLAG_NOCOLOR
					? "[%s]%s"
					: "[\e[31m%s\e[0m]%s");
#else
		avl_tree_print(tab_terminales, stdout);
#endif
	} /* if */

	if (flags & FLAG_NOTER_LIST) {
		printf("PASS#%dB: LISTA DE SIMBOLOS NO TERMINALES(%d simbolos):\n",
			PASS, avl_tree_size(tab_noterminales));
#if PRINT_SYMBOLS
		print_symbols(tab_noterminales,
				flags & FLAG_NOCOLOR
					? "[%s]%s"
					: "[\e[32m%s\e[0m]%s");
#else
		avl_tree_print(tab_noterminales, stdout);
#endif
	} /* if */

} /* pass0 */

static void pass0_pre(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	struct datos* p = arg;

	switch(ntyp) {
	case T_RULE: p->rul = node.rule; break;
	case T_ALTERNATIVE: p->alt = node.alternative; break;
	case T_TERM:
		p->trm  = node.term;
		switch(p->trm->_alt) {
		case 1: /* IDENT */
			p->trm->_alt_1.definition = avl_tree_get(
					tab_noterminales,
					p->trm->_alt_1.ident->str);
			if (!p->trm->_alt_1.definition) {
				avl_tree_put(
						tab_terminales,
						p->trm->_alt_1.ident->str,
						p->trm);
			} /* if */
			break;
		case 2: /* STRING */
			avl_tree_put(
					tab_terminales,
					p->trm->_alt_2.string->str,
					p->trm);
			break;
		} /* switch */
		break;
	} /* switch */
} /* pass0_pre */

/* $Id: pass0.c,v 1.4 2012/09/15 20:24:55 luis Exp $ */
