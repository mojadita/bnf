/* $Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $
 * vim: sw=4 ts=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié jul 18 12:40:33 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */
#ifndef BNF_SYMTAB_H
#define BNF_SYMTAB_H

#include <avl.h>
#include "lists.h"

#include "tokeninfo.h"

static char BNF_SYMTAB_H_RCSId[] = "\n$Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $\n";

enum bnf_alt {
    BNF_NONE,
    BNF_GRAMMAR_ALT_1,
    BNF_GRAMMAR_ALT_2,
    BNF_RULE_ALT_1,
    BNF_ALTERNATIVE_LIST_ALT_1,
    BNF_ALTERNATIVE_LIST_ALT_2,
    BNF_TERM_LIST_ALT_1,
    BNF_TERM_LIST_ALT_2,
    BNF_TERM_ALT_1,
    BNF_TERM_ALT_2,
    BNF_TERM_ALT_3,
    BNF_TERM_ALT_4,
    BNF_TERM_ALT_5,
};


/******** struct grammar ******************************/
struct grammar {
    enum bnf_alt                    typ;
    union {
        struct {
            struct grammar          *e_1;
            struct rule             *e_2;
        }                           _1;
        struct {
            struct rule             *e_1;
        }                           _2;
    }                               alt;
}; /* struct grammar */

struct grammar* new_grammar_1(
        struct grammar              *_1,
        struct rule                 *_2);

struct grammar *new_grammar_2(
        struct rule                 *_1);

/******** struct rule *****************************/
struct rule {
    enum bnf_alt                    typ;
    union {
        struct {
            struct ti_xref          *e_1; /* IDENT */
            struct ti_xref          *e_2; /* '::=' */
            struct alternative_list *e_3; /* alternative_list */
            struct ti_xref          *e_4; /* '.' */
        }                           _1;
    }                               alt;
}; /* struct def_list */

struct rule *new_rule_1(
        struct ti_xref              *_1, /* IDENT */
        struct ti_xref              *_2, /* '::=' */
        struct alternative_list     *_3, /* alternative_list */
        struct ti_xref              *_4); /* '.' */

/******** struct alternative_list **********************************/
struct alternative_list {
    enum bnf_alt                    typ;
    union {
        struct {
            struct alternative_list *e_1;
            struct ti_xref          *e_2;
            struct term_list        *e_3;
        }                           _1;
        struct {
            struct term_list        *e_1;
        }                           _2;
    }                               alt;
};

struct alternative_list *new_alternative_list_1(
        struct alternative_list     *_1,
        struct ti_xref              *_2,
        struct term_list            *_3);

struct alternative_list *new_alternative_list_2(
        struct term_list            *_1);

/******** struct term_list ***************************/
struct term_list {
    enum bnf_alt                    typ;
    union {
        struct {
            struct term_list        *e_1;
            struct term             *e_2;
        }                           _1;
    }                               alt;
};

struct term_list* new_term_list_1(
        struct term_list            *_1,
        struct term                 *_2);

struct term_list* new_term_list_2(void);

/******** struct term *********************************/

struct term {
    enum bnf_alt                    typ;
    union {
        struct {
            struct ti_xref          *e_1;
        }                           _1;
        struct {
            struct ti_xref          *e_1;
        }                           _2;
        struct {
            struct ti_xref          *e_1;
            struct alternative_list *e_2;
            struct ti_xref          *e_3;
        }                           _3;
        struct {
            struct ti_xref          *e_1;
            struct alternative_list *e_2;
            struct ti_xref          *e_3;
        }                           _4;
        struct {
            struct ti_xref          *e_1;
            struct alternative_list *e_2;
            struct ti_xref          *e_3;
        }                           _5;
    }                               alt;
};

struct term *new_term_1(
        struct ti_xref              *_1);

struct term *new_term_2(
        struct ti_xref              *_1);

struct term *new_term_3(
        struct ti_xref              *_1,
        struct alternative_list     *_2,
        struct ti_xref              *_3);

struct term *new_term_4(
        struct ti_xref              *_1,
        struct alternative_list     *_2,
        struct ti_xref              *_3);

struct term *new_term_5(
        struct ti_xref              *_1,
        struct alternative_list     *_2,
        struct ti_xref              *_3);

#endif /* BNF_SYMTAB_H */
/* $Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $ */
