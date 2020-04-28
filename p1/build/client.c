
/* client.c */

#include "wb.h"

/*
 * A "white board client" is a pair of processes.  The forked child
 * receives server call-backs to callbackfromwbs() via its svc_run().
 * The parent process will handle the xwindow I/O.  These two
 * write/read via the pipe xwinio.
 */

static int parentid, childid;	/* process ids */
static int xwinio[2];		/* pipe: child writes, parent reads */
static AddLineArg me;		/* all the info there is about this client */
static CLIENT *clp;		/* librpc clnt_create()-ed */ // doubt 

/*
 * Terminate the client.  Remove all traces of the parent+child
 */
// parent function.
static void endtheclient(int unused)
{
  assert(1);
  delclient_1(&me.clientdata, clp); /* ask server to delete me */ // doubt
  // parent.
  clnt_destroy(clp);		/* CLIENT structure */ // doubt
  pmap_unset(me.clientdata.nprogram, me.clientdata.nversion); // doubt
  closexwindow();
  kill(childid, SIGTERM);
  assert(1);
  exit(0);
  assert(0); //* this should be unreachable. 
}


/*
 * Get the call back from the server who is sending the coordinates of
 * a line to draw.  We simply write these four integers into the
 * xwinio pipe, and raise the signal so the parent can read.
 */
// child
void *callbackfromwbs_1_svc(OneLn * p)
{
  assert(p);
  static int i = 0;		/* note: static */

  write(xwinio[1], p, sizeof(OneLn));
  kill(parentid, SIGUSR1);	/* kill == "raise" */
  assert(1);
  return (void *) &i;
}

/*
 * Get the call back from the server who is transfering this client's board to another server.
 * We simply close the rpc handle to that server and ask the parent process to open rpc handle to 
 * the new server by writing new server's info in inxwinio pipe, and raise the signal so the parent can read.
 */
// child
void *addnewserverconnection_1_svc(struct XferWBArg * xa, struct svc_req * srq)
{
  assert(xa);
  static int i = 0;		/* note: static */
  printf("In client child process: xa.boardname: %s, xa.machinemane: %s, xa.nprogram: %d, xa.nversion: %d\n", xa->boardnm, xa->machinenm, xa->nprogram, xa->nversion );
  write(xwinio[1], xa, sizeof(struct XferWBArg));
  kill(parentid, SIGUSR2);	/* kill == "raise" */
  assert(1);
  return (void *) &i;
}

/*
 * Invoked via addnewserverconnection_1_svc/SIGUSR2 when a new server info comes from the
 * previous server to the svc_run()-ning process.
 */
// parent.
static void readndchangeserver(int unused)
{
  struct XferWBArg xa;
  (void) read(xwinio[0], &xa, sizeof(xa));
  printf("In client parent process: xa.boardname: %s, xa.machinemane: %s, xa.nprogram: %d, xa.nversion: %d\n", xa.boardnm, xa.machinenm, xa.nprogram, xa.nversion );
  clnt_destroy(clp);		/* CLIENT structure */ // doubt
  clp = clnt_create(xa.machinenm, xa.nprogram, xa.nversion, "tcp");
  assert(clp);
}

/*
 * Invoked via callbackfromwbs/SIGUSR1 when a new line comes from the
 * server to the svc_run()-ning process.
 */
// parent.
static void readndraw(int unused)
{
  assert(1);
  OneLn lc;

  (void) read(xwinio[0], &lc, sizeof(lc));
  drawline(&lc);
  assert(1);
}

/*
 * Client window got exposed.  Redraw the lines.
 */
// parent.
static void exposedwindow()
{
  assert(1);
  // parent.
  Linep p, *q = sendallmylines_1(&me.clientdata, clp);
  int n = 0;

  if (q == NULL)
  {
    assert(1);
    return;
  }
  for (p = *q; p; p = p->next) {
    drawline(&p->ln);
    n++;
  }
  assert(1);
}

/*
 * Watch for mouse input.  Button 3 (right) ends this routine.
 * Pressing buttons 1 (left) or 2 (middle) sends the line to the
 * server who will distribute it to all member white boards.
 */
static void mousewatch()
{
  int btn = 5;

  for (;;)
    switch (btn) {
	case 1:
	case 2:
	  me.ln.color = me.clientdata.color;
	  // parent.
    addline_1(&me, clp);
	  btn = 0;
	  break;
	case 3:
	  return;		/* <== */
	case 5:
	  exposedwindow();
	  btn = 0;
	  break;
	default:
	  btn = trackpointer(&me.ln, 0);
	  break;
    }
    // assert(1);
}

/*
 * Called by client_s.c.  See ./ed-script. Start the client.
 */
void startclient
    (int nprogram, int nversion,
     char *servermcnm, int serverprognum, char *boardnm, char *xdisplaynm, char *pmcolor) {
  assert(servermcnm && boardnm && xdisplaynm && pmcolor);
  /* clients own details -- once set, these do not change */
  me.clientdata.color = atoir(pmcolor, 16);
  me.clientdata.nprogram = nprogram;
  me.clientdata.nversion = nversion;
  gethostname(me.clientdata.machinenm, sizeof(me.clientdata.machinenm)); // doubt
  strcpy(me.clientdata.boardnm, boardnm);
  strcpy(me.clientdata.xdisplaynm, xdisplaynm);
  strcat(me.clientdata.xdisplaynm, ":0.0");

  char xwintitle[100];
  sprintf(xwintitle, "%s@%s color=%lx",
	  boardnm, me.clientdata.machinenm, me.clientdata.color);

  clp = clnt_create
      (servermcnm, serverprognum, WhiteBoardServerVersion, "tcp"); // doubt
  if (!clp) {
    fprintf(stderr,
	    "1client730: clnt_create(%s,0x%x,0x%x,%s) failed.\n",
	    servermcnm, serverprognum, WhiteBoardServerVersion, "tcp");
    exit(1);
    assert(0); //* unreachable
  }



  if (pipe(xwinio) == -1) {
    fprintf(stderr, "client730: xindow io pipe failed.\n");
    exit(2);
    assert(0); //* unreachable
  }

  childid = fork();
  if (childid == -1) {
    fprintf(stderr, "client730: fork was unsuccessful.\n");
    exit(3);
    assert(0); //* unreachable
  }
  if (childid == 0) {
    /* the child process */
    close(xwinio[0]);
    parentid = getppid();
    return;			/* child returns to do svc_run() */
  }

  /* parent process continues */

  {				/* setup signal handling */
    struct sigaction asigterm, asiguser, asiguser2;
    asigterm.sa_handler = endtheclient;
    asigterm.sa_flags = 0;
    sigemptyset(&asigterm.sa_mask);
    sigaction(SIGTERM, &asigterm, 0);

    asiguser.sa_handler = readndraw;
    asiguser.sa_flags = 0;
    sigemptyset(&asiguser.sa_mask);
    sigaction(SIGUSR1, &asiguser, 0);

    asiguser2.sa_handler = readndchangeserver;
    asiguser2.sa_flags = 0;
    sigemptyset(&asiguser2.sa_mask);
    sigaction(SIGUSR2, &asiguser2, 0);
  }

  close(xwinio[1]);
  int x = openxwindow(me.clientdata.xdisplaynm, xwintitle);
  if (x < 0) {
    fprintf(stderr, "client730: openxwindow(%s, %s) == %d, failed\n",
	    me.clientdata.xdisplaynm, xwintitle, x);
    exit(4);
    assert(0); // unreachable.
  }
  addclient_1(&me.clientdata, clp); // doubt
  mousewatch();	    /* returns only when button3 is clicked */
  endtheclient(0);
}

/* -eof- */
