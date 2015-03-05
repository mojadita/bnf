dnl $Id: bnf.bnf,v 1.2 2012/09/08 15:58:11 luis Exp $
dnl vim: sw=4 ts=4 ai nu
dnl Author: Luis Colorado <lc@luiscoloradosistemas.com>
dnl Date: jue mar  5 17:55:11 EET 2015
dnl Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
dnl                 All rights reserved.

RULE(`grammar', `gramar_rule', `rule')
RULE(`grammar', `rule')
RULE(`rule', `IDENT', `"::="', `alternative_list', `;')
RULE(`alternative_list', `alternative_list', `"|"', `term_list')
RULE(`alternative_list', `term_list')
RULE(`term_list', `term_list', `term')
RULE(`term_list', `term')
RULE(`term', `IDENT')
RULE(`term', `STRING')
RULE(`term', `"{"', `alternative_list', `"}"')
RULE(`term', `"["', `alternative_list', `"]"')
RULE(`term', `"("', `alternative_list', `")"')

-- $Id: bnf.bnf,v 1.2 2012/09/08 15:58:11 luis Exp $
