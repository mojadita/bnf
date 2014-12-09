/* $Id: bnf_pparser.y,v 1.17 2012/09/15 19:57:32 luis Exp $
 * vim: ts=4 sw=4 nu ai
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié jul 18 12:21:03 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

%{

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "avl.h"

#include "bnf_tabsim.h"
#include "tokeninfo.h"
#include "bnf_main.h"
#include "pass1.h"

#ifndef FALSE
#define FALSE						0
#define TRUE						(!FALSE)
#endif

static char BNF_PARSER_Y_RCSId[] = "\n$Id: bnf_pparser.y,v 1.17 2012/09/15 19:57:32 luis Exp $\n";

#define YYERROR_VERBOSE				1

AVL_TREE
	tab_noterminales	=			NULL,
	tab_terminales		=			NULL,
	tab_nousados		=			NULL,
	tab_subtrees		=			NULL;

AVL_TREE
	symbol2token		=			NULL;
AVL_TREE
	token2symbol		=			NULL;

struct grammar* parsed_grammar =	NULL;

struct rule* main_rule = NULL;

static char*						emp_on="\e[34m";
static char*						emp_off="\e[0m";
static char*						rul_on="\e[31m";
static char*						rul_off="\e[0m";
static char*						lft_on="\e[1;33m";
static char*						lft_off="\e[0m";
static char*						equ_on="\e[32m";
static char*						equ_off="\e[0m";
static char*						trm_on="\e[32m";
static char*						trm_off="\e[0m";
static char*						ntm_on="\e[36m";
static char*						ntm_off="\e[0m";
static char*						ope_on="\e[31m";
static char*						ope_off="\e[0m";
static char*						eol_on="\e[31m";
static char*						eol_off="\e[0m";

void do_yacc_nocolor(void)
{
	emp_on = emp_off = rul_on = rul_off = lft_on = lft_off =
	equ_on = equ_off = trm_on = trm_off = ntm_on = ntm_off =
	ope_on = ope_off = eol_on = eol_off = "";
} /* do_yacc_nocolors */

#define EMPTY() if (flags & FLAG_PARSER) \
	printf("%s-- empty --%s", emp_on, emp_off)

#define P(N,L) if (flags & FLAG_PARSER) \
	printf(__FILE__":%d: [%sRULE " N \
		"%s] %s" L "%s %s::=%s ", \
		__LINE__, \
		rul_on, rul_off, \
		lft_on, lft_off, \
		equ_on, equ_off)

#define TE(O) if (flags & FLAG_PARSER) \
	printf(" %s" O "%s", trm_on, trm_off)
#define NT(O) if (flags & FLAG_PARSER) \
	printf(" %s" O "%s", ntm_on, ntm_off)
#define OP(T) if (flags & FLAG_PARSER) \
	printf(" %s" T "%s", ope_on, ope_off)
#define EOL() if (flags & FLAG_PARSER) \
	printf(" %s;%s\n", eol_on, eol_off)

int temp_id				=			1;
char temp_buffer[15];
static int nerr = 0;

%}

%token	_SYMBOL _TOKEN _GOAL
%token	COLON_COLON_EQ

%type	<grammar>					grammar
%type	<def_list>					def_list
%type	<def>						def
%type	<rule_list>					rule_list
%type	<rule>						rule
%type	<alternative_list>			alternative_list
%type	<alternative>				alternative
%type	<term_list>					term_list
%type	<term>						term

%token	<tokeninfo>					IDENT STRING UNTERMINATED_STRING

%union {
	struct grammar*					grammar;
	struct def_list*				def_list;
	struct def*						def;
	struct rule_list*				rule_list;
	struct rule*					rule;
	struct alternative_list*		alternative_list;
	struct alternative*				alternative;
	struct term_list*				term_list;
	struct term*					term;
	struct tokeninfo*				tokeninfo;
} /* union */

%%

grammar:
	  def_list
	  rule_list {
		P("001.01","grammar");
			NT("def_list");
			NT("rule_list");EOL();

		parsed_grammar = $$ = new_grammar($1, $2);
		return nerr;
	}
	| error {
	}
	;

def_list:
	  def_list def {
	  	P("002.01","def_list");
			NT("def_list");
			NT("def"); EOL();
		$$ = new_def_list_1($1, $2);
	}
	| /* empty */ {
		P("002.02","def_list");
			EMPTY();EOL();
		$$ = new_def_list_2();
	}
	;

def:
	  _SYMBOL IDENT '=' STRING {
	  	P("003.01","def");
			OP("%%symbol");
			TE("IDENT");
			OP("=");
			TE("STRING");EOL();
		$$ = new_def_1($2, $4);
	}
	| _TOKEN IDENT {
		P("003.02","def");
			OP("%%token");
			TE("IDENT");EOL();
		$$ = new_def_2($2);
	}
	| _GOAL IDENT {
		P("003.03","def");
			OP("%%goal");
			TE("IDENT");EOL();
		$$ = new_def_3($2);
	}
	;

rule_list:
	  rule_list rule {
		P("004.01","rule_list");
			NT("rule_list");
			NT("rule");EOL();
		$$ = new_rule_list_1($1, $2);
	}
	| rule {
		P("004.02","rule_list");
			NT("rule");EOL();
		$$ = new_rule_list_2($1);
	}
	;

rule:
	  IDENT COLON_COLON_EQ alternative_list ';' {
	  	P("005.02","rule");
			TE("IDENT");
			OP("::=");
			NT("alternative_list");EOL();
	  	$$ = new_rule($1, $3);
	}
	| error ';'
	;

alternative_list:
	  alternative_list '|' alternative {
		P("006.01","alternative_list");
			NT("alternative_list");
			OP("|");
			NT("alternative");EOL();
	  	$$ = new_alternative_list_1($1, $3);
	}
	| alternative {
		P("006.02","alternative_list");
			NT("alternative");EOL();
		$$ = new_alternative_list_2($1);
	}
	;

alternative:
	  term_list {
	  	P("007.01","alternative");
			NT("term_list");EOL();
		$$ = new_alternative($1);
	}
	;

term_list:
	  term_list term {
		P("008.01","term_list");
			NT("term_list");
			NT("term");EOL();
	  	$$ = new_term_list_1($1, $2);
	}
	| {
		P("008.02","term_list");
			EMPTY();EOL();
		$$ = new_term_list_2();
	}
	;

term:
	  IDENT {
		P("009.01","term");
			TE("IDENT");EOL();
		$$ = new_term_1($1);
	}
	| STRING {
		P("009.02","term");
			TE("STRING");EOL();
		$$ = new_term_2($1);
	}
	| '[' alternative_list ']' {
		P("009.03","term");
			OP("[");
			NT("alternative_list");
			OP("]");EOL();
		$$ = new_term_3($2);
	}
	| '{' alternative_list '}' {
		P("009.04","term");
			OP("{");
			NT("alternative_list");
			OP("}");EOL();
		$$ = new_term_4($2);
	}
	| '(' alternative_list ')' {
		P("009.05","term");
			OP("(");
			NT("alternative_list");
			OP(")");EOL();
		$$ = new_term_5($2);
	}
	;

%%

struct grammar* new_grammar(
		struct def_list*						dl,
		struct rule_list*						lr)
{
	struct grammar *res;
	char key[64];
	AVL_ITERATOR p;
	sprintf(key, "GR(%p,%p)", dl, lr);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct grammar)));
	res->ref_count = 1;
	res->def_list = dl;
	res->rule_list = lr;
	LIST_INIT(&res->rul_lst);
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_grammar */

struct def_list* new_def_list_1(
		struct def_list*						p1,
		struct def*								p2)
{
	struct def_list* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "DL1(%p,%p)", p1, p2);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct def_list)));
	res->ref_count = 1;
	res->_alt = 1;
	res->_alt_1.def_list = p1;
	res->_alt_1.def = p2;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_def_list_1 */

struct def_list* new_def_list_2()
{
	return NULL;
} /* new_def_list_2 */

struct def* new_def_1(
		struct tokeninfo*						p1,
		struct tokeninfo*						p2)
{
	struct def* res; AVL_ITERATOR t, s;
	char *s1, *s2;
	char key[128];
	AVL_ITERATOR p;

	sprintf(key, "D1(%s,%s)", p1->str, p2->str);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct def)));
	res->ref_count = 1;
	res->_alt = 1;
	res->_alt_1.ident = p1;
	res->_alt_1.string = p2;

	if ((s1 = avl_tree_get(symbol2token, p2->str))
		|| (s2 = avl_tree_get(token2symbol, p1->str)))
	{
		fprint_tokeninfo(stderr,
			"ATENCION: %%symbol %s = %s redefine %s\n",
			p1->str, p2->str, s1 ? p2->str : p1->str);
	} /* if */

	/* AHORA VIENE UNA OPERACI\'ON COMPLICADA: Para no tener que
	 * allocar memoria, hemos ajustado el puntero de cada uno de
	 * los arboles a la clave indicada por el otro, de forma que
	 * al borrar las claves se devuelva la memoria usada por los
	 * datos. */
	t = avl_tree_put(symbol2token, p2->str, NULL);
	s = avl_tree_put(token2symbol, p1->str, avl_iterator_key(t));
	avl_tree_put(symbol2token, p2->str, avl_iterator_key(s));
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);
	
	return res;
} /* new_def_1 */

struct def* new_def_2(
		struct tokeninfo*						p1)
{
	struct def* res;
	AVL_ITERATOR t, s;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "D2(%s)", p1->str);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct def)));
	res->ref_count = 1;
	res->_alt = 2;
	res->_alt_2.ident = p1;
	t = avl_tree_put(symbol2token, p1->str, NULL);
	s = avl_tree_put(token2symbol, p1->str, avl_iterator_key(t));
	avl_tree_put(symbol2token, p1->str, avl_iterator_key(s));
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_def_2 */

struct def* new_def_3(
		struct tokeninfo*						p1)
{
	struct def* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "D3(%s)", p1->str);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct def)));
	res->ref_count = 1;
	res->_alt = 3;
	res->_alt_3.ident = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_def_3 */

struct rule_list* new_rule_list_1(
		struct rule_list* p1,
		struct rule *p2)
{
	struct rule_list* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "RL1(%p,%p)", p1, p2);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct rule_list)));
	res->ref_count = 1;
	res->_alt = 1;
	res->_alt_1.rule_list = p1;
	res->_alt_1.rule = p2;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_rule_list */

struct rule_list* new_rule_list_2(
		struct rule *p1)
{
	struct rule_list* res;
	char key[32];
	AVL_ITERATOR p;

	sprintf(key, "RL2(%p)", p1);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct rule_list)));
	res->ref_count = 1;
	res->_alt = 2;
	res->_alt_2.rule = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_rule_list_2 */

struct rule* new_rule(
		struct tokeninfo*						p1,
		struct alternative_list*				p2)
{
	struct rule* res;
	static int i = 1;
	char key[64];

	sprintf(key, "R(%s,%p)", p1->str, p2);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		fprint_tokeninfo(stderr,
			"ERROR: repeated rule %s, previous "
			"definition at line %d\n",
			p1->str, res->lin);
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	res = avl_tree_get(tab_noterminales, p1->str);
	if (res) { /* ya existe ese nodo, a\~nadimos al mismo */
		struct alternative_list* al;
		for (
			al = res->alternative_list;
			al->_alt == 1;
			al = al->_alt_1.alternative_list)
			; /* OJO, CUERPO VAC\'IO */
		al->_alt = 1;
		al->_alt_1.alternative = al->_alt_2.alternative;
		al->_alt_1.alternative_list = p2;
		fprint_tokeninfo(stderr,
			"WARNING: %s: repeated rule identifier, "
			"appending to previous definition at line %d.\n",
			p1->str, res->lin);
	} else {
		AVL_ITERATOR p;
		assert(res = malloc(sizeof(struct rule)));
		res->ref_count = 1;
		res->lin = p1->lin;
		avl_tree_put(tab_noterminales, p1->str, res);
		res->ident = p1;
		res->alternative_list = p2;
		p = avl_tree_put(tab_subtrees, key, res);
		res->key = avl_iterator_key(p);
		res->rule_id = i++;
	} /* if */

	if (!main_rule) main_rule = res;
	LIST_INIT(&res->alt_lst);

	return res;
} /* new_rule */

struct alternative_list* new_alternative_list_1(
		struct alternative_list*				p1,
		struct alternative*						p2)
{
	struct alternative_list* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "AL1(%p,%p)", p1, p2);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct alternative_list)));
	res->ref_count = 1;
	res->_alt = 1;
	res->_alt_1.alternative_list = p1;
	res->_alt_1.alternative = p2;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_alternative_list_1 */

struct alternative_list* new_alternative_list_2(
		struct alternative* p1)
{
	struct alternative_list* res;
	char key[32];
	AVL_ITERATOR p;

	sprintf(key, "AL2(%p)", p1);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct alternative_list)));
	res->ref_count = 1;
	res->_alt = 2;
	res->_alt_2.alternative = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_alternative_list_2 */

struct alternative* new_alternative(
		struct term_list*						p1)
{
	struct alternative*							res;
	static int									id = 0;
	char										key[32];
	AVL_ITERATOR p;

	sprintf(key, "A(%p)", p1);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct alternative)));
	res->ref_count = 1;
	res->term_list = p1;
	res->alt_id = id++;
	LIST_INIT(&res->ntrm_lst);
	LIST_INIT(&res->trm_lst);
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_alternative */

struct term_list* new_term_list_1(
		struct term_list*						p1,
		struct term*							p2)
{
	struct term_list* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "TL1(%p,%p)", p1, p2);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct term_list)));
	res->ref_count = 1;
	res->_alt = 1;
	res->_alt_1.term_list = p1;
	res->_alt_1.term = p2;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_term_list_1 */

struct term_list* new_term_list_2()
{
	return NULL;
} /* new_term_list_2 */

struct term* new_term_1(struct tokeninfo* p1)
{
	struct term* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "T1(%s)", p1->str);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct term)));
	res->ref_count = 1;
	res->_alt = 1;
	res->_alt_1.ident = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_term_1 */

struct term* new_term_2(struct tokeninfo* p1)
{
	struct term* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "T2(%s)", p1->str);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct term)));
	res->ref_count = 1;
	res->_alt = 2;
	res->_alt_2.string = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_term_2 */

struct term* new_term_3(struct alternative_list* p1)
{
	struct term* res;
	char key[64];
	AVL_ITERATOR p;

	sprintf(key, "T3(%p)", p1);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct term)));
	res->ref_count = 1;
	res->_alt = 3;
	res->_alt_3.alternative_list = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_term_3 */

struct term* new_term_4(struct alternative_list* p1)
{
	struct term* res;
	char key[32];
	AVL_ITERATOR p;

	sprintf(key, "T4(%p)", p1);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct term)));
	res->ref_count = 1;
	res->_alt = 4;
	res->_alt_4.alternative_list = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_term_4 */

struct term* new_term_5(struct alternative_list* p1)
{
	struct term* res;
	char key[32];
	AVL_ITERATOR p;

	sprintf(key, "T5(%p)", p1);
	res = avl_tree_get(tab_subtrees, key);
	if (res) {
		res->ref_count++;
#if 0
		printf("%s <- %d\n", key, res->ref_count);
#endif
		return res;
	} /* if */
	assert(res = malloc(sizeof(struct term)));
	res->ref_count = 1;
	res->_alt = 5;
	res->_alt_5.alternative_list = p1;
	p = avl_tree_put(tab_subtrees, key, res);
	res->key = avl_iterator_key(p);

	return res;
} /* new_term_5 */

void pass_grammar(
		int									pass,
		struct grammar*						grammar,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!grammar) return;
	n.grammar = grammar;

	if (f_pre) f_pre(pass, PREORDER, T_GRAMMAR, n, arg);
	pass_def_list(
		pass, grammar->def_list,
		f_pre, f_post,
		arg);
	pass_rule_list(
		pass, grammar->rule_list,
		f_pre, f_post,
		arg);
	if (f_post) f_post(pass, POSTORDER, T_GRAMMAR, n, arg);
} /* pass_grammar */

void pass_def_list(
		int									pass,
		struct def_list*					def_list,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!def_list) return;
	n.def_list = def_list;

	if (f_pre) f_pre(pass, PREORDER, T_DEF_LIST, n, arg);
	switch(def_list->_alt) {
	case 1:
		pass_def_list(
			pass, def_list->_alt_1.def_list,
			f_pre, f_post,
			arg);
		pass_def(
			pass, def_list->_alt_1.def,
			f_pre, f_post,
			arg);
		break;
	} /* switch */
	if (f_post) f_post(pass, POSTORDER, T_DEF_LIST, n, arg);
} /* pass_def_list */

void pass_def(
		int									pass,
		struct def*							def,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!def) return;
	n.def = def;
	if (f_pre) f_pre(pass, PREORDER, T_DEF, n, arg);
	if (f_post) f_post(pass, POSTORDER, T_DEF, n, arg);
} /* pass_def */

void pass_rule_list(
		int									pass,
		struct rule_list*					rule_list,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!rule_list) return;
	n.rule_list = rule_list;

	if (f_pre) f_pre(pass, PREORDER, T_RULE_LIST, n, arg);
	switch(rule_list->_alt) {
	case 1:
		pass_rule_list(
			pass, rule_list->_alt_1.rule_list,
			f_pre, f_post,
			arg);
		pass_rule(
			pass, rule_list->_alt_1.rule,
			f_pre, f_post,
			arg);
		break;
	case 2:
		pass_rule(
			pass, rule_list->_alt_2.rule,
			f_pre, f_post,
			arg);
		break;
	} /* switch */
	if (f_post) f_post(pass, POSTORDER, T_RULE_LIST, n, arg);
} /* pass_rule_list */

void pass_rule(
		int									pass,
		struct rule*						rule,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!rule) return;
	n.rule = rule;

	if (f_pre) f_pre(pass, PREORDER, T_RULE, n, arg);
	pass_alternative_list(
		pass, rule->alternative_list,
		f_pre, f_post,
		arg);
	if (f_post) f_post(pass, POSTORDER, T_RULE, n, arg);
} /* pass_rule */

void pass_alternative_list(
		int									pass,
		struct alternative_list*			alternative_list,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!alternative_list) return;
	n.alternative_list = alternative_list;

	if (f_pre) f_pre(pass, PREORDER, T_ALTERNATIVE_LIST, n, arg);
	switch (alternative_list->_alt) {
	case 1:
		pass_alternative_list(
			pass, alternative_list->_alt_1.alternative_list,
			f_pre, f_post,
			arg);
		pass_alternative(
			pass, alternative_list->_alt_1.alternative,
			f_pre, f_post,
			arg);
		break;
	case 2:
		pass_alternative(
			pass, alternative_list->_alt_2.alternative,
			f_pre, f_post,
			arg);
		break;
	} /* switch */
	if (f_post) f_post(pass, POSTORDER, T_ALTERNATIVE_LIST, n, arg);
} /* pass_alternative_list */

void pass_alternative(
		int									pass,
		struct alternative*					alternative,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!alternative) return;
	n.alternative = alternative;

	if (f_pre) f_pre(pass, PREORDER, T_ALTERNATIVE, n, arg);
	pass_term_list(
		pass, alternative->term_list,
		f_pre, f_post,
		arg);
	if (f_post) f_post(pass, POSTORDER, T_ALTERNATIVE, n, arg);
} /* pass_alternative */

void pass_term_list(
		int									pass,
		struct term_list*					term_list,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!term_list) return;
	n.term_list = term_list;

	if (f_pre) f_pre(pass, PREORDER, T_TERM_LIST, n, arg);
	switch (term_list->_alt) {
	case 1:
		pass_term_list(
			pass, term_list->_alt_1.term_list,
			f_pre, f_post,
			arg);
		pass_term(
			pass, term_list->_alt_1.term,
			f_pre, f_post,
			arg);
		break;
	} /* switch */
	if (f_post) f_post(pass, POSTORDER, T_TERM_LIST, n, arg);
} /* pass_alternative */

void pass_term(
		int									pass,
		struct term*						term,
		callback							f_pre,
		callback							f_post,
		void*								arg)
{
	union syntax_node n;
	if (!term) return;
	n.term = term;

	if (f_pre) f_pre(pass, PREORDER, T_TERM, n, arg);
	switch(term->_alt) {
	case 3:
		pass_alternative_list(
			pass, term->_alt_3.alternative_list,
			f_pre, f_post,
			arg);
		break;
	case 4:
		pass_alternative_list(
			pass, term->_alt_4.alternative_list,
			f_pre, f_post,
			arg);
		break;
	case 5:
		pass_alternative_list(
			pass, term->_alt_5.alternative_list,
			f_pre, f_post,
			arg);
		break;
	} /* switch */
	if (f_post) f_post(pass, POSTORDER, T_TERM, n, arg);
} /* pass_term */

int yyerror(char *s)
{
	if (nerr) fputs("\n", stderr);
	fprint_tokeninfo(stderr, "ERROR: %s\n", s);
	nerr++;
} /* yyerror */

/* $Id: bnf_pparser.y,v 1.17 2012/09/15 19:57:32 luis Exp $ */
