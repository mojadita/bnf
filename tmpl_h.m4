divert(-1)dnl
dnl $Id: tmpl_h.m4,v 1.6 2012/09/23 12:30:10 luis Exp $
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
`struct $2 {'
`	int	ref_count;'
`	char*	key;'
D_ENUM()dnl
`	T_'translit(`$2', `a-z',`A-Z')`,'
D_SYNTAX_NODE()dnl
`	struct $2* $2;'
D_PASSES()dnl
`'
`void pass_$2('
`	struct $2*			p,'
`	callback			f_pre,'
`	callback			f_post,'
`	void*				arg);'
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
	define(`@the_alt_ix',1)dnl
	inc(`@the_rule_N')dnl
	define(`@the_alt_prototype',
`struct 'defn(`@the_rule_name')`* new_'defn(`@the_rule_name')`_$1('dnl
)dnl @the_alt_prototype
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
	app(`@the_alt_prototype', `);'
)dnl
	app(`@the_rule_prototypes',defn(`@the_alt_prototype'))dnl
	undefine(`@the_alt_id')dnl
	undefine(`@the_alt_n')dnl
	undefine(`@the_alt_content')dnl
	undefine(`@the_alt_prototype')dnl
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
	app(`@the_alt_prototype',ifelse(defn(`@the_alt_ix'),1,,``,'')
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
	app(`@the_alt_prototype',ifelse(defn(`@the_alt_ix'),1,,``,'')
`	struct tokeninfo*	p'defn(`@the_alt_ix'))dnl app
	inc(`@the_alt_n')dnl
	inc(`@the_alt_ix')dnl
')dnl terminal

dnl ****************************************************
dnl LITERAL( [ name, ] string)
dnl ****************************************************
define(`literal',`dnl
	app(`@the_alt_content',
`			/* LITERAL: 'ifelse(`$2',,`"`$1'"',``$1, "$2"'')` */'
)dnl app
	app(`@the_alt_prototype',`'
`	/* LITERAL: 'ifelse(`$2',,`"`$1'"', ``$1, "$2"'')` */'dnl
)dnl app
')dnl literal

dnl ****************************************************
dnl * TEMPLATE SECTION
dnl ****************************************************
BEGINNING_OF_TEMPLATE()dnl
`/* $Id: tmpl_h.m4,v 1.6 2012/09/23 12:30:10 luis Exp $'
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
`'
`#ifndef 'module()`_H'
`#define 'module()`_H'
`'
`#include "avl.h"'
`#include "tokeninfo.h"'
`'
`static char 'module()`_H_RCSId[] = '
`"\n$Id: tmpl_h.m4,v 1.6 2012/09/23 12:30:10 luis Exp $\n";'
`'
`extern AVL_TREE tab_subtrees;'
`'
`/************************* STRUCTURES ************************/'
INSERTION_POINT(`RULES')dnl
`'
`/************************* AUXILIARIES ***********************/'
enum pass_type {
	PREORDER,
	POSTORDER,
}; /* enum pass_type */

enum syntax_node_type {
INSERTION_POINT(`ENUM')dnl
}; /* enum syntax_node_type */

union syntax_node {
INSERTION_POINT(`SYNTAX_NODE')dnl
}; /* union syntax_node */

typedef void (*callback)(
	enum pass_type		typ,
	enum syntax_node_type	node,
	void*			arg);

`/************************* PASSES ***********************/'
INSERTION_POINT(`PASSES')dnl

`#endif /* 'module()`_H */'
`/* $Id: tmpl_h.m4,v 1.6 2012/09/23 12:30:10 luis Exp $ */'
divert(-1)dnl
dnl $Id: tmpl_h.m4,v 1.6 2012/09/23 12:30:10 luis Exp $
