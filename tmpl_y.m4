divert(-1)dnl
dnl $Id: tmpl_y.m4,v 1.2 2012/10/12 16:05:37 luis Exp $
dnl Author: Luis Colorado <lc@luiscoloradosistemas.com>
dnl Date: Thu Sep 20 20:29:39 CEST 2012
dnl Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
dnl                 All rights reserved.

include(`macros.m4')dnl

define(`inc',`define(`$1',incr(defn(`$1')))')dnl
define(`app',`define(`$1',defn(`$1')`$2')')dnl

dnl ****************************************************
dnl * RULES SECTION
dnl ****************************************************
dnl RULE_BEGIN(rule_id, rule_name)
dnl ****************************************************
define(`rule_begin',`dnl
	define(`@the_rule_id',`$1')dnl
	define(`@the_rule_name',`$2')dnl
	define(`@the_rule_n', 0)dnl
	define(`@the_rule_N', 0)dnl
	define(`@the_rule_content')dnl
	define(`@the_rule_prototypes')dnl
D_RULES()dnl
`'
`/**** Rule #$1: $2 *********************************/'
D_TYPES()dnl
`%type <$2> $2'
D_UNION()dnl
`	struct $2* $2;'
D_NONE()dnl
')dnl

dnl ****************************************************
dnl RULE_END()
dnl ****************************************************
define(`rule_end',`dnl
D_RULES()dnl
ifelse(eval(defn(`@the_rule_N') > 1),1,`dnl
`	int				_alt;'
')dnl ifelse
ifelse(defn(`@the_rule_n'),1,`dnl if @the_rule_n == 1
`	/* union { */'
defn(`@the_rule_content')dnl
`	/* }; union (unnamed) */'
',eval(defn(`@the_rule_n') > 1),1,`dnl elsif @the_rule_n > 1
`	union {'
defn(`@the_rule_content')dnl
`	}; /* union (unnamed) */'
')dnl ifelse
`}; /* Rule #$1: $2 */'
ifelse(defn(`@the_rule_N'),0,,`dnl
`'
defn(`@the_rule_prototypes')dnl
')dnl
D_NONE()dnl
     	undefine(`@the_rule_id')dnl
	undefine(`@the_rule_name')dnl
	undefine(`@the_rule_n')dnl
	undefine(`@the_rule_N')dnl
	undefine(`@the_rule_content')dnl
	undefine(`@the_rule_prototypes')dnl
')dnl

dnl ****************************************************
dnl * ALTERNATIVES SECTION
dnl ****************************************************
dnl ALTERNATIVE_BEGIN(alt_id)
dnl ****************************************************
define(`alternative_begin',`dnl
	define(`@the_alt_id',`$1')dnl
	define(`@the_alt_n',0)dnl
	define(`@the_alt_content')dnl
	define(`@the_alt_trace')dnl
	define(`@the_alt_ix',1)dnl
	inc(`@the_rule_N')dnl
')dnl

dnl ****************************************************
dnl ALTERNATIVE_END()
dnl ****************************************************
define(`alternative_end',`dnl
D_NONE()dnl
	ifelse(eval(defn(`@the_alt_n') > 0),1,`dnl
		app(`@the_rule_content',
`		struct { /* _alt_$1 */'
defn(`@the_alt_content')dnl
`		} _alt_$1;'
)dnl app
		inc(`@the_rule_n')dnl
',`dnl ifelse )(
		app(`@the_rule_content',
`		/* _alt_$1: empty */'
)dnl app
')dnl ifelse
	app(`@the_alt_trace', `);'
)dnl
	app(`@the_rule_prototypes',defn(`@the_alt_trace'))dnl
	undefine(`@the_alt_id')dnl
	undefine(`@the_alt_n')dnl
	undefine(`@the_alt_content')dnl
	undefine(`@the_alt_trace')dnl
')dnl alternative_end

dnl ****************************************************
dnl ALTERNATIVE(alt_id)
dnl ****************************************************
define(`alternative',`dnl
	app(`@the_rule_content',
`		/* _alt_$1: empty */'
)dnl app
	app(`@the_rule_prototypes',
`struct 'defn(`@the_rule_name')`* new_'defn(`@the_rule_name')`_$1();'
)dnl app @the_rule_prototypes
	inc(`@the_rule_N')dnl
')dnl alternative

dnl ****************************************************
dnl * TERMINALS/NONTERMINALS SECTION
dnl ****************************************************
dnl NONTERMINAL(name)
dnl ****************************************************
define(`nonterminal',`dnl
	app(`@the_alt_content',
`			struct $1*	f'defn(`@the_alt_ix')`;'
)dnl app
	app(`@the_alt_trace',ifelse(defn(`@the_alt_ix'),1,,``,'')
`	struct $1*	p'defn(`@the_alt_ix'))dnl app
	inc(`@the_alt_n')dnl
	inc(`@the_alt_ix')dnl
')dnl nonterminal

dnl ****************************************************
dnl TERMINAL(name)
dnl ****************************************************
define(`terminal',`dnl
	app(`@the_alt_content',
`			struct tokeninfo*	f'defn(`@the_alt_ix')`;'
)dnl app
	app(`@the_alt_trace',ifelse(defn(`@the_alt_ix'),1,,``,'')
`	struct tokeninfo*	p'defn(`@the_alt_ix'))dnl app
	inc(`@the_alt_n')dnl
	inc(`@the_alt_ix')dnl
ifdef(`@tok_$1',,`dnl
D_TOKENS()dnl
`%token <tokeninfo> $1'
D_NONE()dnl
')dnl
define(`@tok_$1')dnl
')dnl terminal

dnl ****************************************************
dnl LITERAL( [ name, ] string)
dnl ****************************************************
define(`literal',`dnl
	app(`@the_alt_content',ifelse(`$2',,` $1',` $1'))dnl
	app(`@the_alt_trace',ifelse(`$2',,` $1', ` $2'))dnl
ifdef(`@lit_$1',,`dnl
ifelse(`$2',,,`dnl
D_TOKENS()dnl
`%token $1'
D_NONE()dnl
')dnl ifelse
')dnl ifdef
define(`@lit_$1')dnl
')dnl literal

dnl ****************************************************
dnl * TEMPLATE SECTION
dnl ****************************************************
BEGINNING_OF_TEMPLATE()dnl
`/* $Id: tmpl_y.m4,v 1.2 2012/10/12 16:05:37 luis Exp $'
` * Author: Luis Colorado <lc@luiscoloradosistemas.com>'
` * Date: 'date()
` * Source: 'source()
` * Module: 'module()
` * Template: 'template()
` * Output: 'output()
` * Command: 'command()
` * NOTE: This file GENERATED AUTOMATICALLY.  Don'_rq_()`t edit.'
` * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.'
` *                 All rights reserved.'
` */'
`%{'
`'
`#include <string.h>'
`#include <stdio.h>'
`'
`#include "avl.h"'
`#include "'module()`_tabsim.h"'
`#include "tokeninfo.h"'
`'
`static char 'module()`_PARSER_Y_RCSId[] = '
`"\n$Id: tmpl_y.m4,v 1.2 2012/10/12 16:05:37 luis Exp $\n";'
`'
`#define YYERROR_VERBOSE				1'
`'
`static char*						emp_on="\e[34m";'
`static char*						emp_off="\e[0m";'
`static char*						rul_on="\e[31m";'
`static char*						rul_off="\e[0m";'
`static char*						lft_on="\e[1;33m";'
`static char*						lft_off="\e[0m";'
`static char*						equ_on="\e[32m";'
`static char*						equ_off="\e[0m";'
`static char*						trm_on="\e[32m";'
`static char*						trm_off="\e[0m";'
`static char*						ntm_on="\e[36m";'
`static char*						ntm_off="\e[0m";'
`static char*						ope_on="\e[31m";'
`static char*						ope_off="\e[0m";'
`static char*						eol_on="\e[31m";'
`static char*						eol_off="\e[0m";'
`'
`void do_yacc_nocolor(void)'
`{'
`	emp_on = emp_off = rul_on = rul_off = lft_on = lft_off ='
`	equ_on = equ_off = trm_on = trm_off = ntm_on = ntm_off ='
`	ope_on = ope_off = eol_on = eol_off = "";'
`} /* do_yacc_nocolors */'
`'
`#define EMPTY() \'
`	do { \'
`		if (flags & FLAG_PARSER) \'
`			printf("%s-- empty --%s", emp_on, emp_off); \'
`	} while(0)'
`'
`#define P(N,L) \'
`	do { \'
`		if (flags & FLAG_PARSER) \'
`			printf(__FILE__":%d: [%sRULE " N \'
`				"%s] %s" L "%s %s::=%s ", \'
`				__LINE__, \'
`				rul_on, rul_off, \'
`				lft_on, lft_off, \'
`				equ_on, equ_off); \'
`	} while (0)'
`'
`#define TE(O) \'
`	do { \'
`		if (flags & FLAG_PARSER) \'
`			printf(" %s" O "%s", trm_on, trm_off); \'
`	} while (0)'
`'
`#define NT(O) \'
`	do { \'
`		if (flags & FLAG_PARSER) \'
`			printf(" %s" O "%s", ntm_on, ntm_off); \'
`	} while (0)'
`'
`#define OP(T) \'
`	do { \'
`		if (flags & FLAG_PARSER) \'
`			printf(" %s" T "%s", ope_on, ope_off); \'
`	} while (0)'
`'
`#define EOL() \'
`	do { \'
`		if (flags & FLAG_PARSER) \'
`			printf(" %s;%s\n", eol_on, eol_off); \'
`	} while (0)'
`'
`'
`extern AVL_TREE tab_subtrees;'
`'
`%}'
`'
`/* TOKENS */'
INSERTION_POINT(`TOKENS')dnl
`'
`/* TYPES */'
INSERTION_POINT(`TYPES')dnl
`'
`%union {'
`	struct tokeninfo* tokeninfo;'
INSERTION_POINT(`UNION')dnl
`} /* %union */'
`'
`%%'
INSERTION_POINT(`RULES')dnl
`'
`%%'
`/* $Id: tmpl_y.m4,v 1.2 2012/10/12 16:05:37 luis Exp $ */'
divert(-1)dnl
dnl $Id: tmpl_y.m4,v 1.2 2012/10/12 16:05:37 luis Exp $
