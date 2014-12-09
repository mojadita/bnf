divert(-1)dnl
dnl $Id: tmpl_xml.m4,v 1.2 2012/09/21 18:13:55 luis Exp $
dnl Author: Luis Colorado <lc@luiscoloradosistemas.com>
dnl Date: Sun Sep 16 00:44:53 CEST 2012
dnl Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
dnl                 All rights reserved.

define(`env')dnl
define(`grammar_begin',`dnl
`<!-- $Id: tmpl_xml.m4,v 1.2 2012/09/21 18:13:55 luis Exp $'
`     Author: Luis Colorado <lc@luiscoloradosistemas.com>'
`     Date:     'date
`     Source:   'source
`     Module:   'module
`     Template: 'template
`     Output:   'output
`     Command:  'command
`     Warning: THIS FILE GENERATED AUTOMATICALLY, DO NOT EDIT.'
`     Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.'
`                     All rights reserved.'
`  -->'
`<grammar>'')dnl
define(`grammar_end',`</grammar>
<!-- $Id: tmpl_xml.m4,v 1.2 2012/09/21 18:13:55 luis Exp $ -->')dnl
define(`rule_begin',`<rule id="$1" name="$2">')dnl
define(`rule_end',`</rule>')dnl
define(`alternative_begin',``<alternative id="$1">'')dnl
define(`alternative_end',``</alternative>'')dnl
define(`alternative',``<alternative id="$1"/>'')dnl
define(`nonterminal',``<nonterminal name="$1"/>'')dnl
define(`terminal',``<terminal'ifelse(`$2',,` name="$1"/>',` name="$1">'escape(`$2')`</`terminal'>')')dnl
define(`literal',``<literal'ifelse(`$2',,`>$1</`literal'>',`` name="$1">''escape(`$2')``</literal>'')')dnl
define(`optional_begin',`<option>')dnl
define(`optional_end',`</option>')dnl
define(`sequence_begin',`<sequence>')dnl
define(`sequence_end',`</sequence>')dnl
define(`subexpression_begin',`<subexpression>')dnl
define(`subexpression_end',`</subexpression>')dnl
divert(0)dnl
dnl $Id: tmpl_xml.m4,v 1.2 2012/09/21 18:13:55 luis Exp $
