UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-L/usr/local/lib -ludunits -lnetcdf -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/sw/lib/pgplot
export UNIXMAKE=make
export UNIXLIBS=-L/sw/lib -ludunits -lnetcdf -lm
endif
ifeq ($(UNIX),SunOS)
export UNIXCPP=g++
export UNIXCPPOPS=-g -Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-L/usr/local/netcdf/lib -L/usr/local/udunits/lib -ludunits -lnetcdf -lm
export UNIXMAKE=make
endif

ifndef INSTALLDIR
export INSTALLDIR=/usr/local/share/ghaas
endif
