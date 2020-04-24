
/* client.x */

#include "oneln.h"

#define NMSZ	50

struct XferWBArg {
  char boardnm[NMSZ];		/* board name to be transferred */
  char machinenm[NMSZ];		/* name of the receiving machine */
  int nprogram;			/* rpc prog num of the server above (receiving server program number) */
  int nversion;			/* registered version number */
};


program WhiteBoardClient {
  version WhiteBoardClientVersion {
    void callbackfromwbs(OneLn) = 1;
    void addnewserverconnection(XferWBArg) = 2;
  } = 1;
} = 0x20000999;			/* replaced by gettransient() value */

/* -eof- */
