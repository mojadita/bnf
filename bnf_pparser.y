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

#include "bnf_symtab.h"
#include "tokeninfo.h"
#include "bnf.h"
#include "bnf_main.h"
#include "pass1.h"
#include "bnf_sparser.h"

#ifndef FALSE
#define FALSE                       0
#define TRUE                        (!FALSE)
#endif

static char BNF_PARSER_Y_RCSId[] = "\n$Id: bnf_pparser.y,v 1.17 2012/09/15 19:57:32 luis Exp $\n";

#define YYERROR_VERBOSE             1

AVL_TREE
    tab_noterminales    =           NULL,
    tab_terminales      =           NULL,
    tab_nousados        =           NULL,
    tab_subtrees        =           NULL;

AVL_TREE
    symbol2token        =           NULL;
AVL_TREE
    token2symbol        =           NULL;

struct grammar* parsed_grammar =    NULL;

struct rule* main_rule = NULL;

static char*                        emp_on="\e[34m";
static char*                        emp_off="\e[0m";
static char*                        rul_on="\e[31m";
static char*                        rul_off="\e[0m";
static char*                        lft_on="\e[1;33m";
static char*                        lft_off="\e[0m";
static char*                        equ_on="\e[31m";
static char*                        equ_off="\e[0m";
static char*                        trm_on="\e[32m";
static char*                        trm_off="\e[0m";
static char*                        ntm_on="\e[36m";
static char*                        ntm_off="\e[0m";
static char*                        ope_on="\e[32m";
static char*                        ope_off="\e[0m";
static char*                        eol_on="\e[31m";
static char*                        eol_off="\e[0m";

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
    printf("%s.%s\n", eol_on, eol_off)

int temp_id             =           1;
char temp_buffer[15];
static int nerr = 0;

%}

%token  <tokeninfo>                 COLON_COLON_EQ
%token  <tokeninfo>                 COMMENT

%type   <grammar>                   grammar
%type   <rule>                      rule
%type   <alternative_list>          alternative_list
%type   <term_list>                 term_list
%type   <term>                      term

%token  <tokeninfo>                 IDENT STRING UNTERMINATED_STRING
%type   <tokeninfo>                 '[' ']' '{' '}' '(' ')' '.' '|'

%union {
    struct grammar*                 grammar;
    struct rule*                    rule;
    struct alternative_list*        alternative_list;
    struct term_list*               term_list;
    struct term*                    term;
    struct ti_xref*                 tokeninfo;
} /* union */

%%

grammar:
      grammar rule {
        P("001.01","grammar");
            NT("grammar");
            NT("rule");
            EOL();
      parsed_grammar = $$ = new_grammar_1($1, $2);
    }
    | rule {
        P("001.02","grammar");
            NT("rule"); EOL();
      parsed_grammar = $$ = new_grammar_2($1);
    }
    ;

rule:
      IDENT COLON_COLON_EQ alternative_list '.' {
        P("002.01","rule");
            TE("IDENT");
            OP("::=");
            NT("alternative_list");
            OP(".");
            EOL();
        $$ = new_rule_1($1, $2, $3, $4);
        $1->flags |= TI_DEFINED_HERE;
    }
    | error '.' {
        fprintf(stderr, "================[.....]================\n");
	    fprint_tokeninfo(tokeninfo_db, stderr, 
            D("syntax error recovered after '.' above. Skypped text.\n"));
    }
    ;

alternative_list:
      alternative_list '|' term_list {
        P("003.01","alternative_list");
            NT("alternative_list");
            OP("|");
            NT("term_list");
            EOL();
        $$ = new_alternative_list_1($1, $2, $3);
    }
    | term_list {
        P("003.02","alternative_list");
            NT("term_list");
            EOL();
        $$ = new_alternative_list_2($1);
    }
    ;

term_list:
      term_list term {
        P("004.01","term_list");
            NT("term_list");
            NT("term");
            EOL();
        $$ = new_term_list_1($1, $2);
    }
    | {
        P("004.02","term_list");
            EMPTY();
            EOL();
        $$ = new_term_list_2();
    }
    ;

term:
      IDENT {
        P("005.01","term");
            TE("IDENT");
            EOL();
        $$ = new_term_1($1);
    }
    | STRING {
        P("005.02","term");
            TE("STRING");
            EOL();
        $$ = new_term_2($1);
    }
    | '[' alternative_list ']' {
        P("005.03","term");
            OP("[");
            NT("alternative_list");
            OP("]");
            EOL();
        $$ = new_term_3($1, $2, $3);
    }
    | '{' alternative_list '}' {
        P("005.04","term");
            OP("{");
            NT("alternative_list");
            OP("}");
            EOL();
        $$ = new_term_4($1, $2, $3);
    }
    | '(' alternative_list ')' {
        P("005.05","term");
            OP("(");
            NT("alternative_list");
            OP(")");
            EOL();
        $$ = new_term_5($1, $2, $3);
    }
    ;

%%

#if 0
struct grammar* new_grammar(
        struct rule_list*                       lr)
{
    struct grammar *res;
    char key[64];
    AVL_ITERATOR p;
    sprintf(key, "GR(%p)", lr);
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
    LIST_INIT(&res->rul_lst);
    p = avl_tree_put(tab_subtrees, key, res);
    res->key = avl_iterator_key(p);

    return res;
} /* new_grammar */

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
        struct tokeninfo*                       p1,
        struct alternative_list*                p2)
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
        struct alternative_list*                p1,
        struct alternative*                     p2)
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
        struct term_list*                       p1)
{
    struct alternative*                         res;
    static int                                  id = 0;
    char                                        key[32];
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
        struct term_list*                       p1,
        struct term*                            p2)
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
        int                                 pass,
        struct grammar*                     grammar,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
        int                                 pass,
        struct def_list*                    def_list,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
        int                                 pass,
        struct def*                         def,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
{
    union syntax_node n;
    if (!def) return;
    n.def = def;
    if (f_pre) f_pre(pass, PREORDER, T_DEF, n, arg);
    if (f_post) f_post(pass, POSTORDER, T_DEF, n, arg);
} /* pass_def */

void pass_rule_list(
        int                                 pass,
        struct rule_list*                   rule_list,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
        int                                 pass,
        struct rule*                        rule,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
        int                                 pass,
        struct alternative_list*            alternative_list,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
        int                                 pass,
        struct alternative*                 alternative,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
        int                                 pass,
        struct term_list*                   term_list,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
        int                                 pass,
        struct term*                        term,
        callback                            f_pre,
        callback                            f_post,
        void*                               arg)
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
#endif

int yyerror(char *s)
{
	if (nerr) fputs("\n", stderr);
	fprint_tokeninfo(tokeninfo_db, stderr, "ERROR: %s\n", s);
	nerr++;
} /* yyerror */

/* $Id: bnf_pparser.y,v 1.17 2012/09/15 19:57:32 luis Exp $ */
