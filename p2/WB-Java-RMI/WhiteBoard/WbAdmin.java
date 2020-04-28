// @file WbAdmin.java @author pmateti@wright.edu
// This is a "shell" for the WhiteBoard project of CEG 7370

package WhiteBoard;

import java.io.*;
import java.util.*;
import java.rmi.*;
import java.awt.Color;

//* making wbadmin exactly as wbclient [same packages included and same super types]

public class WbAdmin extends java.rmi.server.UnicastRemoteObject implements WbClient {

	// * dummy methods for interface wbclient:

	// * this method retrives all lines on myBoardNm from server.
	public void sendAllLines() throws java.rmi.RemoteException {
		// wbServer.sendAllLines(this, myBoardNm);// doubt
	}

	// * This method sends new line ln to server.
	public void sendLine(LineCoords ln) {
		// ln.c = myColor;
		// try {wbServer.addLine(ln, myBoardNm);}
		// catch (Exception e) {e.printStackTrace();}
	}

	// * This method deletes this client from its server and also delete it
	// completely.
	public void pleaseDie() throws java.rmi.RemoteException {
		// try {
		// wbServer.delClient(this, myBoardNm);
		// Naming.unbind(myURL); // doubt
		// } catch (Exception e) {e.printStackTrace();}
		// Invoke.myPrint("WbClient ", myURL + " exits");
		// System.exit(0);
	}

	// * doubt.
	public void recvDisplayObj(LinesFrame s) {
		// Invoke.myPrint("WbClient waiting Ended", "" + s);
		// myLinesFrame = s;
		// try {
		// wbServer.addClient(this, myBoardNm);
		// } catch (Exception e) {
		// e.printStackTrace();
		// }
	}

	// this comes from wbServer
	// * This method updates board upon receiving new line from server via
	// mylinesFrame.
	public void updateBoard(LineCoords ln) throws java.rmi.RemoteException {
		// myLinesFrame.recvOneLine(ln); // doubt
	}

	public void  sendClientInfo(WbServer wc) throws java.rmi.RemoteException{
		// String S = ""; // " Client URL: " + myURL + " client_machine_name: " + thisMcnm + " board_name: " + myBoardNm + "my_server_url: " + myServerURL + " my_color: " + myColor;
		// // System.out.println(S);
		// wc.recvClientInfo(S);
	}

	public void  sendClientInfo(WbClient wc) throws java.rmi.RemoteException{
		// String S = ""; // " Client URL: " + myURL + " client_machine_name: " + thisMcnm + " board_name: " + myBoardNm + "my_server_url: " + myServerURL + " my_color: " + myColor;
		// // System.out.println(S);
		// wc.recvClientInfo(S);
	}
	
	public void updateMyServer(WbServer new_server, String new_url) throws java.rmi.RemoteException
	{
		// this.wbServer = new_server;
		// this.myURL = new_url; 
	}

    public void recvClientInfo(String S) throws java.rmi.RemoteException      
	{
		System.out.println(S);
	}


	private Vector<WbServer> vServers;
	private Vector<String> serversUrl;
	// private ABoard board_to_send;
	private String to_URL;

	private static final String menu = "\nWbAdmin: create a "
			+ "[s]erver, [a]dd client, [q]uery, query[u]rl, [t]ransfer, e[x]it";

	public WbAdmin() throws Exception {
		vServers = new Vector<WbServer>();
		serversUrl = new Vector<String>();
	}

	private void serverCreate() {
		String args = Invoke.promptAndGet("ServerMachineName");
		String url = Invoke.javaVM('S', args);
		System.out.println("url of new server is: " + url);
		try {
			Thread.sleep(5000); // sleep for 5 seconds before establishing connection to the new server.
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		WbServer new_server = (WbServer) Invoke.lookup(url);// doubt
		vServers.add(new_server);
		serversUrl.add(url);
	}

	private void addClientReq() {
		String args = Invoke.promptAndGet("BoardName DisplayOn ServerURL color");
		Invoke.javaVM('C', args);
	}

	private void transferReq() throws java.rmi.RemoteException{
		// Transfer a white board to a new server. For you TODO

		
		String arg = Invoke.promptAndGet("URL_from BoardName URL_to");
		String args[] = arg.split(" ");
		// System.out.println(" url_from: " + args[0] + ", board_name: "+ args[1] + ", url_to: " + args[2]);
		
		WbServer from_server = (WbServer) Invoke.lookup(args[0]); // doubt
		this.to_URL = args[2];
		// WbServer to_server = (WbServer) Invoke.lookup(args[1]); // doubt

		from_server.sendBoard_t(this, args[1]);
	}

	// * debugging function:
	private void printABoard(ABoard board) throws java.rmi.RemoteException{
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

	// * a function to receive one board from a server.
	public void receiveBoard_q(ABoard board) throws java.rmi.RemoteException {
		// System.out.println("in wbadmin: receiveBoard_q");
		printABoard(board);
	}

	// * a function to receive one board from a server.
	public void receiveBoard_t(ABoard board) throws java.rmi.RemoteException {
		//* debussing.	
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

	//* This will query all the servers which were created via this WbAdmin
	// interface only.
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
