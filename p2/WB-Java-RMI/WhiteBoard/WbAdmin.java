// @file WbAdmin.java @author pmateti@wright.edu
// This is a "shell" for the WhiteBoard project of CEG 7370

package WhiteBoard;

import java.io.*;
import java.util.*;
import java.rmi.*;
import java.awt.Color;

//* made wbadmin exactly as wbclient [same packages included and same superclasses and or interfaces].
public class WbAdmin extends java.rmi.server.UnicastRemoteObject implements WbClient {

	// * dummy/useless methods for interface wbclient:

	public void sendAllLines() throws java.rmi.RemoteException {}
	public void sendLine(LineCoords ln) {}
	public void pleaseDie() throws java.rmi.RemoteException {}
	public void recvDisplayObj(LinesFrame s) {}
	public void updateBoard(LineCoords ln) throws java.rmi.RemoteException {}
	public void  sendClientInfo(WbServer wc) throws java.rmi.RemoteException{}
	public void  sendClientInfo(WbClient wc) throws java.rmi.RemoteException{}
	public void updateMyServer(WbServer new_server, String new_url) throws java.rmi.RemoteException{}

	//* this vector stores reference to all servers created by this wbadmin interface.
	private Vector<WbServer> vServers;
	//* this vector stores url of all servers created by this wbadmin interface.
	private Vector<String> serversUrl;
	//* acts as a global variable used during transferring of board.
	private String to_URL;

	private static final String menu = "\nWbAdmin: create a "
			+ "[s]erver, [a]dd client, [q]uery, query[u]rl, [t]ransfer, e[x]it";

	public WbAdmin() throws Exception {
		vServers = new Vector<WbServer>();
		serversUrl = new Vector<String>();
		to_URL = null;
	}

	//* modified. Now after creating a new server, its reference and url is stored in private vectors.
	private void serverCreate() {
		String args = Invoke.promptAndGet("ServerMachineName");
		String url = Invoke.javaVM('S', args);
		System.out.println("url of new server is: " + url);
		//* sleeping for 5 secs to make sure that the new server is up by the time we try to loopup it via url.
		try {
			Thread.sleep(5000); // sleep for 5 seconds before establishing connection to the new server.
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		serversUrl.add(url);
		WbServer new_server = (WbServer) Invoke.lookup(url);// doubt
		vServers.add(new_server);
	}

	private void addClientReq() {
		String args = Invoke.promptAndGet("BoardName DisplayOn ServerURL color");
		Invoke.javaVM('C', args);
	}

	//* staring method for a transfer request. It makes connection to from_server and request it to send thr board to transfer.
	//* the from_server in turins call our receiveBoard_t method to send the board to transfer.
	private void transferReq() throws java.rmi.RemoteException{
		// Transfer a white board to a new server. For you TODO

		String arg = Invoke.promptAndGet("URL_from BoardName URL_to");
		String args[] = arg.split(" ");
		// System.out.println(" url_from: " + args[0] + ", board_name: "+ args[1] + ", url_to: " + args[2]);
		
		WbServer from_server = (WbServer) Invoke.lookup(args[0]); // doubt
		this.to_URL = args[2];
		from_server.sendBoard_t(this, args[1]);
	}

	//* a function to receive one board from server.
	//* In this method a new connection is made to the to_server and receiveBoard_t method of to_server is called. 
	//* Also for all the clients on the transferred board, updateMyServer is called to ask the cliet to update the server to to_server.
	public void receiveBoard_t(ABoard board) throws java.rmi.RemoteException {
			//* debugging.	
			// System.out.println("IN WBADMIN: received board to transfer:");
			// printABoard(board);
		WbServer to_server = (WbServer) Invoke.lookup(this.to_URL); // doubt
		if ((to_server) != null)
		{
			to_server.receiveBoard_t(board);	
			for (WbClient client: board.vClients)
			{
				client.updateMyServer(to_server, to_URL);
			}	
		}else
		{
			System.out.println("ERROR: cannot connect to target server!");			
		}
	}

	// * debugging function: prints all the clients and lines of board.
	private void printABoard(ABoard board) throws java.rmi.RemoteException {
		System.out.println("\tboardname: " + board.boardName);
		System.out.println("\tclients on this board:");
		for (WbClient client : board.vClients) {
			System.out.print("\t\t");
			client.sendClientInfo(this);
		}
		System.out.println();
		// System.out.println("\tlines on this board:\n");
		// for (LineCoords lines : board.vLines) {
		// 	System.out.println("\t\t" + lines.toString());
		// }
	}

	//* for debugging and query. This method receives info from a client as a string and then prints it. 
	public void recvClientInfo(String S) throws java.rmi.RemoteException      
	{
		System.out.println(S);
	}
	

	// * a function to receive one board from a server just to print the board for query.
	public void receiveBoard_q(ABoard board) throws java.rmi.RemoteException {
		// System.out.println("in wbadmin: receiveBoard_q");
		printABoard(board);
	}


	// * a function to query a server by its url.
	private void queryUrl() throws java.rmi.RemoteException {
		String args = Invoke.promptAndGet("ServerURL");
		String url = args;
		System.out.println(url);
		WbServer new_server = (WbServer) Invoke.lookup(url);// doubt
		queryServer(new_server, url);
	}

	//* A function to query server whose URL is "url".
	private void queryServer(WbServer server, String url) throws java.rmi.RemoteException {
		System.out.println("\t\t\t***------ information about Server at url: " + url+" -----***");
		server.sendAllBoards_q(this); // imp.
	}

	//* This will query all the servers which were created via this WbAdmin interface only.
	private void queryReq() throws java.rmi.RemoteException {
		// Query for inforamtion from each server. For you TODO
		int index = 0;
		for (WbServer server : vServers) {
			queryServer(server, serversUrl.get(index));
			index++;
		}
	}

	private void userInteract() throws java.rmi.RemoteException {
		while (true) {
			String choice = Invoke.promptAndGet(menu);
			switch (choice.charAt(0)) {
				case 's':
					serverCreate();
					break;
				case 'a':
					addClientReq();
					break;
				case 'q':
					queryReq();
					break;
				case 'u':
					queryUrl();
					break;
				case 't':
					transferReq();
					break;
				case 'x':
					System.exit(0);
					break;
			}
		}
	}

	public static void main(String[] args) throws java.rmi.RemoteException {
		try {
			WbAdmin wa = new WbAdmin();
			wa.userInteract();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}
}

// -eof-
