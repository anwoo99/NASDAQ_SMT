include ../environments

LIB=$(LIB_DIR)
INC=$(INC_DIR)
CFLAGS= -g -O -I./ -I../inc $(FEP_CFLAGS) 
LFLAGS=$(FEP_LFLAGS) -L../lib/fep -lFEP -lm
AFLAGS=$(MY_AFLAGS)
AROPTS=$(MY_AROPTS)
LIBOBJ=libNASOBJ.o

INCINC=../inc/context.h ../inc/fep.h ../inc/schema.h ../inc/stream.h ../inc/config.h ./nastag.h
CMD=nasrecv nasfep

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

nasrecv:nasrecv.o $(LIBOBJ) 
	$(CC) -o nasrecv nasrecv.o $(LIBOBJ) $(LFLAGS)
nasfep:nasfep.o	$(LIBOBJ)
	$(CC) -o nasfep nasfep.o $(LIBOBJ) $(LFLAGS)

nasfep.o: nasfep.c $(INCINC)
nasrecv.o: nasrecv.c $(INCINC)
$(LIBOBJ):	$(LIBOBJ)(smartoption.o)	$(LIBOBJ)(nastool.o)	\
		$(LIBOBJ)(moldudp64.o)
		if [ -s /usr/bin/ranlib ]; then ranlib $(LIBOBJ); fi
		@echo $(LIBOBJ) is up-to-date.


$(LIBOBJ)(smartopion.o):	$(INCINC)
$(LIBOBJ)(nastool.o):		$(INCINC)
$(LIBOBJ)(moldudp64.o):		$(INCINC)

install: all
	cp -f $(CMD) $(BIN_DIR)
clean:	
	rm -f *.o $(CMD)
