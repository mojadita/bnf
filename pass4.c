/* $Id: pass4.c,v 1.5 2012/09/15 19:57:33 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sun Jul 22 17:58:22 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

#include "tokeninfo.h"

#include "bnf_main.h"
#include "bnf_tabsim.h"

#include "pass1.h"

#ifndef FALSE
#define FALSE							0
#define TRUE							(!FALSE)
#endif

#define PASS							4

static char PASS3_C_RCSId[] =
"\n$Id: pass4.c,v 1.5 2012/09/15 19:57:33 luis Exp $\n";

static FILE*	f						= NULL;
static int indent						= 0;
#define INDENT							4

/* PROTOTYPES */
static void p4_link_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg);

static void pass4_1_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg);

static void pass4_protos_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg);

static void pass4_2_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg);

static void pass4_3_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg);

static void pass4_4_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg);

void pass4(char*					fn)
{
	static time_t					ts;
	FILE *out;
	time(&ts);

	char cmd_buffer[1024];

	snprintf(cmd_buffer, sizeof cmd_buffer,
			"%s >%s", "/usr/bin/m4", fn);

	fprintf(stderr,
			"writing output to |%s\n", cmd_buffer);

	out = popen(cmd_buffer, "wb");

	fprintf(out,
		"include(`%s')dnl\n"
		"date(`%s')dnl\n"
		"main_rule(`%s')dnl\n",
		fn,
		ctime(&ts),
		main_rule->ident->str);

} /* pass3 */

static struct rule* rul = NULL;

static void p4_link_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	static struct grammar* grm = NULL;
	static struct rule* rul = NULL;
	static struct alternative* alt = NULL;
	static struct term* trm = NULL;

	switch(ntyp) {
	case T_GRAMMAR:
		grm = node.grammar;
		break;
	case T_RULE:
		rul = node.rule;
		LIST_APPEND(&grm->rul_lst, &rul->node_rul_lst);
		break;
	case T_ALTERNATIVE:
		alt = node.alternative;
		LIST_APPEND(&rul->alt_lst, &alt->node_alt_lst);
		break;
	case T_TERM:
		trm = node.term;
		if (trm->_alt == 1 && trm->_alt_1.definition) { /* no-terminal */
			LIST_APPEND(&alt->ntrm_lst, &trm->node_ntrm_lst);
		} /* if */
		LIST_APPEND(&alt->trm_lst, &trm->node_trm_lst);
		break;
	} /* switch */
} /* p4_link_cb */

static void pass4_1_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	static struct alternative* alt = NULL;
	static struct term* trm = NULL;
	static int has_more_than_one_alt = FALSE;
	static int pass_id = 0;

	switch(ptyp) {
	case PREORDER:
		switch(ntyp) {
		case T_RULE:
			rul = node.rule;
			fprintf(f,
				"\n/******************** "
				"struct %s (Rule %d) "
				"********************/\n",
				rul->ident->str,
				rul->rule_id);

			fprintf(f,
				"struct %s {\n" ,
				rul->ident->str);

			indent = INDENT;

			has_more_than_one_alt =
				(rul->alternative_list->_alt == 1);

			if (has_more_than_one_alt) {
				fprintf(f,"%*sint _alt;\n"
						"%*sunion {\n",
						indent, "",
						indent, "");
				indent += INDENT;
			} /* if */
			break;
		case T_ALTERNATIVE:
			alt = node.alternative;

			if (has_more_than_one_alt && alt->term_list) {
				fprintf(f,
						"%*sstruct { /* alt %d */\n",
						indent, "", alt->alt_id);
				indent += INDENT;
			} /* if */
			break;
		case T_TERM:
			trm = node.term;
			switch (trm->_alt) {
			case 1: /* ident */
				if (trm->_alt_1.definition) {
					fprintf(f,
							"%*sstruct %s* %s;\n",
							indent, "",
							node.term->_alt_1.ident->str,
							node.term->_alt_1.ident->str);
				} else {
					fprintf(f,
							"%*sstruct tokeninfo* %s;\n",
							indent, "",
							node.term->_alt_1.ident->str);
				} /* if */
				break;
			} /* switch */
			break;
		} /* switch */
		break;

	case POSTORDER:
		switch(ntyp) {
		case T_TERM:
			trm = NULL; break;
		case T_ALTERNATIVE:
			if (has_more_than_one_alt && alt->term_list) {
				indent -= INDENT;
				fprintf(f,
						"%*s} _alt_%d;\n",
						indent, "",
						alt->alt_id);
			} /* if */
			alt = NULL; break;
		case T_RULE:
			if (has_more_than_one_alt) {
				indent -= INDENT;
				fprintf(f,
						"%*s}; /* union */\n",
						indent, "");
			} /* if */
			indent = 0;
			has_more_than_one_alt = FALSE;
			fprintf(f,
					"}; /* struct %s */\n",
					rul->ident->str);
			pass_rule(pass_id++, rul, pass4_protos_cb, pass4_protos_cb, NULL);
			rul = NULL; break;
		} /* switch */
		break;
	} /* switch */
} /* pass3_pre */

static void pass4_protos_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	static struct alternative* alt = NULL;
	static struct term* trm = NULL;
	static int npar = 0;

	switch(ptyp){
	case PREORDER:
		switch(ntyp) {
		case T_ALTERNATIVE:
			alt = node.alternative;
			fprintf(f,
					"\nstruct %s* new_%s_%d(",
					rul->ident->str,
					rul->ident->str,
					alt->alt_id);
			npar = 0;
			break;
		case T_TERM:
			trm = node.term;
			switch(trm->_alt) {
			case 1: /* IDENT */
				if (!npar++) {
					fprintf(f, "\n%*s", INDENT, "");
				} else {
					fprintf(f, ",\n%*s", INDENT, "");
				} /* if */
				if (trm->_alt_1.definition) { /* no terminal */
					fprintf(f, "struct %s* p%d",
							trm->_alt_1.ident->str,
							npar++);
				} else {
					fprintf(f, "struct tokeninfo* p%d",
							npar++);
				} /* if */
			} /* switch */
			break;
		} /* switch */
		break;
	case POSTORDER:
		switch(ntyp) {
		case T_ALTERNATIVE:
			fprintf(f, ");\n");
			break;
		} /* switch */
		break;
	} /* switch */
} /* pass4_protos_cb */

static void pass4_2_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	char buff[64];
	char *s;
	switch(ntyp) {
	case T_RULE:
		snprintf(buff, sizeof buff, "T_%s,\n", node.rule->ident->str);
		for (s = buff; *s; s++) *s = toupper(*s);
		fprintf(f, "%*s%s", INDENT, "", buff);
		break;
	} /* switch */
} /* pass4_2_cb */

static void pass4_3_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	char buff[64];
	char *s;
	switch(ntyp) {
	case T_RULE:
		fprintf(f, "%*sstruct %s* %s;\n",
				INDENT, "",
				node.rule->ident->str,
				node.rule->ident->str);
		break;
	} /* switch */
} /* pass4_2_cb */

static void pass4_4_cb(
	int								passno,
	enum pass_type					ptyp,
	enum syntax_node_type			ntyp,
	union syntax_node				node,
	void*							arg)
{
	char buff[64];
	char *s;
	switch(ntyp) {
	case T_RULE:
		fprintf(f,
			"\nvoid pass_%s(\n"
			"    int            passno,\n"
			"    struct %s*     p,\n"
			"    callback       f_pre,\n"
			"    callback       f_post);\n",
				node.rule->ident->str,
				node.rule->ident->str);
		break;
	} /* switch */
} /* pass4_2_cb */

/* $Id: pass4.c,v 1.5 2012/09/15 19:57:33 luis Exp $ */
