include ../environments

LIB=$(LIB_DIR)
INC=$(INC_DIR)
CFLAGS= -g -O -I./ -I../inc $(FEP_CFLAGS) 
LFLAGS=$(FEP_LFLAGS) -L../lib/fep -lFEP -lm -w -rdynamic -g
AFLAGS=$(MY_AFLAGS)
AROPTS=$(MY_AROPTS)
LIBOBJ=libSMTOBJ.o

INCINC=../inc/context.h ../inc/fep.h ../inc/schema.h ../inc/stream.h ../inc/config.h
CMD=smtrecv smtfep

# Default Rules:
.c:
	$(CC) $(CFLAGS) $(LFLAGS) $< -o $@
.c~:
	$(GET) $(GFLAGS) -p $< > $*.c
	$(CC) $(CFLAGS) $(LFLAGS) $*.c -o $@
	-rm -f $*.c
.c.a:
	$(CC) -c $(CFLAGS) $<
	$(AR) $(AFLAGS) $@ $*.o; rm -f $*.o 
.s~.s:
	$(GET) $(GFLAGS) $<
.h~.h:
	$(GET) $(GFLAGS) $<
.c~.c:
	$(GET) $(GFLAGS) $<

all:	$(CMD) $(CHK)

smtrecv:smtrecv.o $(LIBOBJ) 
	$(CC) -o smtrecv smtrecv.o $(LIBOBJ) $(LFLAGS)
smtfep:smtfep.o	$(LIBOBJ)
	$(CC) -o smtfep smtfep.o $(LIBOBJ) $(LFLAGS)

smtfep.o: smtfep.c $(INCINC)
smtrecv.o: smtrecv.c $(INCINC)
$(LIBOBJ):	$(LIBOBJ)(smartoption.o)	$(LIBOBJ)(smttool.o)	\
		$(LIBOBJ)(moldudp64.o)		$(LIBOBJ)(smt_0x33.o)		\
		$(LIBOBJ)(smtinst.o)		\
		if [ -s /usr/bin/ranlib ]; then ranlib $(LIBOBJ); fi
		@echo $(LIBOBJ) is up-to-date.


$(LIBOBJ)(smartopion.o):	$(INCINC)
$(LIBOBJ)(smttool.o):		$(INCINC)
$(LIBOBJ)(moldudp64.o):		$(INCINC)
$(LIBOBJ)(smt_0x33.o):		$(INCINC)
$(LIBOBJ)(smtinst.o):		$(INCINC)

install: all
	cp -f $(CMD) $(BIN_DIR)
clean:	
	rm -f *.o $(CMD)
