# Copyright (C) 1997, 2002, 2003, 2006, 2007 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with the GNU C Library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
# 02111-1307 USA.

#
#	Makefile for argp.
#

include ../config.make

TESTS		= argp-test tst-argp1 bug-argp1 tst-argp2

OPTS_argp-ex3 = ARG1 ARG2
OPTS_argp-ex4 = ARG1 string1 string2 string3
OPTS_bug-argp1 = -- --help

include ../Test.mak

TOBEINSTALLED=$(TOBEINSTALLED_BIN) $(TOBEINSTALLED_SH)

.PHONY: $(TOBEINSTALLED)

install: $(TOBEINSTALLED)
ifneq ("n$(TOBEINSTALLED)n","n n")
	echo "Installing argp....."
	cp -fr $(foreach f,$^, $(OBJPFX)/$f) .
else
	@echo "Nothing to install for argp."
endif
