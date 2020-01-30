
/* server.x == WhiteBoard Server RPC Interface */

#define NMSZ	50

#include "oneln.h"

typedef struct ALine *Linep;

struct ALine {
  struct ALine *next;		/* generic singly-linked list */
  OneLn ln;			/* see oneln.h */
};

struct ClientData {
  char boardnm[NMSZ];		/* board name */
  char xdisplaynm[NMSZ];	/* X11 display name */
  char machinenm[NMSZ];		/* name of machine running this client */
  int nprogram;			/* rpc prog num for callbacks */
  int nversion;			/* registered version number */
  long color;
};

struct AddLineArg {
  OneLn ln;
  ClientData clientdata;
};

program WhiteBoardServer {
  version WhiteBoardServerVersion {
    int addclient(ClientData) = 1;
    int delclient(ClientData) = 2;
    int addline(AddLineArg) = 3;
    Linep sendallmylines(ClientData) = 4;
  } = 1;
} = 0x20000099;			/* change to your own last 4 digits */

/* -eof- */
