/* $Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $
 * vim: sw=4 ts=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: mié jul 18 12:40:33 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#ifndef BNF_TABSIM_H
#define BNF_TABSIM_H

#include "avl.h"
#include "lists.h"

#include "tokeninfo.h"

static char BNF_SYMTAB_H_RCSId[] = "\n$Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $\n";

extern AVL_TREE
    tab_noterminales,
    tab_terminales,
    tab_strings,
    token2symbol,
    symbol2token,
    tab_subtrees;

extern struct grammar*  parsed_grammar;

struct grammar;
struct def_list;
struct def;
struct rule_list;
struct rule;
struct alternative_list;
struct alternative;
struct term_list;
struct term;

/******** struct grammar ******************************/
struct grammar {
    int                                 ref_count;
    char*                               key;
    struct def_list*                    def_list;
    struct rule_list*                   rule_list;
    LNODE_T                             rul_lst;    
}; /* struct grammar */

struct grammar* new_grammar(
        struct rule_list*               p2);

/******** struct def_list *****************************/
struct def_list {
    int                                 ref_count;
    char*                               key;
    int                                 _alt;
    union {
        struct {
            struct def_list*            def_list;
            struct def*                 def;
        }_alt_1;
    };
}; /* struct def_list */

struct def_list* new_def_list_1(
        struct def_list*                p1,
        struct def*                     p2);

struct def_list* new_def_list_2();

/******** struct def **********************************/
struct def {
    int                                 ref_count;
    char*                               key;
    int                                 _alt;
    union {
        struct {
            struct tokeninfo*           ident;
            struct tokeninfo*           string;
        }_alt_1;
        struct {
            struct tokeninfo*           ident;
        }_alt_2;
        struct {
            struct tokeninfo*           ident;
        }_alt_3;
    };
}; /* struct def */

struct def* new_def_1(
        struct tokeninfo*               p1,
        struct tokeninfo*               p2);
struct def* new_def_2(
        struct tokeninfo*               p1);
struct def* new_def_3(
        struct tokeninfo*               p1);

/******** struct rule_list ***************************/
struct rule_list {
    int                                 ref_count;
    char*                               key;
    int                                 _alt;
    union {
        struct {
            struct rule_list*           rule_list;
            struct rule*                rule;
        }_alt_1;
        struct {
            struct rule*                rule;
        }_alt_2;
    };
};

struct rule_list* new_rule_list_1(
        struct rule_list*               p1,
        struct rule*                    p2);
struct rule_list* new_rule_list_2(
        struct rule*                    p1);

/******** struct rule *********************************/

#define RULE_FLAG_SINT                  1
struct rule {
    int                                 ref_count;
    char*                               key;
    int                                 lin;
    struct tokeninfo*                   ident;
    struct alternative_list*            alternative_list;

    int                                 flags;
    int                                 rule_id;
    LNODE_T                             alt_lst;
    LNODE_T                             node_rul_lst;
};

struct rule* new_rule(
        struct tokeninfo*               p1,
        struct alternative_list*        p2);

extern struct rule*                     main_rule;

/******** struct alternative_list ********************/
struct alternative_list {
    int                                 ref_count;
    char*                               key;
    int                                 _alt;
    union {
        struct {
            struct alternative_list*    alternative_list;
            struct alternative*         alternative;
        }_alt_1;
        struct {
            struct alternative*         alternative;
        }_alt_2;
    };
};

struct alternative_list* new_alternative_list_1(
        struct alternative_list*        p1,
        struct alternative*             p2);
struct alternative_list* new_alternative_list_2(
        struct alternative*             p1);

/******** struct alternative **************************/

struct alternative {
    int                                 ref_count;
    char*                               key;
    struct term_list*                   term_list;
    int                                 alt_id;

    LNODE_T                             trm_lst; /* term list */
    LNODE_T                             ntrm_lst; /* term/noterm list */
    LNODE_T                             node_alt_lst;
};

struct alternative* new_alternative(
        struct term_list*               p1);

/******** struct term_list ****************************/
struct term_list {
    int                                 ref_count;
    char*                               key;
    int                                 _alt;
    union {
        struct {
            struct term_list*           term_list;
            struct term*                term;
        }_alt_1;
    };
};

struct term_list* new_term_list_1(
        struct term_list*               p1,
        struct term*                    p2);
struct term_list* new_term_list_2();

/******** struct term *********************************/
struct term {
    int                                 ref_count;
    char*                               key;
    int                                 _alt;
    union {
        struct {
            struct tokeninfo*           ident;
            struct rule*                definition;
        }_alt_1;
        struct {
            struct tokeninfo*           string;
        }_alt_2;
        struct {
            struct alternative_list*    alternative_list;
        }_alt_3;
        struct {
            struct alternative_list*    alternative_list;
        }_alt_4;
        struct {
            struct alternative_list*    alternative_list;
        }_alt_5;
    };
    LNODE_T                             node_ntrm_lst;
    LNODE_T                             node_trm_lst;
}; /* struct term */

struct term* new_term_1(
        struct tokeninfo*               p1);
struct term* new_term_2(
        struct tokeninfo*               p1);
struct term* new_term_3(
        struct alternative_list*        p1);
struct term* new_term_4(
        struct alternative_list*        p1);
struct term* new_term_5(
        struct alternative_list*        p1);

/******************************************************/

enum pass_type {
    PREORDER,
    POSTORDER,
};

enum syntax_node_type {
    T_GRAMMAR,
    T_DEF_LIST,
    T_DEF,
    T_RULE_LIST,
    T_RULE,
    T_ALTERNATIVE_LIST,
    T_ALTERNATIVE,
    T_TERM_LIST,
    T_TERM,
};

union syntax_node {
    struct grammar*                     grammar;
    struct def_list*                    def_list;
    struct def*                         def;
    struct rule_list*                   rule_list;
    struct rule*                        rule;
    struct alternative_list*            alternative_list;
    struct alternative*                 alternative;
    struct term_list*                   term_list;
    struct term*                        term;
};

typedef void(*callback)(
        int                             passno,
        enum pass_type                  typ,
        enum syntax_node_type           node_typ,
        union syntax_node               node,
        void*                           arg);

void pass_grammar(
        int                             passno,
        struct grammar*                 p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_def_list(
        int                             passno,
        struct def_list*                p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_def(
        int                             passno,
        struct def*                     p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_rule_list(
        int                             passno,
        struct rule_list*               p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_rule(
        int                             passno,
        struct rule*                    p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_alternative_list(
        int                             passno,
        struct alternative_list*        p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_alternative(
        int                             passno,
        struct alternative*             p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_term_list(
        int                             passno,
        struct term_list*               p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void pass_term(
        int                             passno,
        struct term*                    p,
        callback                        f_pre,
        callback                        f_post,
        void*                           arg);

void do_yacc_nocolor(void);

#endif /* BNF_TABSIM_H */
/* $Id: bnf_symtab.h,v 1.12 2012/09/15 19:57:32 luis Exp $ */
