
/* server.c */

#include "wb.h"

/*
 * Generic node in a singly-linked list
 */
typedef struct ListNode {
  struct ListNode *next;
} ListNode;

//* for assertions.
static int is_same_list(ListNode* hdr1, ListNode* hdr2)
{
  while (!(hdr1==NULL || hdr2==NULL))
  {
    if (hdr1!=hdr2)
      return 0;
    hdr1 = hdr1->next;
    hdr2 = hdr2->next;
  }
  if (hdr1==hdr2)
    return 1;
  else
    return 0;
}

//* for assertions.
static int isSameExceptHead(ListNode* hdr, ListNode* hdr_old)
{
  ListNode* hdr_second;
  if(hdr->next == NULL)
  {
    hdr_second = NULL;    
  }
  else
    hdr_second = hdr->next->next;
  return is_same_list(hdr_second, hdr_old);
}

//* for assertions.
static int is_not_circular(ListNode* hdr)
{
  if (hdr==NULL)
    return 1;
  ListNode* fast = hdr->next;
  ListNode* slow = hdr;
  while (!(fast==NULL || slow==NULL))
  {
    if (fast==slow)
      return 0;
    fast = fast->next;
    if (fast==NULL)
      break;
    fast = fast->next;
    slow = slow->next;
  }
  return 1;
}

static void insert(ListNode ** hdr, ListNode * p)
{
  assert(is_not_circular((ListNode*)hdr) && p); //*
  ListNode** hdr_old = hdr;  //* for assertions.
  if (hdr == NULL)
  {
    assert(1); //*
    return;
  }
  ListNode * first_element;
  if(((ListNode *)hdr)->next == NULL)
  {
    first_element = NULL;
  }
  else
    first_element = ((ListNode *)hdr)->next->next;  //* for assertions.
  p->next = *hdr;
  *hdr = p;
  assert(is_not_circular((ListNode*)hdr) && hdr==hdr_old && isSameExceptHead(*hdr,first_element)); //*
}

//* for assertion
static int is_not_present(ListNode* hdr, ListNode* d)
{
  ListNode* p;
  for (p=hdr; p; p=p->next)
  {
    if (p==d)
      return 0;
  }
  return 1;
}

static void delete(ListNode ** hdr, ListNode * d)
{ 
  assert(is_not_circular((ListNode*)hdr) && d); //* doubt
  ListNode *p, *q;

  if (hdr == NULL || *hdr == NULL)
    return;
  for (p = (ListNode *) hdr, q = p->next; q; p = q, q = q->next) {
    if (q == d) {
      p->next = q->next;
      free(q);
      assert(is_not_circular((ListNode*)hdr) && is_not_present((ListNode*)hdr,d)); //*
      return;
    }
  }
  assert(is_not_circular((ListNode*)hdr) && is_not_present((ListNode*)hdr,d)); //*
}

typedef struct AClient {
  struct AClient *next;
  ClientData clientdata;
  CLIENT *callback;		/* rpc.h */
} AClient;

typedef struct ABoard {
  struct ABoard *next;
  /* name of the board is in clients->clientdata.boardnm */
  AClient *clients;		/* list of clients on one board */
  ALine *lines;			/* list of LINEs that it has */
} ABoard;

static ABoard *boards = NULL;	/* list of boards that server has */

/* Find the white board with name nm[].  */

static ABoard *find_wbp(char *nm)
{
  assert(nm); //*
  ABoard *p;

  for (p = boards; p; p = p->next) {
    if (strcmp(nm, p->clients->clientdata.boardnm) == 0)
    {
      break;
    }
  }
  assert(!p || strcmp(nm, p->clients->clientdata.boardnm) == 0); //*
  return p;
}

/*
 * Add a client.  May start a new white board.  We clnt_create once
 * for each client.
 */
int *addclient_1_svc(ClientData * cd, struct svc_req *srq)
{
  assert(cd); //* doubt
  static int result;		/* note: static */
  ABoard *ab = find_wbp(cd->boardnm);
  AClient *q = (AClient *) malloc(sizeof(AClient));

  printf("addclient_1_svc(%p, %p)\n", (void *) cd, (void *) srq);

  if (q == NULL)
    goto error;
  q->clientdata = *cd;
  q->callback =
      clnt_create(cd->machinenm, cd->nprogram, cd->nversion, "tcp");
  if (q->callback == NULL) {
    free(q);
    goto error;
  }
  if (ab == NULL) {
    /* new white board */
    ab = (ABoard *) malloc(sizeof(ABoard));
    if (ab == NULL)
      goto error;
    ab->lines = NULL;
    ab->clients = NULL;
    insert((ListNode **) & boards, (ListNode *) ab);
  }
  insert((ListNode **) & ab->clients, (ListNode *) q);
  result = 0;
  assert(!is_not_present((ListNode*)boards, (ListNode*)ab) && !is_not_present((ListNode*)(ab->clients),(ListNode*)q) && !result); //* doubt
  return &result;
error:
  result = -1;
  assert(result==-1); //* doubt
  return &result;
}

/*
 * Commit suicide!  Unregister yourself. Invoked as SIGALRM handler.
 */
static void die(int dummy)
{
  assert(1); //*
  int x = pmap_unset(WhiteBoardServer, WhiteBoardServerVersion);
  exit(x != 1);
  assert(1); //*
}

static void delboard(ABoard * ab)
{
  assert(!ab);
  ALine *lp, *lq;

  for (lp = ab->lines; lp; lp = lq) {
    lq = lp->next;
    free(lp);
  }
  delete((ListNode **) & boards, (ListNode *) ab);
  assert(is_not_present((ListNode*)boards, (ListNode*)ab) && !ab->lines);
}

/*
 * Delete a client.  If this is the last client on a whiteboard, delete the
 * board too.  If no more boards left, kill yourself.
 */
int *delclient_1_svc(ClientData * cd, struct svc_req *srq)
{
  assert(cd); //* doubt
  static int result;		/* note: static */
  AClient *p;
  ABoard *ab = find_wbp(cd->boardnm);

  if (ab == NULL)
    goto error;

  /* delete the client; search on nprogram and machinenm */
  for (p = ab->clients; p; p = p->next) {
    if (p->clientdata.nprogram == cd->nprogram
	&& strcmp(p->clientdata.machinenm, cd->machinenm) == 0) {
      clnt_destroy(p->callback);
      delete((ListNode **) & ab->clients, (ListNode *) p);
      if (ab->clients == NULL)
      	delboard(ab);
      break;
    }
  }
  if (boards == NULL) {
    /* server has no clients; so die *after* doing a return &i. */
    struct sigaction asigalrm;
    asigalrm.sa_flags = 0;
    asigalrm.sa_handler = die;
    sigemptyset(&asigalrm.sa_mask);
    sigaction(SIGALRM, &asigalrm, 0);	/* install the signal handler */
    alarm(1);			/* invoke die() after 1 second */
  }
  result = 0;
  assert(is_not_present((ListNode*)ab->clients,(ListNode*)p) && (ab->clients || is_not_present((ListNode*)boards,(ListNode*)ab))); //* doubt
  // assert(is_not_present((ListNode*),(ListNode*)) , is_not_present((ListNode*),(ListNode*))); //* doubt
  return &result;
error:
  result = -1;
  assert(1); //* doubt
  return &result;
}

/*
 * A clients gives us a new line.  Get the coordinates of the line and
 * distribute among the clients.
 */
int *addline_1_svc(AddLineArg * ap, struct svc_req *srq)
{
  assert(ap); //*
  static int result;		/* note: static */
  AClient *p;
  ALine *lp = (ALine *) malloc(sizeof(ALine));
  ABoard *ab = find_wbp(ap->clientdata.boardnm);

  if (ab == NULL || lp == NULL)
    goto error;

  /* add the line to the list of lines on this board */
  lp->ln = ap->ln;
  insert((ListNode **) & ab->lines, (ListNode *) lp);

  /* tell all clients on this board of this addition */
  for (p = ab->clients; p; p = p->next) {
    callbackfromwbs_1(&ap->ln, p->callback);
  }
  result = 0;
  assert(!is_not_present((ListNode* )ab->lines,lp)); //*
  return &result;
error:
  result = -1;
  assert(is_not_present((ListNode*)boards,(ListNode*)ab)); //* 
  return &result;
}

/*
 * A client wants to know all the lines present on his white board.
 */
Linep *sendallmylines_1_svc(ClientData * cd, struct svc_req * srq)
{
  assert(cd); //*
  static ALine *lp = NULL;	/* note: static */
  ABoard *ab = find_wbp(cd->boardnm);
  assert(!ab || !is_not_present((ListNode*)boards,(ListNode*)ab)); //*
  return (ab ? &ab->lines : &lp);
}

/* -eof- */
