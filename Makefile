# $Id: Makefile,v 1.13 2012/09/20 10:41:52 luis Exp $
# vim: ts=8 sw=8 nu
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: lun jul 16 11:14:49 CEST 2012
# Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.

targets = bnf

.PHONY: all clean

all: $(targets)
clean:
	$(RM)	$(targets) \
		$(tstavl_objs) \
		$(bnf_objs) \
		bnf_pparser.c \
		bnf_pparser.h

avl.o: avl.h

tstavl_objs = tstavl.o avl.o
tstavl_libs = 
tstavl: $(tstavl_objs)
	$(CC) $(LDFLAGS) -o tstavl $(tstavl_objs) $(tstavl_libs)
$(tstavl_objs): avl.h

bnf_objs = \
	bnf_pparser.o \
	avl.o \
	bnf_sparser.o \
	tokeninfo.o \
	bnf_main.o \
	print_symbols.o \
	print_strtable.o \
	bnf_cfg.o \
	pass0.o pass1.o pass2.o \
	pass3.o pass4.o
bnf_libs =
bnf: $(bnf_objs)
	$(CC) $(CFLAGS) $(LDFLAGS) -o bnf $(bnf_objs) $(bnf_libs)

bnf_pparser.o: bnf_tabsim.h avl.h tokeninfo.h bnf_main.h pass1.h
bnf_sparser.o: bnf_pparser.h tokeninfo.h bnf_main.h bnf_sparser.h
tokeninfo.o: tokeninfo.h
bnf_main.o: bnf_tabsim.h avl.h tokeninfo.h bnf_sparser.h bnf_main.h pass1.h print_symbols.h print_strtable.h
print_symbols.o: print_symbols.h avl.h
print_strtable.o: print_strtable.h avl.h
pass0.o: pass1.h bnf_tabsim.h avl.h tokeninfo.h bnf_main.h
pass1.o: pass1.h bnf_tabsim.h avl.h tokeninfo.h bnf_main.h
pass2.o: pass1.h bnf_tabsim.h avl.h tokeninfo.h bnf_main.h
pass3.o: pass1.h bnf_tabsim.h avl.h tokeninfo.h bnf_main.h
pass4.o: pass1.h bnf_tabsim.h avl.h tokeninfo.h bnf_main.h

bnf_pparser.c bnf_pparser.h: bnf_pparser.y
	$(YACC) $(YFLAGS) --defines=bnf_pparser.h --output=bnf_pparser.c bnf_pparser.y

# $Id: Makefile,v 1.13 2012/09/20 10:41:52 luis Exp $
