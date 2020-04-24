
/* server.c */

#include "wb.h"
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

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
  // assert(is_not_circular((ListNode*)hdr) && p); //*
  ListNode** hdr_old = hdr;  //* for assertions.
  if (hdr == NULL)
  {
    // assert(1); //*
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
  // assert(is_not_circular((ListNode*)hdr) && hdr==hdr_old && isSameExceptHead(*hdr,first_element)); //*
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
  // assert(is_not_circular((ListNode*)hdr) && d); //* doubt
  ListNode *p, *q;

  if (hdr == NULL || *hdr == NULL)
    return;
  for (p = (ListNode *) hdr, q = p->next; q; p = q, q = q->next) {
    if (q == d) {
      p->next = q->next;
      free(q);
      // assert(is_not_circular((ListNode*)hdr) && is_not_present((ListNode*)hdr,d)); //*
      return;
    }
  }
  // assert(is_not_circular((ListNode*)hdr) && is_not_present((ListNode*)hdr,d)); //*
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
  // assert(nm); //*
  ABoard *p;

  for (p = boards; p; p = p->next) {
    if (strcmp(nm, p->clients->clientdata.boardnm) == 0)
    {
      break;
    }
  }
  // assert(!p || strcmp(nm, p->clients->clientdata.boardnm) == 0); //*
  return p;
}

/*
 * Add a client.  May start a new white board.  We clnt_create once
 * for each client.
 */
int *addclient_1_svc(ClientData * cd, struct svc_req *srq)
{
  // assert(cd); //* doubt
  static int result;		/* note: static */
  ABoard *ab = find_wbp(cd->boardnm);
  AClient *q = (AClient *) malloc(sizeof(AClient));

  printf("addclient_1_svc(%p, %p)\n", (void *) cd, (void *) srq);

  if (q == NULL)
    goto error;
  q->clientdata = *cd;
  printf("asli: %s, %d, %d\n", cd->machinenm, cd->nprogram, cd->nversion);
  q->callback =
      clnt_create(cd->machinenm, cd->nprogram, cd->nversion, "tcp"); // doubt
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
  // assert(!is_not_present((ListNode*)boards, (ListNode*)ab) && !is_not_present((ListNode*)(ab->clients),(ListNode*)q) && !result); //* doubt
  return &result;
error:
  result = -1;
  // assert(result==-1); //* doubt
  return &result;
}

/*
 * Commit suicide!  Unregister yourself. Invoked as SIGALRM handler.
 */
static void die(int dummy)
{
  // assert(1); //*
  int x = pmap_unset(WhiteBoardServer, WhiteBoardServerVersion); // doubt
  exit(x != 1);
  // assert(1); //*
}

static void delboard(ABoard * ab)
{
  // assert(ab);
  ALine *lp, *lq;

  for (lp = ab->lines; lp; lp = lq) {
    lq = lp->next;
    free(lp);
  }
  delete((ListNode **) & boards, (ListNode *) ab);
  // assert(is_not_present((ListNode*)boards, (ListNode*)ab) && !ab->lines);
}

/*
 * Delete a client.  If this is the last client on a whiteboard, delete the
 * board too.  If no more boards left, kill yourself.
 */
int *delclient_1_svc(ClientData * cd, struct svc_req *srq)
{
  // assert(cd); //* doubt
  static int result;		/* note: static */
  AClient *p;
  ABoard *ab = find_wbp(cd->boardnm);

  if (ab == NULL)
    goto error;

  /* delete the client; search on nprogram and machinenm */
  for (p = ab->clients; p; p = p->next) {
    if (p->clientdata.nprogram == cd->nprogram
	&& strcmp(p->clientdata.machinenm, cd->machinenm) == 0) {
      clnt_destroy(p->callback); // doubt
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
  // assert(1); //* doubt
  return &result;
}

/*
 * A clients gives us a new line.  Get the coordinates of the line and
 * distribute among the clients.
 */
int *addline_1_svc(AddLineArg * ap, struct svc_req *srq)
{
  // assert(ap); //*
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
    callbackfromwbs_1(&ap->ln, p->callback); // doubt
  }
  result = 0;
  // assert(!is_not_present((ListNode* )ab->lines,lp)); //*
  return &result;
error:
  result = -1;
  // assert(is_not_present((ListNode*)boards,(ListNode*)ab)); //* 
  return &result;
}

/*
 * A client wants to know all the lines present on his white board.
 */
Linep *sendallmylines_1_svc(ClientData * cd, struct svc_req * srq)
{
  // assert(cd); //*
  static ALine *lp = NULL;	/* note: static */
  ABoard *ab = find_wbp(cd->boardnm);
  // assesrt(!ab || !is_not_present((ListNode*)boards,(ListNode*)ab)); //*
  return (ab ? &ab->lines : &lp);
}


/*
 * A client wants to query all the boards present on this server along with all the 
 * clients and lines on that board.
 */
struct BBoard * query_1_svc(int * unused, struct svc_req * srq) 
{  
  //* debugging.
  printf("in server: query_1_svc\n");
  
  //* initializing current_board_head and current_board_tail.
  struct BBoard *current_board_head = (struct BBoard *) malloc(sizeof(struct BBoard));  
  current_board_head->clients = NULL; current_board_head->lines = NULL; current_board_head->next = NULL;
  struct BBoard *current_board_tail = current_board_head;
  
  //* debugging.
  printf("%p\n", current_board_head);
  
  //* loop to copy boards into current_board_head.
  ABoard *p;
  for (p = boards; p; p = p->next) 
  {
    //* debugging.
    // printf("1st for loop\n");
    
    //* initializing current_board.
    struct BBoard * current_board = (struct BBoard *) malloc(sizeof(struct BBoard));
    current_board->next = NULL; current_board->lines = NULL; current_board->clients = NULL;
    
    //* initializing current_client_head and current_client_tail.
    struct BClient * current_client_head = (struct BCLient *) malloc(sizeof(struct BClient)); 
    current_client_head->next = NULL; current_client_head->clientdata = NULL;
    struct BClient * current_client_tail = current_client_head;
    
    //* loop to copy p->clients to current_board->clients.
    AClient * q;
    for (q = p->clients ; q ; q=q->next)
    { 
      //* debugging.
      // printf("2nd for loop\n");
      
      //* initializing current_client.
      struct BClient * current_client = (struct BCLient *) malloc(sizeof(struct BClient));  
      current_client->next = NULL ; current_client->clientdata = NULL;
      
      //* initializing current_client_data and copying q->clientdata to current_client->clientdata. 
      ClientData * current_client_data = (ClientData*)malloc(sizeof(ClientData)); // initialization?
      strcpy(current_client_data->boardnm, q->clientdata.boardnm);
      strcpy(current_client_data->machinenm, q->clientdata.machinenm);
      strcpy(current_client_data->xdisplaynm, q->clientdata.xdisplaynm);
      current_client_data->color = q->clientdata.color;
      current_client_data->nprogram = q->clientdata.nprogram;
      current_client_data->nversion = q->clientdata.nversion;
      
      //* strttign clientdata of current_client.
      current_client->clientdata = current_client_data;

      //* putting current client at the end of client linked list.
      current_client_tail->next = current_client;
      current_client_tail = current_client;
    }
    
    //* putting client_client_head->next to current_board->clients.
    current_client_head = current_client_head->next;
    current_board->clients = current_client_head;
    
    //* debugging.
    printf("clients copied!\n");

            // * initializing current_line_head and current_line_tail.
            struct ALine * current_line_head = (struct ALine*) malloc(sizeof(struct ALine));
            current_line_head->next = NULL; 
            current_line_head->ln.x1=0; 
            current_line_head->ln.x2=0; 
            current_line_head->ln.y1=0; 
            current_line_head->ln.y2=0; 
            current_line_head->ln.color=0;
            struct ALine * current_line_tail = current_line_head;

            //* for loop to copy p->lines to current_board->lines. 
            ALine *r;
            for (r = p->lines; r ; r=r->next)
            {
              //* debugging.
              // printf("3rd for loop\n");
              
              //* initializing current_line.
              struct ALine * current_line = (struct ALine*) malloc(sizeof(struct ALine));    
              current_line->next = NULL; //current_line->ln = NULL; // doubt
              current_line->ln.x1 = r->ln.x1; 
              current_line->ln.x2 = r->ln.x2; 
              current_line->ln.y1 = r->ln.y1; 
              current_line->ln.y2 = r->ln.y2;
              current_line->ln.color = r->ln.color;
              
              //* putting current line at the end of line linked list.
              current_line_tail->next = current_line;
              current_line_tail = current_line;
            }
            
            //* putting current_line_head-> next into current_board->lines.
            current_line_head = current_line_head->next;
            current_board->lines = current_line_head;
            
            //* debugging.
            // printf("lines copied!\n");

    //* putting current_board in the end of current_board linked list.  
    current_board_tail->next = current_board;
    current_board_tail = current_board;
  }
  

  //* debugging.
  printf("boards copied!\n");

  //* correcting dummy current_board_head.
  current_board_head = current_board_head->next;
  
  return current_board_head;
}

int getTransientProgNumber(int version);

/*
 * A client wants to start a new server on this machine.
 */
int* newserver_1_svc(struct NewServerMcNm * nsmn, struct svc_req * srq) 
{
    static int result;
    char newServerMachineName[50]; 
    strcpy(newServerMachineName, nsmn->newServerMachineName);
   
    
    // char hostbuffer[256]; // doubt 256? 
    // char *IPbuffer; 
    // struct hostent *host_entry; 
    // int hostname; 
  
    // // To retrieve hostname 
    // hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
    // // checkHostName(hostname); 
  
    // // To retrieve host information 
    // host_entry = gethostbyname(hostbuffer); 
    // // checkHostEntry(host_entry); 
  
    // // To convert an Internet network 
    // // address into ASCII string 
    // IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); 
  
    // printf("Hostname: %s\n", hostbuffer); 
    // printf("Host IP: %s\n", IPbuffer); 
 
 
 
 
  // char myMachineNm[50];
  // gethostname(myMachineNm, sizeof(myMachineNm));
  // printf("IPbuffer: %s, newServerMachineName: %s\n", IPbuffer, newServerMachineName);
  // if (strcmp(newServerMachineName, IPbuffer ) ==0 ) // doubt. How to correct?
  {
    int  newprognum = getTransientProgNumber(WhiteBoardServerVersion);
    int id = fork();
    if (id==0)
    {
      // child process.
      char parameter[50]; 
      sprintf(parameter, "%d", newprognum); 
      printf("prigramnum in child process: %s\n", parameter);
      char* arr[] = {"server7370", parameter, NULL};
      execv("./server7370", arr);
      assert(0);
    }
    printf("programnum in parent process: %d\n", newprognum);
    result = newprognum;
    return &result;
  }
  
  // printf("ERROR: cannot start new server on a different machine!\n");
  
  result = -1;
  return &result;
  
}

//* for debugging.
void print_bboard(BBoard* board)
{
  printf("clinets:\n");
  BClient * cl;
  for (cl=board->clients; cl; cl=cl->next)
  {
    printf("board: %s, ip: %s, prognum: %d, color: %ld\n",cl->clientdata->boardnm, cl->clientdata->machinenm, cl->clientdata->nprogram, cl->clientdata->color);
  }
  printf("lines:\n");
  ALine * ll;
  for(ll=board->lines; ll; ll=ll->next)
  {
    printf("x1: %d, x2: %d, y1: %d, y2: %d, color: %ld\n",ll->ln.x1, ll->ln.x2, ll->ln.y1, ll->ln.y2, ll->ln.color);
  }
}

//* for debugging.
void print_aboard(ABoard* board)
{
  printf("clinets:\n");
  AClient * cl;
  for (cl=board->clients; cl; cl=cl->next)
  {
    printf("board: %s, ip: %s, prognum: %d, color: %ld, clp: %p\n",cl->clientdata.boardnm, cl->clientdata.machinenm, cl->clientdata.nprogram, cl->clientdata.color, cl->callback);
  }
  printf("lines:\n");
  ALine * ll;
  for(ll=board->lines; ll; ll=ll->next)
  {
    printf("x1: %d, x2: %d, y1: %d, y2: %d, color: %ld\n",ll->ln.x1, ll->ln.x2, ll->ln.y1, ll->ln.y2, ll->ln.color);
  }
}

/*
 * An admin client wants to transfer a board from this server to another server.
 */
int* transferwhiteboard_1_svc(struct XferWBArg * xa, struct svc_req * srq) 
{
  //* return variable. 
  static int result;
  
  //* debugging.
  printf("in transferwhiteboard_1_svc in server: xa.boardname: %s, xa.machinemane: %s, xa.nprogram: %d, xa.nversion: %d\n", xa->boardnm, xa->machinenm, xa->nprogram, xa->nversion );
  
  //* making copy of xa into xa_copy.
  struct XferWBArg * xa_copy = (struct XferWBArg *) malloc(sizeof(struct XferWBArg));
  strcpy(xa_copy->boardnm , xa->boardnm);
  strcpy(xa_copy->machinenm, xa->machinenm);
  xa_copy->nprogram = xa->nprogram; // doubt.
  xa_copy->nversion = xa->nversion; // doubt.
  
  //* extracting board to transfer.
  ABoard *board_to_transfer =  find_wbp(xa->boardnm);
  printf("board in original server:\n");
  print_aboard(board_to_transfer);
  

  //* telling all the clients in this board to start new connection to new server and delete connection to this server.
  if (board_to_transfer)
  {
    AClient * q = board_to_transfer->clients;
    for (;q ; q=q->next)
    {
      printf("ravan : %d\n", q->clientdata.nprogram);
      addnewserverconnection_1(xa_copy, q->callback);
    }
  }
  //* debugging.
  printf("requested the clients to register to the new server.\n");


  //* copying Aboard board_to_transfer to Bboard actual_board_to_transfer.
  //* initializing actual_board_to_transfer. 
  BBoard *actual_board_to_transfer = (struct BBoard *)malloc(sizeof(struct BBoard));
  actual_board_to_transfer->next = NULL;
  actual_board_to_transfer->lines = NULL;
  actual_board_to_transfer->clients = NULL;
  
  //* initializing client_head and client_tail.
  BClient* client_head = (BClient*)malloc(sizeof(BClient));
  client_head->next = NULL; client_head->clientdata = NULL; 
  BClient* client_tail = client_head;
  //* for loop to copy board_to_transfer->clients to actual_board_to_transfer->clients. 
  AClient * cl;
  for (cl = board_to_transfer->clients; cl; cl=cl->next)
  {
    BClient* client_current = (BClient*)malloc(sizeof(BClient));
    client_current->next = NULL;// client_current->clientdata=NULL;
    client_current->clientdata = (ClientData*)malloc(sizeof(ClientData));
    strcpy(client_current->clientdata->boardnm, cl->clientdata.boardnm);
    strcpy(client_current->clientdata->machinenm, cl->clientdata.machinenm);
    strcpy(client_current->clientdata->xdisplaynm, cl->clientdata.xdisplaynm);
    client_current->clientdata->color = cl->clientdata.color;
    client_current->clientdata->nprogram = cl->clientdata.nprogram;
    client_current->clientdata->nversion = cl->clientdata.nversion;

    //* closing rpc handle of this server to the clients.
    printf("cnlt_destry in original server: %s %s %d %d %ld %p\n", cl->clientdata.boardnm, cl->clientdata.machinenm, cl->clientdata.nprogram, cl->clientdata.nversion, cl->clientdata.color, cl->callback);
    clnt_destroy(cl->callback);

    client_tail->next = client_current;
    client_tail = client_current;
  }
  actual_board_to_transfer->clients = client_head->next;

  //* initializing line_head and line_tail.
  ALine* line_head = (ALine*)malloc(sizeof(ALine));
  line_head->next = NULL;
  line_head->ln.x1 = 0;
  line_head->ln.x2 = 0;
  line_head->ln.y1 = 0;
  line_head->ln.y2 = 0;
  line_head->ln.color = 0;
  ALine* line_tail = line_head;
  //* for loop to copy board_to_transfer->lines to actual_board_to_transfer->lines. 
  ALine* ll;
  for(ll=board_to_transfer->lines; ll ; ll=ll->next)
  {
    ALine* line_current = (ALine*)malloc(sizeof(ALine));
    line_current->next = NULL;
    line_current->ln.x1 = ll->ln.x1;
    line_current->ln.x2 = ll->ln.x2;
    line_current->ln.y1 = ll->ln.y1;
    line_current->ln.y2 = ll->ln.y2;
    line_current->ln.color = ll->ln.color;

    line_tail->next = line_current;
    line_tail = line_current;
  }
  actual_board_to_transfer->lines = line_head->next; 

  //* debugging.
  printf("actual_board_to_transfer in original server:\n");
  print_bboard(actual_board_to_transfer);
   
  //* opening a new rpc connection to new_server and transferring the bboard actual_board_to_transfer.
  CLIENT * clp_new_server = clnt_create(xa->machinenm, xa->nprogram, xa->nversion, "tcp");  
  result = receivenewwhiteboard_1(actual_board_to_transfer, clp_new_server);
  clnt_destroy(clp_new_server);

  //* debugging.
  printf("sent the board to new server.\n");

  //* deleting board from this server.
  delboard(board_to_transfer);
  printf("board deleted from original server\n");

  //* returing correct error code.
  result = 0;
  return &result;
}

/*
 * A server wants to send one board to this server.
 * This server should establish connection to all clients in that board 
 * and also add the board in its boards list.
 */
int* receivenewwhiteboard_1_svc(struct BBoard * wb, struct svc_req * srq) 
{
  //* return variable. 
  static int result;
  result = - 1;
  
  //* debussing.
  printf("board received in new server\n");
  print_bboard(wb);
  

  //* copying Bboard wb to Aboard board_to_insert.
  //* initializing board_to_insert. 
  ABoard *board_to_insert = (struct ABoard *)malloc(sizeof(struct ABoard));
  board_to_insert->next = NULL;
  board_to_insert->lines = NULL;
  board_to_insert->clients = NULL;
  
  //* initializing client_head and client_tail.
  AClient* client_head = (AClient*)malloc(sizeof(AClient));
  client_head->next = NULL; client_head->callback = NULL; //client_head->clientdata = NULL; // doubt.
  client_head->clientdata.color = 0;
  client_head->clientdata.nprogram = 0;
  client_head->clientdata.nversion = 0;
  strcpy(client_head->clientdata.machinenm,"\0");
  strcpy(client_head->clientdata.boardnm,"\0");
  strcpy(client_head->clientdata.xdisplaynm,"\0");
  BClient* client_tail = client_head;
  //* for loop to copy board_to_transfer->clients to actual_board_to_transfer->clients. 
  BClient * cl;
  for (cl = wb->clients; cl; cl=cl->next)
  {
    AClient* client_current = (AClient*)malloc(sizeof(AClient));
    client_current->next = NULL; client_current->callback=NULL; // client_current->clientdata=NULL;
    // client_current->clientdata = (ClientData*)malloc(sizeof(ClientData));
    strcpy(client_current->clientdata.boardnm, cl->clientdata->boardnm);
    strcpy(client_current->clientdata.machinenm, cl->clientdata->machinenm);
    strcpy(client_current->clientdata.xdisplaynm, cl->clientdata->xdisplaynm);
    client_current->clientdata.color = cl->clientdata->color;
    client_current->clientdata.nprogram = cl->clientdata->nprogram;
    client_current->clientdata.nversion = cl->clientdata->nversion;
    
    //* making a new rpc handle to client.
    printf("hi1\n");
    printf("clnt_create in new server: %s %s %d %d %ld %p\n", client_current->clientdata.boardnm ,client_current->clientdata.machinenm, client_current->clientdata.nprogram, client_current->clientdata.nversion, client_current->clientdata.color, client_current->callback);
    client_current->callback =  clnt_create(client_current->clientdata.machinenm, client_current->clientdata.nprogram, client_current->clientdata.nversion, "tcp");
    printf("new callbak: %p\n", client_current->callback);
    printf("bye1\n");

    client_tail->next = client_current;
    client_tail = client_current;
  }
  board_to_insert->clients = client_head->next;

  //* initializing line_head and line_tail.
  ALine* line_head = (ALine*)malloc(sizeof(ALine));
  line_head->next = NULL;
  line_head->ln.x1 = 0;
  line_head->ln.x2 = 0;
  line_head->ln.y1 = 0;
  line_head->ln.y2 = 0;
  line_head->ln.color = 0;
  ALine* line_tail = line_head;
  //* for loop to copy board_to_transfer->lines to actual_board_to_transfer->lines. 
  ALine* ll;
  for(ll=wb->lines; ll ; ll=ll->next)
  {
    ALine* line_current = (ALine*)malloc(sizeof(ALine));
    line_current->next = NULL;
    line_current->ln.x1 = ll->ln.x1;
    line_current->ln.x2 = ll->ln.x2;
    line_current->ln.y1 = ll->ln.y1;
    line_current->ln.y2 = ll->ln.y2;
    line_current->ln.color = ll->ln.color;

    line_tail->next = line_current;
    line_tail = line_current;
  }
  board_to_insert->lines = line_head->next; 

  //* debugging.
  printf("aboard_to_insert in new server:\n");
  print_aboard(board_to_insert);


  //* inserting the board received into boards.
  insert((ListNode **) & boards, (ListNode *) board_to_insert);

  //* debugging.
  printf("received board inserted into boards list.\n");

  //* returing correct error code.
  result = 0;
  return &result;
}

/* -eof- */
