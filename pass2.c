/* $Id: pass2.c,v 1.10 2012/09/15 19:57:33 luis Exp $
 * vim: ts=4 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sun Jul 22 17:58:22 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *             All rights reserved.
 */

#ifndef FALSE
#define FALSE			0
#define TRUE			1
#endif

#ifndef DEBUG
#define DEBUG			FALSE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tokeninfo.h"
#include "avl.h"
#include "bnf_main.h"
#include "bnf_tabsim.h"
#include "pass1.h"

static char PASS2_C_RCSId[] = "\n$Id: pass2.c,v 1.10 2012/09/15 19:57:33 luis Exp $\n";

#define PASS						2

struct node {
	struct rule*					r;
	LNODE_T							node;
};

static void pass2_cb(
		int							pass,
		enum pass_type				ptyp,
		enum syntax_node_type		ntyp,
		union syntax_node			node,
		void* 						arg); 

AVL_TREE noterm_alcanzables =		NULL;
AVL_TREE noterm_noalcanzables =		NULL;

LIST_DECLARE(queue);
#define INDENT						4
int									indent = 0;

void pass2(void)
{
	int								pass = 0;

	noterm_alcanzables = new_avl_tree(strcmp);
	noterm_noalcanzables = new_avl_tree(strcmp);
	
#if DEBUG
	printf("PASS#%d: IDENTIFICACION NOTERMINALES DESLIGADOS.\n",
			PASS);
#endif

	/* INSERTAMOS EN LA COLA LA PRIMERA REGLA. */
	{ /* block */
		struct node*	n;
		assert(n = malloc(sizeof(struct node)));
		n->r = main_rule;
		LIST_APPEND(&queue, &n->node); /* INSERTAMOS */
#if DEBUG
		printf("PASS#%d.%03d: %*s>>> %s\n",
				PASS, pass, indent , "", main_rule->ident->str);
#endif
		avl_tree_put( /* LO INTRODUCIMOS EN LA LISTA DE ALCANZABLES */
				noterm_alcanzables,
				main_rule->ident->str,
				main_rule); /* LA INFORMACI?N DE LA REGLA PRINCIPAL	*/
	} /* block */

	/* MIENTRAS HAYA ENTRADAS EN LA COLA */
	while (!LIST_EMPTY(&queue)) {
		struct node* n = LIST_ELEMENT( /* REFERENCIA AL PRIMER ELEMENTO */
				LIST_FIRST(&queue), struct node, node);

		LIST_DELETE(&n->node); /* DESENCOLAMOS */
#if DEBUG
		printf("PASS#%d.%03d: %*s<<< %s\n",
				PASS, pass, indent, "", n->r->ident->str);
#endif
		/* EFECTUAMOS PASADA SOBRE LA REGLA ENCONTRADA. */
		pass_rule(pass++, n->r, pass2_cb, pass2_cb, NULL);

		free(n); /* Y BORRAMOS PARA SIEMPRE ;) */
	} /* while */

	/* ELIMINAMOS DE LA LISTA DE NOTERMINALES AQUELLOS QUE SON ALCANZABLES
	 * PARA ELLO, CONSTRUIMOS UNA TABLA CON LA DIFERENCIA ENTRE LOS
	 * NO TERMINALES Y LOS NO TERMINALES ALCANZABLES. */
	{ /* block */
		AVL_ITERATOR p;
		for (p = avl_tree_first(tab_noterminales); p; p = avl_iterator_next(p))
		{
			char *k = avl_iterator_key(p);
			/* si no est? en la lista de alcanzables, lo a?adimos a la lista
			 * de no alcanzables. */
			if (!avl_tree_get(noterm_alcanzables, k)) {
				/* METEMOS LA INFORMACI?N SOBRE LA REGLA QUE DEFINE EL NO
				 * TERMINAL. */
				avl_tree_put(noterm_noalcanzables, k,
						avl_tree_get(tab_noterminales, k));
			} /* if */
		} /* for */
	} /* block */

	/* SI HAY NO TERMINALES NO ALCANZABLES, IMPRIMIMOS UN WARNING POR CADA UNO
	 * CON LA POSICI?N DONDE EST? DEFINIDO. */
	if (avl_tree_size(noterm_noalcanzables)) {
		AVL_ITERATOR p;
		for (p = avl_tree_first(noterm_noalcanzables);
				p; p = avl_iterator_next(p))
		{
			struct rule* r = avl_iterator_data(p);
			fprintf(stderr, "%s:%d:%d: warning: [%s] unreachable.\n",
					filename, r->ident->lin, r->ident->col,
					r->ident->str);
		} /* for */
	} /* if */
} /* pass2 */

static void pass2_cb(
		int							pass, /* solo usado si DEBUG */
		enum pass_type				ptyp, /* no usado */
		enum syntax_node_type		ntyp,
		union syntax_node			node,
		void*						arg)
{
	switch(ptyp) {
	case PREORDER:
		switch(ntyp) {
#if DEBUG
		case T_RULE:
			printf("PASS#%d.%03d: %*sBEG %s\n",
					PASS, pass, indent, "", node.rule->ident->str);
			indent += INDENT;
			break;
#endif
		case T_TERM: { /* block */
			struct rule* r;
			/* SI EL TERMINAL ES UN IDENTIFICADOR Y ADEM\'AS TIENE UNA
			 * DEFINICI\'ON (ES UN NOTERMINAL) Y ADEM\'AS NO EST\'A EN
			 * LA LISTA DE NOTERMINALES ALCANZABLES... ENTONCES LO ME-
			 * TEMOS EN LA LISTA Y A\~NADIMOS UN REGISTRO A LA COLA
			 * PARA QUE SE PROCESE RECURS\'IVAMENTE, MAS TARDE. */
			if (   (node.term->_alt == 1)
				&& (r = node.term->_alt_1.definition)
				&& (!avl_tree_get(noterm_alcanzables, r->ident->str)))
			{
				struct node* n;

				/* METEMOS EN LA TABLA noterm_alcanzables LA DEFINICI?N DE LA
				 * REGLA QUE DEFINE ESTE IDENTIFICADOR */
				avl_tree_put(noterm_alcanzables, r->ident->str, r);
				/* CREAMOS EL NODO QUE METEREMOS EN LA COLA */
				assert(n = malloc(sizeof(struct node)));
				n->r = r; /* INICIALIZAMOS */
				LIST_APPEND(&queue, &n->node); /* A\~NADIMOS A LA COLA */
#if DEBUG
				printf("PASS#%d.%03d: %*s>>> %s\n",
						PASS, pass, indent, "", r->ident->str);
#endif
			} /* if */
					 } /* block */
			break;
		} /* switch */
		break;
	case POSTORDER:
		switch(ntyp) {
#if DEBUG
		case T_RULE:
			indent -= INDENT;
			printf("PASS#%d.%03d: %*sEND %s\n",
					PASS, pass, indent, "", node.rule->ident->str);
			break;
#endif
		} /* switch */
	} /* switch */
} /* pass2_cb */

/* $Id: pass2.c,v 1.10 2012/09/15 19:57:33 luis Exp $ */
