#
# Makefile for the DIC CEG 7370 C+RPC  based WhiteBoard project
# Linux version
# end-of-line comments are helpful in non-Linux OS

CURRENT_DIR=build
SOBJ = server.o server_svc.o client_clnt.o server_xdr.o
COBJ = client.o transient.o server_clnt.o client_s.o server_xdr.o xwindow.o
# doubt: why DBSD_SIGNALS is comented?
CFLAGS =  -Wall -pedantic -ansi -std=c99 # -DBSD_SIGNALS 
# doubt: what version of gcc should I use? By default it is using gcc-7.
CC = gcc

all: server7370 client7370 deregall7370 xwindow7370
	@ ls -l server7370 client7370 xwindow7370 deregall7370

xwindow7370: xwindow.c
	# doubt: why not -O3? What is DMAIN? What is -lx11?
	gcc -o xwindow7370 -DMAIN xwindow.c -lX11

deregall7370: deregall.o
	gcc -o deregall7370 deregall.o

server7370: server.h $(SOBJ) 
	gcc -o server7370 $(SOBJ)
#doubt: what is this below comment?
# on some OS -lrpcsvc

client7370: server.h client.h $(COBJ)
	gcc -o client7370 $(COBJ) -lX11
# /usr/lib/libX11.so # -lrpcsvc -lX11

server.h server_xdr.c server_clnt.c server_svc.c: server.x wb.h
	rpcgen server.x

client.h client_xdr.c client_svc.c client_clnt.c: client.x wb.h
	rpcgen client.x

client_s.c: client_svc.c
	cp -f client_svc.c client_s.c
	ed client_s.c < ed-script.txt 2> /dev/null > /dev/null

#doubt: use of make print?
print:
	enscript -B -fCourier8 -2r Makefile 0README.txt \
		server.x server.c client.x  client.c

#doubt: use of make indent? What is this indent comand?
indent:
	indent -kr -di1 client.c client.x server.c server.x xwindow.c

wbadmin: wbadmin.c wbadmin7370 wbadmin.o
# 	gcc -c wbadmin.c -std=c99  -o wbadmin.o
	gcc -o wbadmin7370 wbadmin.c server_clnt.o server_xdr.o -lX11

tar archive: clean
	(cd ..; tar cvvfj $(CURRENT_DIR)-`date +%G%m%d%H%M`.tbz $(CURRENT_DIR))

clean:
	rm -f *.o *.BAK *~ *.out *_svc.c *_clnt.c *_xdr.c \
		server7370 client7370 xwindow7370 deregall7370 wbadmin7370\
		client_s.c server.h client.h

# -eof-

