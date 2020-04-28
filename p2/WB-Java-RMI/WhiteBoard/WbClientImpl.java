// file: WbClientImpl.java by pmateti@wright.edu

package WhiteBoard;

import java.io.*;
import java.util.*;
import java.rmi.*;
import java.awt.Color;

public class WbClientImpl extends java.rmi.server.UnicastRemoteObject implements WbClient {

	private WbServer wbServer;
	private String thisMcnm, myBoardNm, myURL, myServerURL;
	private Color myColor;
	private LinesFrame myLinesFrame;

	// create our lines frame process, which will do recvDisplayObj()
	// doubt.
	private void makeMyLinesFrame(String[] args) throws Exception {
		Invoke.javaVM('L', args[1] + " " + args[2] + " " + thisMcnm + " " + args[0]);
	}


	public WbClientImpl(String[] args) throws Exception {
		// args = [clientId, brdNm, displayMcnm, wbserverURL, color]
		super();

		myBoardNm = args[1];
		myURL = Invoke.makeURL('C', args[0]); // doubt
		Naming.rebind(myURL, this); // doubt
		Invoke.myPrint("WbClientImpl", "did Naming.rebind " + myURL); // doubt

		thisMcnm = java.net.InetAddress.getLocalHost().getHostName();
		makeMyLinesFrame(args);
		myServerURL = args[3];
		wbServer = (WbServer) Invoke.lookup(myServerURL);// doubt
		myColor = new Color(Integer.parseInt(args[4], 16));
		Invoke.myPrint("WbClient waiting for", "recvDisplayObj");
		// addClient() occurs in recvDisplayObj()
	}

	//* method called by a client or wbadmin to receive info about this client as a string.
	public void  sendClientInfo(WbClient wc) throws java.rmi.RemoteException{
		String S = " Client URL: " + myURL + ", client_machine_name: " + thisMcnm + ", board_name: " + myBoardNm + ", my_server_url: " + myServerURL + ", my_color: " + myColor;
		// System.out.println(S);
		wc.recvClientInfo(S);
	}

	//* method called by a server to receive info about this client as a string.
	public void  sendClientInfo(WbServer ws) throws java.rmi.RemoteException{
		String S = " Client URL: " + myURL + ", client_machine_name: " + thisMcnm + ", board_name: " + myBoardNm + ", my_server_url: " + myServerURL + ", my_color: " + myColor;
		// System.out.println(S);
		ws.recvClientInfo(S);
	}

	//* for debugging and query. This method receives info from a client as a string and then prints it. 
	public void recvClientInfo(String S) throws java.rmi.RemoteException      
	{
		System.out.println(S);
	}

	//* debugging function:
	private void printABoard(ABoard board) {
		System.out.println("\tboardname" + board.boardName);
		System.out.println("\tclients on this board:");
		for (WbClient client : board.vClients) {
			System.out.println("\t\t" + client.toString());
		}
		System.out.println("\tlines on this board:");
		for (LineCoords lines : board.vLines) {
			System.out.println("\t\t" + lines.toString());
		}
	}

	//* a function to receive one board from a server.
	public void receiveBoard_q(ABoard board) throws java.rmi.RemoteException {
		System.out.println("in wbadmin: receiveBoard");
		printABoard(board);
	}
	
	//* a function to receive one board from a server.
	public void receiveBoard_t(ABoard board) throws java.rmi.RemoteException {
		System.out.println("in wbadmin: receiveBoard");
		printABoard(board);
	}

	//* this method is called by wbadmin to request this client to update its server as its board has been transferred to the new_server.
	public void updateMyServer(WbServer new_server, String new_url) throws java.rmi.RemoteException
	{
		this.wbServer = new_server;
		this.myURL = new_url; 
	}

	//* This method updates board upon receiving new line from server.
	public void updateBoard(LineCoords ln) throws java.rmi.RemoteException {
		myLinesFrame.recvOneLine(ln); // doubt
	}

	//* this method retrives all lines on myBoardNm from server.
	public void sendAllLines() throws java.rmi.RemoteException {
		wbServer.sendAllLines(this, myBoardNm);// doubt
	}


	// * This method sends new line ln to server.
	public void sendLine(LineCoords ln) {
		ln.c = myColor;
		try {
			wbServer.addLine(ln, myBoardNm);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	// * doubt.
	public void recvDisplayObj(LinesFrame s) {
		Invoke.myPrint("WbClient waiting Ended", "" + s);
		myLinesFrame = s;
		try {
			wbServer.addClient(this, myBoardNm);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	// * This method deletes this client from its server and also delete it
	// completely.
	public void pleaseDie() throws java.rmi.RemoteException {
		try {
			wbServer.delClient(this, myBoardNm);
			Naming.unbind(myURL); // doubt
		} catch (Exception e) {
			e.printStackTrace();
		}
		Invoke.myPrint("WbClient ", myURL + " exits");
		System.exit(0);
	}

	public static void main(String args[]) {
		try {
			WbClientImpl me = new WbClientImpl(args);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

// -eof-
