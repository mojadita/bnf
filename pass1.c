/* $Id: pass1.c,v 1.16 2012/09/21 18:13:55 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sun Jul 22 17:58:22 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */


#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "tokeninfo.h"

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
"\n$Id: pass1.c,v 1.16 2012/09/21 18:13:55 luis Exp $\n";

struct datos {
	struct grammar*						grm;
	struct rule*						rul;
	struct alternative*					alt;
	struct term*						trm;
	int									rule_id;
	int									alt_id;
	int									seq_id;
	int									opt_id;
};

/* PROTOTYPES */
static void pass1_term_list(
		int								passno,
		struct term_list*				p,
		struct datos*					dp);

static void pass1_pre(
		int								passno,
		enum pass_type					ptyp,
		enum syntax_node_type			ntyp,
		union syntax_node				node,
		void*							arg);

static void pass1_post(
		int								passno,
		enum pass_type					ptyp,
		enum syntax_node_type			ntyp,
		union syntax_node				node,
		void*							arg);

static struct tokeninfo* new_tokeninfo(
		char*							fmt,
		...);
static struct tokeninfo* vnew_tokeninfo(
		char*							fmt,
		va_list							p);

static struct alternative_list* new_al(
		struct tokeninfo*				t,
		struct alternative_list*		al);

static struct term_list* new_tl(
		struct tokeninfo*				t,
		struct term_list*				tl);

void pass1(void)
{
	struct datos d = {
		.grm = parsed_grammar,
		.rul = NULL,
		.alt = NULL,
		.trm = NULL,
		.alt_id = 0,
		.seq_id = 0,
		.opt_id = 0,
	};
	pass_grammar(1, parsed_grammar, pass1_pre, pass1_post, &d);
} /* pass1 */


static void pass1_pre(
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
	case T_TERM: p->trm = node.term; break;
	} /* if */
} /* pass1_pre */

static void pass1_post(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	struct datos* p = arg;

	switch(ntyp) {
	case T_RULE: p->rul = NULL; break;
	case T_ALTERNATIVE: p->alt = NULL; break;
	case T_TERM_LIST:
		pass1_term_list(passno, node.term_list, p);
		break;
	} /* if */
} /* pass1_post */

static void pass1_term_list(
		int								passno,
		struct term_list*				p,
		struct datos*					datp)
{
	struct term*						t;
	struct tokeninfo*					tok;
	struct rule_list*					rl;
	struct rule*						r;

	/* si p es NULL, retornamos, no hay term_list */
	if (!p) return;

	t = p->_alt_1.term;
	switch (t->_alt) {
	default: return; /* 1, 2: no hacemos nada. */
	case 3: /* term ::= '[' alternative_list ']' */
		/* añadimos una regla cuyo nombre es _opt_%03d_
		 * para ello:
		 * 1.- tenemos que crear un tokeninfo con el nuevo
		 *     nombre que tendrá.
		 * 2.- cambiamos el término para que represente un
		 *     identificador (el token que acabamos de crear)
		 * 3.- tenemos que añadir una alternativa con una lista
		 *     de términos vacía, que representará la omisión
		 *
		 */
		tok = new_tokeninfo("_opt_%03d_", datp->opt_id++);
#if DEBUG
		printf("PASS#%d: RULE %s (line %d): creating new rule %s.\n",
				passno, datp->rul->ident->str, datp->rul->lin, tok->str);
#endif
		rl = new_rule_list_1(datp->grm->rule_list,
				 r = new_rule(tok,
				 	new_alternative_list_1(
				 		t->_alt_3.alternative_list,
				 		new_alternative(new_term_list_2()))));
		datp->grm->rule_list = rl;
		p->_alt_1.term = new_term_1(tok);
		p->_alt_1.term->_alt_1.definition = r;
		/* TODO: tenemos que cambiar la clave con la que está dado
		 * de alta el term_list apuntado por p */
		break;

	case 4: /* term ::= '{' alternative_list '}' */
		tok = new_tokeninfo("_seq_%03d_", datp->seq_id++);
#if DEBUG
		printf("PASS#%d: RULE %s (line %d): creating new rule %s\n",
				passno, datp->rul->ident->str, datp->rul->ident->lin, tok->str);
		printf("PASS#%d: ... adding %s to all its alternatives"
				"(except the empty one):\n",
				passno, tok->str);
#endif
		rl = new_rule_list_1(datp->grm->rule_list,
				r = new_rule(
					tok,
					new_alternative_list_1(
						new_al(
							tok,
							t->_alt_4.alternative_list),
						new_alternative(new_term_list_2()))));
		datp->grm->rule_list = rl; /* añadimos la regla */
		p->_alt_1.term = new_term_1(tok);
		p->_alt_1.term->_alt_1.definition = r;
		/* TODO: tenemos que cambiar la clave con la que está dado
		 * de alta el term_list apuntado por p */
		break;

	case 5: /* term ::= '(' alternative_list ')' */
		tok = new_tokeninfo("_sub_%03d_", datp->alt_id++);
#if DEBUG
		printf("PASS#%d: RULE %s (line %d): creating new rule %s\n",
				passno, datp->rul->ident->str, datp->rul->ident->lin, tok->str);
#endif
		rl = new_rule_list_1(datp->grm->rule_list,
				r = new_rule(
					tok, t->_alt_5.alternative_list));
		datp->grm->rule_list = rl; /* añadimos la regla */
		p->_alt_1.term = new_term_1(tok);
		p->_alt_1.term->_alt_1.definition = r;
		/* TODO: tenemos que cambiar la clave con la que está dado
		 * de alta el term_list apuntado por p */
		break;

	} /* switch */

} /* pass1_term_list */

static struct tokeninfo* new_tokeninfo(char* fmt, ...)
{
	va_list p;
	struct tokeninfo* res;

	va_start(p, fmt);
	res = vnew_tokeninfo(fmt, p);
	va_end(p);

	return res;
} /* new_tokeninfo */

static struct tokeninfo* vnew_tokeninfo(char* fmt, va_list p)
{
	struct tokeninfo* res;
	char buff[128];
	AVL_ITERATOR it;

	vsnprintf(buff, sizeof buff, fmt, p);

	res = avl_tree_get(tab_subtrees, buff);
	if (res) return res;

	assert(res = malloc(sizeof(struct tokeninfo)));
	res->lin = -1; res->col = -1; res->len = strlen(buff);
	res->aut = FALSE;
	it = avl_tree_put(tab_subtrees, buff, res);
	res->str = avl_iterator_key(it);

	return res;
} /* vnew_tokeninfo */

/* genera una alternative_list a partir de la que se pasa como
 * parámetro, donde cada alternative tiene como primer
 * elemento un terminal con el tokeninfo t */
static struct alternative_list* new_al(
		struct tokeninfo*						t,
		struct alternative_list*				al)
{
	switch(al->_alt) {
	case 1: /* struct alternative_list*, struct alternative* */
		return new_alternative_list_1(
					new_al(t, al->_alt_1.alternative_list),
					new_alternative(
						new_tl(t,
							al->_alt_1.alternative->term_list)));
	case 2: /* struct alternative* */
		return new_alternative_list_2(
				new_alternative(
					new_tl(t,
						al->_alt_2.alternative->term_list)));
	} /* switch */
} /* new_al */

static struct term_list* new_tl(
		struct tokeninfo*						t,
		struct term_list*						tl)
{
	if (tl) {
		return new_term_list_1(
				new_tl(t, tl->_alt_1.term_list),
				tl->_alt_1.term);
	} else {
		return new_term_list_1(
			new_term_list_2(),
			new_term_1(t));
	} /* if */
} /* new_tl */

/* $Id: pass1.c,v 1.16 2012/09/21 18:13:55 luis Exp $ */
