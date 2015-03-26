/* $Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $
 * vim: sw=4 ts=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié jul 18 12:40:33 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#include <malloc.h>
#include <assert.h>
#include "bnf.h"
#include "bnf_symtab.h"

char BNF_SYMTAB_C_RCSId[] = "\n$Id: bnf_symtab.c,v 1.12 2012/09/15 19:57:32 luis Exp $\n";

/******** struct grammar **************************/
struct grammar* new_grammar_1(
        struct grammar              *_1,
        struct rule                 *_2)
{
    struct grammar *res;

    assert(res = malloc(sizeof (struct grammar)));

    res->typ = BNF_GRAMMAR_ALT_1;
    res->alt._1.e_1 = _1;
    res->alt._1.e_2 = _2;

    return res;
} /* new_grammar_1 */

struct grammar *new_grammar_2(
        struct rule                 *_1)
{
    struct grammar *res;

    assert(res = malloc(sizeof (struct grammar)));

    res->typ = BNF_GRAMMAR_ALT_2;
    res->alt._2.e_1 = _1;

    return res;
} /* new_grammar_2 */

/******** struct rule *****************************/
struct rule *new_rule_1(
        struct ti_xref              *_1,
        struct ti_xref              *_2,
        struct alternative_list     *_3,
        struct ti_xref              *_4)
{
    struct rule *res;

    assert(res = malloc(sizeof (struct rule)));
    
    res->typ = BNF_RULE_ALT_1;
    res->alt._1.e_1 = _1;
    res->alt._1.e_2 = _2;
    res->alt._1.e_3 = _3;
    res->alt._1.e_4 = _4;

    return res;
} /* new_rule_1 */

/******** struct alternative_list *****************/

struct alternative_list *new_alternative_list_1(
        struct alternative_list     *_1,
        struct ti_xref              *_2,
        struct term_list            *_3)
{
    struct alternative_list *res;

    assert(res = malloc(sizeof (struct alternative_list)));

    res->typ = BNF_ALTERNATIVE_LIST_ALT_1;
    res->alt._1.e_1 = _1;
    res->alt._1.e_2 = _2;
    res->alt._1.e_3 = _3;

    return res;
} /* new_alternative_list_1 */

struct alternative_list *new_alternative_list_2(
        struct term_list            *_1)
{
    struct alternative_list *res;

    assert(res = malloc(sizeof (struct alternative_list)));

    res->typ = BNF_ALTERNATIVE_LIST_ALT_2;
    res->alt._2.e_1 = _1;

    return res;
} /* new_alternative_list_2 */

/******** struct term_list ************************/

struct term_list* new_term_list_1(
        struct term_list            *_1,
        struct term                 *_2)
{
    struct term_list *res;

    assert(res = malloc(sizeof (struct term_list)));

    res->typ = BNF_TERM_LIST_ALT_1;
    res->alt._1.e_1 = _1;
    res->alt._1.e_2 = _2;

    return res;
} /* new_term_list_1 */

struct term_list* new_term_list_2(void)
{
    struct term_list *res;

    assert(res = malloc(sizeof (struct term_list)));

    res->typ = BNF_TERM_LIST_ALT_2;

    return res;
} /* new_term_list_2 */

/******** struct term *****************************/

struct term *new_term_1(
        struct ti_xref              *_1)
{
    struct term *res;

    assert(res = malloc(sizeof (struct term)));

    res->typ = BNF_TERM_ALT_1;
    res->alt._1.e_1 = _1;

    return res;
} /* new_term_1 */

struct term *new_term_2(
        struct ti_xref              *_1)
{
    struct term *res;

    assert(res = malloc(sizeof (struct term)));

    res->typ = BNF_TERM_ALT_2;
    res->alt._2.e_1 = _1;

    return res;
} /* new_term_2 */

struct term *new_term_3(
        struct ti_xref              *_1,
        struct alternative_list     *_2,
        struct ti_xref              *_3)
{
    struct term *res;
    
    assert(res = malloc(sizeof (struct term)));

    res->typ = BNF_TERM_ALT_3;
    res->alt._3.e_1 = _1;
    res->alt._3.e_2 = _2;
    res->alt._3.e_3 = _3;

    return res;
} /* new_term_3 */

struct term *new_term_4(
        struct ti_xref              *_1,
        struct alternative_list     *_2,
        struct ti_xref              *_3)
{
    struct term *res;

    assert(res = malloc(sizeof (struct term)));

    res->typ = BNF_TERM_ALT_4;
    res->alt._4.e_1 = _1;
    res->alt._4.e_2 = _2;
    res->alt._4.e_3 = _3;

    return res;
} /* new_term_4 */

struct term *new_term_5(
        struct ti_xref              *_1,
        struct alternative_list     *_2,
        struct ti_xref              *_3)
{
    struct term *res;

    assert(res = malloc(sizeof (struct term)));

    res->typ = BNF_TERM_ALT_5;
    res->alt._5.e_1 = _1;
    res->alt._5.e_2 = _2;
    res->alt._5.e_3 = _3;

    return res;
} /* new_term_5 */

/* $Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $ */
