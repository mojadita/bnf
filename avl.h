/* $Id: avl.h,v 1.1.1.1 2012/08/20 19:25:31 luis Exp $
 * vim: ts=4 sw=4 nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Tue Aug  4 20:23:01     2009
 *
 * Disclaimer:
 *  (C) 2009-2012 LUIS COLORADO SISTEMAS S.L.
 *  All rights reserved.  No part of this code can be distributed in
 *  any form, withour previous written consent of the copyright owner.
 *  This code is distributed WITHOUT ANY WARRANTY, even the implied
 *  WARRANTY OF MERCHANTABILITY or FITNESS FOR ANY PARTICULAR PURPOSE.
 *  In case of an agreed previous, written contract, the WARRANTIES
 *  applied will be the ones written in the applicable contract.
 *
 * Nota: Adaptado a C en fecha Sat Apr  7 12:58:14 CEST 2012  a fin de
 * ===== poder ser utilizado en parsers escritos con flex/bison.
 */

/* Do not include anything BEFORE the line below, as it would not be
 * protected against double inclusion from other files
 */
#ifndef AVL_H
#define AVL_H

#include <stdio.h> /* por FILE, etc. */

static char AVL_H_RCSId[] = "\n$Id: avl.h,v 1.1.1.1 2012/08/20 19:25:31 luis Exp $\n";

/* types */
/* AVL_MT Match type.  Allows to select the match type for a key search.
 * This type is passed as a parameter to the search engine, so the iterator
 * obtained matches the search criteria. */
typedef enum {
	MT_L, /* less */
	MT_LE, /* less than or equal */
	MT_EQ, /* equal */
	MT_GE, /* greater than or equal */
	MT_G, /* greater than */
} AVL_MT;

/* the next type is for a pointer to compare strings, so we can use
 * different ordering functions in tree maps.  It's used in the tree
 * constructor. */
typedef int (*AVL_FCOMP)(const char *s1, const char *s2);

/* the iterator type, incomplete opaque type */
typedef struct avl_node *AVL_ITERATOR;

/* the AVL_TREE type, incomplete opaque type */
typedef struct avl_tree *AVL_TREE;

/* prototypes */

/* AVL_TREE constructor.  As we don't have Object Oriented support in C,
 * we use the next function to create an object of type AVL_TREE.  Note
 * that AVL_TREE is only a reference pointer to an opaque object.  No
 * operations are permitted apart of the ones exported by this module. */
AVL_TREE new_avl_tree(
	AVL_FCOMP fc /* comparator function, see above. */
); /* Tested Mon Apr  9 10:34:05 CEST 2012 OK*/

/* AVL_TREE destructor. Call this function when you don't need anymore the
 * AVL_TREE you got in the new_avl_tree() function.  It deallocates all
 * the allocated memory and frees the pointer, so it is not available
 * anymore.  Please, take into account that the pointer continues to point
 * to memory, so you must set it to NULL after calling this function. */
void free_avl_tree(
	AVL_TREE t /* tree descriptor */
); /* Tested Mon Apr  9 10:34:17 CEST 2012 OK*/

/* The next function clears an AVL_TREE so it's empty after this call.
 * This function deallocates all memory related to this AVL_TREE also. */
void avl_tree_clear(
	AVL_TREE t /* tree descriptor */
);

/* This function gets an AVL_ITERATOR pointing to the first node of the
 * tree, so getting the key or the data (see below) gets the data pointing
 * to the first lexicographical key of the tree map. */
AVL_ITERATOR avl_tree_first(
	AVL_TREE t /* the tree descriptor */
); /* Tested Mon Apr  9 10:34:32 CEST 2012 OK*/

/* This function gets an AVL_ITERATOR pointing to the last node in the
 * tree map.  Se above. */
AVL_ITERATOR avl_tree_last(
	AVL_TREE t /* the tree map descriptor */
); /* Tested Mon Apr  9 10:34:43 CEST 2012 OK*/

/* This function gets an AVL_ITERATOR pointing to the node specified by
 * key k.  If ex is FALSE and the key is not in the map, the function
 * returns a key next to the key passed.  (not necessarily before of after,
 * can be either) If ex is TRUE, it returns an iterator pointing to key or
 * NULL in case it doesn't exist in the map. */
AVL_ITERATOR avl_tree_atkey(
	AVL_TREE t, /* the tree descriptor */
	const char *k, /* the key searched */
	AVL_MT mt /* true for exact match of false if we want a nearest key */
); /* Mon Apr  9 11:09:27 CEST 2012 */

/* This function sets a key to value d (d is void *).
 * It sets key k to map d either it exists or not.  If key is not
 * existent, the function creates a key. */
AVL_ITERATOR avl_tree_put(
	AVL_TREE t, /* the tree descriptor */
	const char *k, /* the pair key */
	void *d /* the pair data pointer */
); /* Tested Mon Apr  9 10:35:28 CEST 2012 OK*/

/* This function deletes a <key,data> pair identified by key k.
 * The function returns TRUE if deleted successfully and FALSE otherwise. */
int avl_tree_del(
	AVL_TREE t, /* the tree descriptor */
	const char *k /* the key to be deleted. */
); /* Tested Mon Apr  9 10:35:38 CEST 2012 OK*/

/* This function deletes the <key,data> pair pointed to by iterator i.
 * i must be a valid iterator on tree t.  */
void avl_iterator_del(
	AVL_ITERATOR i, /* iterator pointing to a valid pair in tree t. */
	AVL_TREE t /* tree descriptor of tree whose element pair is
				* going to be deleted.  */
);

/* This function checks tree t for emptyness. */
int avl_tree_empty(
	AVL_TREE t /* the tree descriptor.  */
);

/* This function gives the number of pairs in tree t.  */
int avl_tree_size(
	AVL_TREE t /* the tree descriptor. */
); /* Mon Apr  9 11:08:58 CEST 2012 OK*/

/* This function returns TRUE if tree t has a key named k. */
int avl_tree_has(
	AVL_TREE t, /* the tree to be searched.  */
	const char *k /* the key to search for. */
); /* Mon Apr  9 10:36:17 CEST 2012 */

/* This function returns the data pointer for key k in tree t. */
void *avl_tree_get(
	AVL_TREE t, /* the tree to be searched for. */
	const char *k /* the key to search. */
); /* Mon Apr  9 10:36:32 CEST 2012 */

/* This function pretty prints a tree description of tree t. */
void avl_tree_print(
	AVL_TREE t, /* the tree to print. */
	FILE *o /* the FILE descriptor to print t on. */
); /* Tested Mon Apr  9 10:36:46 CEST 2012 */

/* This function gets an iterator value pointing to the next element
 * in tree map t. */
AVL_ITERATOR avl_iterator_next(
	AVL_ITERATOR i /* the iterator from where to move. */
); /* Tested Mon Apr  9 10:36:58 CEST 2012 */

/* This function gets an iterator value pointing to the previous element
 * in tree map t. */
AVL_ITERATOR avl_iterator_prev(
	AVL_ITERATOR i /* the iterator from where to move. */
); /* Tested Mon Apr  9 10:37:07 CEST 2012 */

/* This function gets the iterator key of a pair. */
char *avl_iterator_key(
	AVL_ITERATOR i /* iterator to get key from */
); /* Tested Mon Apr  9 10:37:17 CEST 2012 */

/* This function gets the iterator data pointer of a pair. */
void *avl_iterator_data(
	AVL_ITERATOR i /* iterator to get data pointer from */
); /* Tested Mon Apr  9 10:37:30 CEST 2012 */

#endif /* AVL_H */
/* Do not include anything AFTER the line above, as it would not be
 * protected against double inclusion from other files.  */
/* $Id: avl.h,v 1.1.1.1 2012/08/20 19:25:31 luis Exp $ */
