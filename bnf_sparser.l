%{
/* $Id: bnf_sparser.l,v 1.6 2012/09/21 18:13:55 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié jul 18 10:16:50 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 * This parser processor reads yacc rules and constructs
 * two databases of terminals/nonterminals, a cross
 * reference, a grammar listing and some code to facilitate
 * the construction of the symbol table.
 * It is itself written in bison/flex.
 */


#include "bnf_pparser.h"
#include "tokeninfo.h"
#include "bnf_main.h"
#include "bnf_sparser.h"

#ifndef FALSE
#define FALSE			0
#define TRUE			!(FALSE)
#endif

static char BNF_SPARSER_L_RCSId[] = "\n$Id: bnf_sparser.l,v 1.6 2012/09/21 18:13:55 luis Exp $\n";

int						yycolno = HOME;
int						yylevel = 0;

static char*			esc1 = "\e[1;44m";
static char*			esc2 = "\e[0m";
static char*			esc3 = "\e[32m";
static char*			esc4 = "\e[0m";
static char*			esc5 = "\e[31m";
static char*			esc6 = "\e[0m";

#define SYMBOL	yytext[0]

#define TRACE(X) if (flags & FLAG_SCANNER) printf( \
	"%s:%d:%d: %s" #X "%s %s<=%s \"%s%s%s\"\n", \
	filename, yylineno, yycolno, \
	esc1, esc2, esc3, esc4, esc5, \
	yytext, esc6)

#define ACT() yycolno += yyleng

#define ADD(autodel) yylval.tokeninfo = \
	add_tokeninfo(yytext, yylineno, yycolno, yyleng, autodel);

%}

%option					noyywrap
%option					yylineno

ident					([a-z_A-Z][a-z_A-Z0-9]*)
chr						("'"([^\'\n]|\'\')*"'")
chr2					("'"([^\'\n]|\'\')*)$
str						(\"([^\"\n]|\"\")*\")
str2					(\"([^\"\n]|(.|\n))*)$
com						("--".*)

%%

" "*		{										ACT(); }
"\t"		{	yycolno -= (yycolno-HOME) % TABSIZE; yycolno += TABSIZE; }
"\n"		{	yycolno = HOME; }

"::="		{	ADD(TRUE); TRACE(COLON_COLON_EQ);	ACT(); return(COLON_COLON_EQ); }

{ident}		{	ADD(FALSE);	TRACE(IDENT);			ACT(); return(IDENT); }
{chr}		{	ADD(FALSE);	TRACE(STRING);			ACT(); return(STRING); }
{chr2}		{	ADD(FALSE);	TRACE(STRING);			ACT(); return(UNTERMINATED_STRING); }
{str}		{	ADD(FALSE);	TRACE(STRING);			ACT(); return(STRING); }
{str2}		{	ADD(FALSE);	TRACE(STRING);			ACT(); return(UNTERMINATED_STRING); }
.			{	ADD(FALSE);	TRACE(SYMBOL);			ACT(); return(SYMBOL); }
{com}$		{	ADD(TRUE);	TRACE(COMMENT);			ACT(); }

%%

void do_lex_nocolor(void)
{
	esc1 = esc2 = esc3 = esc4 = esc5 = esc6 = "";
} /* do_lex_nocolor */

/* $Id: bnf_sparser.l,v 1.6 2012/09/21 18:13:55 luis Exp $ */
