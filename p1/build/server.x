
/* server.x == WhiteBoard Server RPC Interface */

#define NMSZ	50

#include "oneln.h"

typedef struct ALine *Linep; /* Linep is of type struct Aline */ 

struct ALine {
  struct ALine *next;		/* generic singly-linked list */
  OneLn ln;			/* see oneln.h */
};

struct ClientData {
  char boardnm[NMSZ];		/* board name */
  char xdisplaynm[NMSZ];	/* X11 display name */
  char machinenm[NMSZ];		/* name of machine running this client */
  int nprogram;			/* rpc prog num for callbacks */ /* doubt */
  int nversion;			/* registered version number */ /* doubt */
  long color;
};

struct AddLineArg {
  OneLn ln;
  ClientData clientdata;
};

struct NewServerMcNm{
  char newServerMachineName[NMSZ];
};

struct BClient {		/* cf with AClient */
  struct BClient *next;
  ClientData * clientdata;
				/* CLIENT *callback; removed */
};

struct BBoard {			/* cf with ABoard */
  struct BBoard *next;
  struct BClient *clients;
  ALine *lines;
};

struct XferWBArg {
  char boardnm[NMSZ];		/* board name to be transferred */
  char machinenm[NMSZ];		/* name of the receiving machine */
  int nprogram;			/* rpc prog num of the server above (receiving server program number) */
  int nversion;			/* registered version number */
};


/* doubt: did not understand the syntax of the below struct. Use of this struct/program? */  
program WhiteBoardServer {
  version WhiteBoardServerVersion {
    int addclient(ClientData) = 1;
    int delclient(ClientData) = 2;
    int addline(AddLineArg) = 3;
    Linep sendallmylines(ClientData) = 4;
    struct BBoard query(int) = 5;
    int newserver(NewServerMcNm) = 6; 
    int transferwhiteboard(XferWBArg) = 7;
    int receivenewwhiteboard(BBoard) = 8;
  } = 1;
} = 0x20000099;			/* change to your own last 4 digits */ /* doubt? */ 

/* -eof- */


