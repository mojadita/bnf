/* $Id: bnf_cfg.h,v 1.1 2012/09/09 11:54:38 luis Exp $
 * vim: ts=8 sw=4 ai nu
 * Author: Luis Colorado <lc@luiscoloradosistemas.com>
 * Date: Sun Sep  9 13:37:39 CEST 2012
 * Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
 *                 All rights reserved.
 */
#ifndef BNF_CFG_H
#define BNF_CFG_H

char *get_config(char *s, char *d);
void put_config(char *s, char *v);
size_t fprint_config_m4(FILE* o);
size_t print_config_m4(void);

#endif /* BNF_CFG_H */
/* $Id: bnf_cfg.h,v 1.1 2012/09/09 11:54:38 luis Exp $ */
