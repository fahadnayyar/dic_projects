// gcc -o wbadmin  server_clnt.o client_s.o server_xdr.o -lX11

/*
 * An example 730-C-RPC White Board Administration program
 *
 * compile with:  gcc wbadmin.c -std=c99  -o wbadmin
 */

#include "wb.h"

/* The declarations for BClient, BBoard, XferWBArg, query, newserver,
 * and transferwhiteboard should go into the new server.x */

// struct BClient {		/* cf with AClient */
//   struct BClient *next;
//   ClientData * clientdata;
// 				/* CLIENT *callback; removed */
// };

// struct BBoard {			/* cf with ABoard */
//   struct BBoard *next;
//   struct BClient *clients;
//   ALine *lines;
// };

// #define NMSZ 50

// struct XferWBArg {
//   char boardnm[NMSZ];		/* board name to be transferred */
//   char machinenm[NMSZ];		/* name of the receiving machine */
//   int nprogram;			/* rpc prog num of the server above */
//   int nversion;			/* registered version number */
// };

// struct BBoard * query_1(int * unused, CLIENT * clp); //{  return 0;}
// int newserver_1(char * machineName, CLIENT * clp) { return 0; }
// int transferwhiteboard_1(struct XferWBArg a, CLIENT * clp) { return 0; }

//* usage of wbadmin.
void usage()
{
  fprintf
    (stderr,
     "\nNote: Linked with dummy query, newserver, transferwhiteboard functions.\n\n"
     "Usage: -q for query, -n for new server create, -t for transfer board\n"
     "wbadmin -q <server-machine-nm> <prognum-in-hex>\n"
     "wbadmin -n <existing-server-machine-nm> <prognum-in-hex> <new-server-machine-nm>\n"
     "wbadmin -t <from-server-machine-nm> <prognum-in-hex> <wb-nm> <to-server-machine-nm> <prognum>\n");
}

//* function to query a server with name snm and rpc program number prgn.
int queryServer(char * snm, int prgn, CLIENT * clp)
{
  assert(snm && clp);
  //* rpc call to server to get its board as BBoard b.
  int dummy = 0;
  struct BBoard * b = query_1(&dummy, clp);
  

  if (b == 0)
  {
    printf("ERROR: Server has no boards.\n");
    return -1;
  }
  else
  {
    for (; b; b = b->next) 
    {
      printf("Board %s on server %s prognum %x has:\n", b->clients->clientdata->boardnm, snm, prgn);
      if (b->clients == 0)
      {
        printf("\tno clients\n"); /* for robustness and better functionality ... */
      } 	
      else 
      {
        for (struct BClient * c = b->clients; c; c = c->next)
        {
          printf("\tclient on server %s displayed at %s with prognum %x\n",
          c->clientdata->machinenm,
          c->clientdata->xdisplaynm,
          c->clientdata->nprogram);
        }        
          ALine *l;
          printf("\n\tthese are the lines on the board:\n");
          for (l = b->lines; l ; l=l->next)
          {
            printf("\t\t%ld, %d, %d, %d, %d\n", l->ln.color, l->ln.x1, l->ln.x2, l->ln.y1, l->ln.y2);    
          }
      } 
      }
  }
  assert(1);
  return 0;
}


int transferBoard(struct XferWBArg a, CLIENT * clp)
{  
  assert(clp);
  struct XferWBArg * xa = (struct XferWBArg *)malloc(sizeof(struct XferWBArg));
  xa->nprogram = a.nprogram;
  xa->nversion = a.nversion;
  strcpy(xa->boardnm, a.boardnm);
  strcpy(xa->machinenm, a.machinenm);
  printf("transferboard in wbadmin: %d %d %d %d\n", xa->nprogram, xa->nversion, xa->nversion, xa->nprogram);
  int ret = *transferwhiteboard_1(xa, clp); // doubt.
  if (ret==-1)
  {
    printf("ERROR: unable to transfer board!\n");
  }
  assert(1);
  return ret;
}

int startNewServer(char* nsnm1, CLIENT * clp)
{
  assert(nsnm1 && clp);
  struct NewServerMcNm nsnm;
  strcpy(nsnm.newServerMachineName, nsnm1);
  struct NewServerMcNm* newServerMcNm = &nsnm;
  int return_program_num = *newserver_1(newServerMcNm, clp); // doubt: can we directly pass nsnm?
  
  //* debugging.
  // printf("nsnm1: %s\n", nsnm1);
  // printf("newServerMcNm: %s\n", newServerMcNm->newServerMachineName);
  // printf("return_program_num: %d\n", return_program_num);
  
  if (return_program_num == -1)
  {
    printf("ERROR: <existing-server-machine-nm> != <new-server-machine-nm>\n");
  }
  assert(1);
  return return_program_num;
}



int main(int argc, char * argv[])
{
  CLIENT * clp = 0; // doubt
  int  result = -1;
	
  if (argc < 4)
    goto error;
  char * cmd = argv[1];
  if (cmd[0] != '-')
    goto error;

  char * host = argv[2];
  int prognum = strtol(argv[3], 0, 16);
  printf("prgn %x\n", prognum);
  clp = clnt_create(host, prognum, WhiteBoardServerVersion, "tcp"); // doubt
  if (clp == 0)
  {
    printf("yo");
    goto error;
  }

  switch (cmd[1]) {
  case 'q':
    result = queryServer(host, prognum, clp);
    break;
  case 'n':
    if (argc < 5)
      goto error;
    // result = newserver_1(argv[4], clp); /* result == prog num of new server */
    result = startNewServer(argv[4], clp);
    break;
  case 't':
    if (argc < 6)
      goto error;
    struct XferWBArg xa;
    strcpy(xa.boardnm, argv[4]);
    strcpy(xa.machinenm, argv[5]);
    xa.nprogram = strtol(argv[6], 0, 16);
    xa.nversion = WhiteBoardServerVersion;
    // result = transferwhiteboard_1(xa, clp);
    result = transferBoard(xa, clp);
    break;
  default:
    goto error;
  }
done:
  if (clp)
    clnt_destroy(clp);
  printf("result %d\n", result);
  return result;

error:				/* more elaborate error reporting is better */
    usage();
    goto done;
}

/* -eof- */
