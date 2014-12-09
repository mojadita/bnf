dnl $Id: macros.m4,v 1.2 2012/09/22 22:51:55 luis Exp $
dnl AUTHOR: Luis Colorado <licolorado@indra.es>
dnl DATE: lun nov 29 15:57:19 CET 2004
dnl Disclaimer: (C) 2012 LUIS COLORADO SISTEMAS S.L.U.
dnl                 All rights reserved.
divert(-1)dnl
dnl MUST DO A divert(-1) BEFORE INCLUDING THIS FILE.
dnl SOME GENERAL MACROS TO CONTROL OUTPUT POSITION IN
dnl OUTPUT FILE.

define(`_lq_',changequote(<,>)<changequote(<,>)`changequote(`,')>changequote(`,'))
define(`_rq_',changequote(<,>)<changequote(<,>)'changequote(`,')>changequote(`,'))

dnl BEGINNING_OF_TEMPLATE
define(`BEGINNING_OF_TEMPLATE',`dnl
divert(0)dnl
undefine(`BEGINNING_OF_TEMPLATE')dnl
')dnl

dnl INSERTION_POINT
define(`INSERTION_POINT',`dnl
define(`D_$1',`divert('THE_DIVERSION`)')dnl
define(`THE_DIVERSION', eval(THE_DIVERSION + 1))dnl
divert(THE_DIVERSION)dnl
')dnl

dnl WE SAVE THE DIVERSION NUMBER TO INSERTION_POINT() TO
dnl WORK.
define(`THE_DIVERSION',`0')dnl

dnl D_NONE IS A MACRO TO CUT OUTPUT. MUST BE CALLED AT THE
dnl END OF EACH MACRO DEFINITION.
define(`D_NONE',`divert(-1)')dnl

define(ASK, `dnl
syscmd(`echo -n "$1($2): " >&2')dnl
define(`$1', esyscmd(`read LINE; echo -n "$LINE"'))dnl
ifelse(defn(`$1'),,`define(`$1', `$2')')dnl
')dnl

define(`chop', `substr($1, 0, eval(len($1)-1))')dnl

define(`THE_DATE', esyscmd(`LC_ALL=C LC_MESSAGES=C LANG=C date'))dnl
define(`THE_DATE', chop(THE_DATE))dnl

define(`REPEAT',`dnl
ASK(`$1')dnl
ifelse(defn(`$1'),,,`dnl
$2`'REPEAT(`$1',`$2')dnl
')dnl
')dnl

define(`append',`dnl
define(`$1',defn(`$1')ifelse(defn(`$1'),,,``$3'')`$2')dnl
')dnl append

# $Id: macros.m4,v 1.2 2012/09/22 22:51:55 luis Exp $
