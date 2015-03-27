/* $Id: bnf_main.c,v 1.24 2012/10/05 11:10:55 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié jul 18 14:43:30 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#ifndef FALSE
#define FALSE                               0
#define TRUE                                (!FALSE)
#endif

#include "bnf.h"
#include "avl.h"
#include "bnf_symtab.h"
#include "bnf_sparser.h"
#include "bnf_main.h"
#include "pass1.h"
#include "print_symbols.h"
#include "print_strtable.h"

static char BNF_MAIN_C_CVS_ID[] =
"\n$Id: bnf_main.c,v 1.24 2012/10/05 11:10:55 luis Exp $\n";

int flags = 0;

char *filename = "stdin";
char *xml_template_filename = "tmpl_xml.m4";
char *XML_template_filename = "tmpl_xml.m4";
char *HEADER_template_filename = "tmpl_h.m4";
char *CMODULE_template_filename = "tmpl_c.m4";
char *GENYACC_template_filename = "tmpl_y.m4";
char *m4_binary = "/usr/bin/m4";

char copyright[] = 
" * $Id: bnf_main.c,v 1.24 2012/10/05 11:10:55 luis Exp $\n"
" * Title: BNF\n"
" * Version: 0.51\n"
" * Author: Luis Colorado <lc@luiscoloradosistemas.com>\n"
" * Date: mié jul 18 14:43:30 CEST 2012\n"
" * Disclaimer: (C) 2012-2015 LUIS COLORADO SISTEMAS S.L.U.\n"
" *             All rights reserved.\n";

int main(int argc, char **argv)
{
    int opt, res;

    while ((opt = getopt(argc, argv, "vd:ntCm:""x::X::h::c::y::")) != EOF) {
        switch(opt) {
            char *s;
        case 'v': printf("%s", copyright); return EXIT_SUCCESS;
        case 'd': for (s = optarg; *s; s++) {
                      switch(*s) {
                      case 's': flags ^= FLAG_SCANNER; break;
                      case 'p': flags ^= FLAG_PARSER; break;
                      default:
                                fprintf(stderr,
                                        D("unknown debug flag: %c\n"),
                                        *s);
                                break;
                      } /* switch */
                  } /* for */
                  break;
        case 'n': flags ^= FLAG_NOTER_LIST; break;
        case 't': flags ^= FLAG_TER_LIST; break;
        case 'C': flags ^= FLAG_NOCOLOR; break;
        case 'm': m4_binary = optarg; break;
        case 'x': flags ^= FLAG_xml;
                  if (optarg)
                      xml_template_filename = optarg;
                  break;
        case 'X': flags ^= FLAG_XML;
                  if (optarg)
                      XML_template_filename = optarg;
                  break;
        case 'h': flags ^= FLAG_HEADER;
                  if (optarg)
                      HEADER_template_filename = optarg;
                  break;
        case 'c': flags ^= FLAG_CMODULE;
                  if (optarg)
                      CMODULE_template_filename = optarg;
                  break;
        case 'y': flags ^= FLAG_GENYACC;
                  if (optarg)
                      GENYACC_template_filename = optarg;
                  break;
        } /* switch */
    } /* while */

    argc -= optind; argv += optind;

    if (argc) {
        FILE* f = fopen(argv[0], "r");
        filename = argv[0];
        if (!f) {
            fprintf(stderr,
                    "Error: %s: %s(errno = %d)\n",
                    filename, strerror(errno), errno);
            exit(EXIT_FAILURE);
        } /* if */
        yyrestart(f);
    } /* if */

    do_lex_init();

    if (flags & FLAG_NOCOLOR) {
        do_lex_nocolor();
        do_yacc_nocolor();
    } /* if */

    res = yyparse();


#if 0
    xref_tokeninfo(tokeninfo_db, stdout, filename);
    if (res == 0) {
        AVL_ITERATOR p;

        fprintf(stderr, "SINTAXIS CORRECTA\n");

        /* LA SIGUIENTE PASADA IDENTIFICA LOS SÍMBOLOS
         * TERMINALES Y LOS NO TERMINALES.  LOS TERMINALES
         * SON AQUELLOS QUE NO APARECEN EN EL LADO IZQUIERDO
         * DE UNA PRODUCCIÓN.  LOS NO TERMINALES APARECEN
         * EN EL LADO IZQUIERDO DE UNA PRODUCCIÓN.
         */
        pass0();

        /* LA SIGUIENTE PASADA IMPRIME EN FORMATO XML LAS
         * REGLAS INTRODUCIDAS.  ÉSTAS APARECEN TAL Y COMO HAN
         * SIDO INTRODUCIDAS, SIN NINGÚN TIPO DE PROCESAMIENTO
         * INTERNO.  */
        if (flags & FLAG_xml)
            pass3(  xml_template_filename,
                    "%s_1.xml");

        /* LA SIGUIENTE PASADA REALIZA LA SEPARACIÓN DE
         * SUBEXPRESIONES DE LA FORMA ( ... | ... ),
         * [ ... | ... ] Y { ... | ... } EN DEFINICIONES
         * DE NUEVAS REGLAS QUE NO LAS INCLUYAN PARA DEJAR
         * LA GRAMÁTICA COMO UNA GRAMÁTICA EQUIVALENTE EN FORMATO
         * BNF CANÓNICO.  */
        pass1();

        /* LA SIGUIENTE PASADA EXPLORA DESDE EL SÍMBOLO INICIAL
         * TODOS LOS SÍMBOLOS ALCANZABLES Y SEPARA (E IMPRIME)
         * LOS SÍMBOLOS NO TERMINALES (QUE APARECEN EN EL LADO 
         * IZQUIERDO DE UNA PRODUCCIÓN) QUE NO SON ALCANZABLES
         * DESDE EL SÍMBOLO INICIAL.  ESTOS SÍMBOLOS SON ELIMINA-
         * BLES DIRECTAMENTE DE LA GRAMÁTICA, PUES NO SON ALCANZA-
         * BLES DESDE EL SÍMBOLO INICIAL DE LA MISMA. EL PROGRAMA
         * PRODUCE UN WARNING POR CADA UNO DE ESTOS SÍMBOLOS NO
         * TERMINALES. */
        pass2();

        /* SE IMPRIME NUEVAMENTE LA GRÁMATICA EN FORMATO XML PERO
         * ESTA VEZ REALIZADA LA TRANFORMACIÓN EN UNA GRAMÁTICA
         * BNF CANÓNICA (sin paréntesis). */
        if (flags & FLAG_XML)
            pass3(  XML_template_filename,
                    "%s_2.xml");

        /* SE IMPRIMEN FICHEROS FUENTES NECESARIOS PARA IMPLEMENTAR
         * LA GRAMÁTICA PEDIDA. */
        if (flags & FLAG_HEADER)
            pass3(  HEADER_template_filename,
                    "%s_tabsim.h");
        if (flags & FLAG_CMODULE)
            pass3(  CMODULE_template_filename,
                    "%s_tabsim.c");
        if (flags & FLAG_GENYACC)
            pass3(  GENYACC_template_filename,
                    "%s_parser.y");

        //pass_grammar(3, $$, pass15_pre, pass15_post);
        //pass_grammar(2, $$, pass2_pre, NULL);
#if 0
        print_tabsym(token2symbol, "TOKEN2SYMBOL table (%d entries)\n",
                avl_tree_size(token2symbol));
        print_tabsym(symbol2token, "SYMBOL2TOKEN table (%d entries)\n",
                avl_tree_size(symbol2token));
        print_symbols(tab_subtrees,
                flags & FLAG_NOCOLOR
                    ? "%s%s"
                    : "\e[36m%s\e[0m%s");
#endif
    } /* if */
#endif
} /* main */

/* $Id: bnf_main.c,v 1.24 2012/10/05 11:10:55 luis Exp $ */
