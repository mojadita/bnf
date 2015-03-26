# $Id: Makefile,v 1.13 2012/09/20 10:41:52 luis Exp $
# vim: ts=8 sw=8 nu
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: lun jul 16 11:14:49 CEST 2012
# Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.

targets = bnf
ut_targets = $(foreach t,$(targets),$(foreach u,$($(t)_objs),$(u:.o=_ut)))

CFLAGS += -g3 -O0

.PHONY: all clean ut

all: $(targets) $(ut_targets)
clean:
	$(RM)	$(targets) \
		$(tstavl_objs) \
		$(bnf_objs) \
		bnf_pparser.c \
		bnf_pparser.h

ut:
	$(MAKE) $(ut_targets)
	echo $(ut_targets)

avl.o: avl.h

bnf_objs = \
	bnf_pparser.o \
	bnf_sparser.o \
	tokeninfo.o \
	bnf_main.o \
	print_symbols.o \
	print_strtable.o \
	bnf_cfg.o 
	#pass0.o pass1.o pass2.o \
	#pass3.o pass4.o
bnf_libs = -lavl
bnf: $(bnf_objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o bnf $(bnf_objs) $(bnf_libs)

bnf_pparser.o: bnf_symtab.h tokeninfo.h bnf_main.h pass1.h
bnf_sparser.o: bnf_pparser.h tokeninfo.h bnf_main.h bnf_sparser.h
tokeninfo.o: tokeninfo.h lists.h
bnf_main.o: bnf_symtab.h tokeninfo.h bnf_sparser.h bnf_main.h pass1.h print_symbols.h print_strtable.h
print_symbols.o: print_symbols.h
print_strtable.o: print_strtable.h
pass0.o: pass1.h bnf_symtab.h tokeninfo.h bnf_main.h
pass1.o: pass1.h bnf_symtab.h tokeninfo.h bnf_main.h
pass2.o: pass1.h bnf_symtab.h tokeninfo.h bnf_main.h
pass3.o: pass1.h bnf_symtab.h tokeninfo.h bnf_main.h
pass4.o: pass1.h bnf_symtab.h tokeninfo.h bnf_main.h

bnf_pparser.c bnf_pparser.h: bnf_pparser.y
	$(YACC) $(YFLAGS) --defines=bnf_pparser.h --output=bnf_pparser.c bnf_pparser.y

# $Id: Makefile,v 1.13 2012/09/20 10:41:52 luis Exp $
