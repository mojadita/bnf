/* $Id: pass3.c,v 1.14 2012/09/22 22:51:55 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sun Jul 22 17:58:22 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "bnf_main.h"
#include "bnf_cfg.h"
#include "bnf_tabsim.h"

#include "pass1.h"

#ifndef FALSE
#define FALSE                           0
#define TRUE                            (!FALSE)
#endif

#define PASS                            3

static char PASS3_C_RCSId[] =
"\n$Id: pass3.c,v 1.14 2012/09/22 22:51:55 luis Exp $\n";

/* PROTOTYPES */
static void pass3_pre(
    int                             passno,
    enum pass_type                  ptyp,
    enum syntax_node_type           ntyp,
    union syntax_node               node,
    void*                           arg);

static void pass3_post(
    int                             passno,
    enum pass_type                  ptyp,
    enum syntax_node_type           ntyp,
    union syntax_node               node,
    void*                           arg);

static char *strdequote(
        char*                       s);

struct datos {
    int                             indent;
    int                             increment;
    char*                           input_name;
    char*                           module_name;
    char*                           template_name;
    char*                           output_name;
    char*                           command;
    FILE*                           file;
};

void pass3(
        char*                       fn,
        char*                       tpl)
{
    static time_t                   ts;
    char buffer[1024];
    int buffer_sz = sizeof buffer, n;
    char *p = buffer;
    struct datos d;

    d.indent = 0;
    d.increment = 5;
    d.file = NULL;
    d.input_name = filename;

    d.module_name = buffer;
    snprintf(buffer, buffer_sz, "%s", filename);
    p = strchr(buffer, '.');
    if (p) {
        *p++ = '\0';
        buffer_sz -= p - buffer;
    } else {
        n = strlen(buffer) + 1;
        buffer_sz -= n; 
        p = buffer + n;
    } /* if */

    d.template_name = fn;
    d.output_name = p;
    n = snprintf(p, buffer_sz, tpl, d.module_name);
    buffer_sz -= n+1; p += n+1;

    time(&ts);
    {
        char *s, *s2 = strdup(get_config(
                    "BNF_INCLUDES",
                    ".:/usr/share/bnf/include"));
        d.command = p;
        n = snprintf(p, buffer_sz, m4_binary);
        buffer_sz -= n; p += n;
        for (s = strtok(s2, ":"); s; s = strtok(NULL, ":")) {
            n = snprintf(p, buffer_sz, " -I%s", s);
            buffer_sz -= n; p += n;
        } /* for */
        n = snprintf(p, buffer_sz, " >%s", d.output_name);
        buffer_sz -= n+1; p += n+1;
        free(s2);
    } /* block */

#if DEBUG
    printf("%s:%d: %s\n", __FILE__, __LINE__, d.command);
#endif

    d.file = popen(d.command, "w");
    if (!d.file) {
        fprintf(stderr,
                "%s:%d: ERROR: %s: %s(errno=%d)\n",
                __FILE__, __LINE__, fn,
                strerror(errno), errno);
        return;
    } /* if */

    fprintf(d.file, "dnl $Id: pass3.c,v 1.14 2012/09/22 22:51:55 luis Exp $\n");
    fprintf(d.file, "dnl Author: Luis Colorado <lc@luiscoloradosistemas.com>\n");
    fprintf(d.file, "dnl Date: Sun Sep 16 17:13:17 CEST 2012\n");
    fprintf(d.file, "dnl Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.\n");
    fprintf(d.file, "dnl                      All rights reserved.\n");
    fprintf(d.file, "define(`date',    `%s')dnl\n", strtok(ctime(&ts), "\n"));
    fprintf(d.file, "define(`source',  `%s')dnl\n", filename);
    fprintf(d.file, "define(`template',`%s')dnl\n", d.template_name);
    fprintf(d.file, "define(`output',  `%s')dnl\n", d.output_name);
    fprintf(d.file, "define(`module',  `%s')dnl\n", d.module_name);
    fprintf(d.file, "define(`command', `%s')dnl\n", d.command);
    fprintf(d.file, "include(`%s')dnl\n", d.template_name);
    fprint_config_m4(d.file);

    pass_grammar(PASS, parsed_grammar, pass3_pre, pass3_post, &d);

    pclose(d.file);

} /* pass3 */

static void pass3_pre(
    int                             passno,
    enum pass_type                  ptyp,
    enum syntax_node_type           ntyp,
    union syntax_node               node,
    void*                           arg)
{
    struct rule* rul;
    struct term* trm;
    struct datos *dp = arg;

    switch(ntyp) {
    case T_GRAMMAR:
        fprintf(dp->file, "%*sgrammar_begin(`%s')\n",
            dp->indent, "",
            main_rule->ident->str);
        dp->indent += dp->increment;
        break;
    case T_RULE: rul = node.rule;
        fprintf(dp->file, "\n%*srule_begin(`%d',`%s')\n",
                dp->indent, "",
                rul->rule_id,
                rul->ident->str); 
        dp->indent += dp->increment;
        break;
    case T_ALTERNATIVE:
        if (node.alternative->term_list) {
            fprintf(dp->file, "%*salternative_begin(`%d')\n",
                    dp->indent, "",
                    node.alternative->alt_id);
            dp->indent += dp->increment;
        } else {
            fprintf(dp->file, "%*salternative(`%d')\n",
                    dp->indent, "",
                    node.alternative->alt_id);
        } /* if */
        break;
    case T_TERM:
        trm  = node.term;
        switch(trm->_alt) {
        case 1: /* IDENT */
            fprintf(dp->file, "%*s%s(`%s')\n",
                dp->indent, "",
                trm->_alt_1.definition
                    ? "nonterminal"
                    : "terminal",
                trm->_alt_1.ident->str);
            break;
        case 2: { /* STRING */
                    char *alt;
            fprintf(dp->file, "%*sliteral(",
                dp->indent, ""); 

            if ((alt = avl_tree_get(
                            symbol2token,
                            trm->_alt_2.string->str))
                    != NULL)
            {
                fprintf(dp->file, "`%s',",
                        alt);
            } /* if */
            fprintf(dp->file,
                    "`%s')\n",
                    strdequote(trm->_alt_2.string->str));
                } /* bloque */
            break;
        case 3: /* [ alternative_list ] */
            fprintf(dp->file, "%*soptional_begin()\n",
                    dp->indent, "");
            dp->indent += dp->increment;
            break;
        case 4: /* { alternative_list } */
            fprintf(dp->file, "%*ssequence_begin()\n",
                    dp->indent, "");
            dp->indent += dp->increment;
            break;
        case 5: /* ( alternative_list ) */
            fprintf(dp->file, "%*ssubexpression_begin()\n",
                    dp->indent, "");
            dp->indent += dp->increment;
            break;
        } /* switch */
        break;
    } /* switch */
} /* pass3_pre */

static void pass3_post(
    int                             passno,
    enum pass_type                  ptyp,
    enum syntax_node_type           ntyp,
    union syntax_node               node,
    void*                           arg)
{
    struct datos* dp = arg;
    struct rule* rul;

    switch(ntyp) {
    case T_GRAMMAR:
        dp->indent -= dp->increment;
        fprintf(dp->file, "\n%*sgrammar_end(`%s')\n",
            dp->indent, "",
            main_rule->ident->str);
        break;
    case T_RULE: rul = node.rule;
        dp->indent -= dp->increment;
        fprintf(dp->file, "%*srule_end(`%d',`%s')\n",
            dp->indent, "",
            rul->rule_id,
            rul->ident->str);
        break;
    case T_ALTERNATIVE:
        if (node.alternative->term_list) {
            dp->indent -= dp->increment;
            fprintf(dp->file, "%*salternative_end(`%d')\n",
                dp->indent, "",
                node.alternative->alt_id);
        } /* if */
        break;
    case T_TERM:
        switch(node.term->_alt) {
        case 3: /* [ alternative_list ] */
            dp->indent -= dp->increment;
            fprintf(dp->file, "%*soptional_end()\n",
                    dp->indent, "");
            break;
        case 4: /* { alternative_list } */
            dp->indent -= dp->increment;
            fprintf(dp->file, "%*ssequence_end()\n",
                    dp->indent, "");
            break;
        case 5: /* ( alternative_list ) */
            dp->indent -= dp->increment;
            fprintf(dp->file, "%*ssubexpression_end()\n",
                    dp->indent, "");
            break;
        } /* switch */
        break;
    } /* if */
} /* pass3_post */

static char* strdequote(
        char*                           a)
{
    static char buffer[128];
    char *b;
    for (b = buffer; *a; a++) {
        if (strchr("\'\"", *a)) {
            a++;
            if (!*a) break;
        } /* if */
        switch(*a) {
            int car;
        case '\'': b += sprintf(b, "'rq()`"); break;
        case '`': b += sprintf(b, "'lq()`"); break;
        default: *b++ = *a;
        } /* switch */
    } /* while */
    *b = '\0';

    return buffer;
} /* strdequote */

/* $Id: pass3.c,v 1.14 2012/09/22 22:51:55 luis Exp $ */
