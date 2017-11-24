SRC1=		main.cpp vito_p300.cpp unit.cpp
SRC2=		sign.c
SRCH=		vito_p300.h unit.h
ALL_SRC=	$(SRC1) $(SRC2) $(SRCH)
OBJS=		main.o vito_p300.o unit.o
SBIN=		/usr/local/sbin
EXE=		vConnect
EXE2=		vconnect
TARNAME=vconn_`date +%F_%T`.tar

CFLAGS+=	-O -g -Wall

all:	vConnect

vConnect:	$(OBJS)
	g++ $(CFLAGS) $(OBJS) -o $@

install:	vConnect
	/usr/bin/install -m 555 -o root -g wheel $(EXE) $(SBIN)/
	@ln -s $(SBIN)/$(EXE) $(SBIN)/$(EXE2)

clean:
	@rm $(OBJS) $(EXE) *.~ *.tar *.tar.bz2

tar:
	tar cvf $(TARNAME) $(ALL_SRC) Makefile
	bzip2 -f -v -9 $(TARNAME)
