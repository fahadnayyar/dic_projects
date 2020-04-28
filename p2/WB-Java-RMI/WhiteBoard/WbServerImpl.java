// @file WbServerImpl.java @author pmateti@wright.edu

package WhiteBoard;

import java.io.*;
import java.util.*;
import java.rmi.*;
import java.rmi.server.*;

class ABoard implements java.io.Serializable {
	String boardName; // Name of this board
	Vector<LineCoords> vLines; // all lines on this board
	Vector<WbClient> vClients; // all clients on this board

	public ABoard(String brdnm) {
		boardName = brdnm;
		vLines = new Vector<LineCoords>();
		vClients = new Vector<WbClient>();
	}
}

public class WbServerImpl extends UnicastRemoteObject implements WbServer {

	private Vector<ABoard> vBoards; // all boards on this server
	private String myURL;

	public WbServerImpl(String[] args) throws Exception {
		// args = [serverID, serverMcnm]
		vBoards = new Vector<ABoard>();
		myURL = Invoke.makeURL('S', args[0]); // doubt
		Naming.rebind(myURL, this); // rmi register ourselves // doubt
		Invoke.myPrint("WbServerImpl", myURL + " started");
	}

	// doubt
	// * this method kills this server.SS
	private void pleaseDie() {
		int delay = 5000; // in msec, delayed death
		Timer timer = new Timer();
		timer.schedule(new TimerTask() {
			public void run() {
				try {
					Naming.unbind(myURL);
				} // doubt }
				catch (Exception e) {
					e.printStackTrace();
				}
				Invoke.myPrint("WbServerImpl", myURL + " exits");
				System.exit(0);
			}
		}, delay);

	}

	// * finds and return ABoard with name bardnm.
	private ABoard findAboard(String brdnm) {
		for (Enumeration e = vBoards.elements(); e.hasMoreElements();) {
			ABoard b = (ABoard) e.nextElement();
			if (brdnm.equals(b.boardName))
				return b;
		}
		return null;
	}

	//* rhis method is called by wbadmin requesting this server to send all its boards for query purposes.
	public void sendAllBoards_q(WbClient wa) throws java.rmi.RemoteException {
		// System.out.println("in server, sendAllBoards_q");
		for (Enumeration e = vBoards.elements(); e.hasMoreElements();) {
			ABoard b = (ABoard) e.nextElement();
			try {
				wa.receiveBoard_q(b);
			} // imp
			catch (Exception x) {
				x.printStackTrace();
			}
		}
	}

	//* debugging function:
	private void printABoard(ABoard board) throws java.rmi.RemoteException {
		System.out.println("\tboardname: \n" + board.boardName);
		System.out.println("\tclients on this board:\n");
		for (WbClient client : board.vClients) {
			// System.out.println(client.toString());
			System.out.print("\t\t");
			client.sendClientInfo(this);
		}
		System.out.println("\tlines on this board:\n");
		for (LineCoords lines : board.vLines) {
			System.out.println("\t\t" + lines.toString());
		}
	}

	//* method called by a client or wbadmin to receive info about this client as a string.
	public void recvClientInfo(String S) throws java.rmi.RemoteException      
	{
		System.out.println(S);
	}

	//* this method is called by wbadmin to send new board to this server.
	public void receiveBoard_t(ABoard board) throws java.rmi.RemoteException
	{
		System.out.println("IN NEW_SERVER: board received is: ");
		printABoard(board);
		vBoards.addElement(board);
	}


	//* this method is called by wbadmin requesting this server to send board (with name brdnm) to wa (actaully wbadmin).
	public void sendBoard_t(WbClient wa, String brdnm) throws java.rmi.RemoteException {
		// System.out.println("in server, sendAllBoards_q");
		ABoard board = findAboard(brdnm);
		if (board!=null)
		{
			System.out.println("board in source server: ");
			printABoard(board);
			try {
				wa.receiveBoard_t(board);
				vBoards.remove(board);
			} // imp
			catch (Exception x) {
				x.printStackTrace();
			}
		}else
		{
			System.out.println("ERROR: No board with name " + brdnm +" is present on thsi server!");
		}
	}

	// * rmi public method called by client. This sends all the lines on board brdnm
	// to WbClient wc.
	public void sendAllLines(WbClient wc, String brdnm) throws java.rmi.RemoteException {
		ABoard ab = findAboard(brdnm);
		sendAllLines(wc, ab);
	}

	// * This sends all the lines of board ab to Wbclient wc.
	private void sendAllLines(WbClient wc, ABoard ab) {
		for (Enumeration e = ab.vLines.elements(); e.hasMoreElements();) {
			try {
				wc.updateBoard((LineCoords) e.nextElement());
			} catch (Exception x) {
				x.printStackTrace();
			}
		}
	}

	// * rmi public method called by client. This adds client wc from this server.
	public void addClient(WbClient wc, String brdnm) throws java.rmi.RemoteException {
		ABoard ab = findAboard(brdnm);
		if (ab == null) {
			ab = new ABoard(brdnm);
			vBoards.addElement(ab);
		} else {
			sendAllLines(wc, ab); // new client on an old board
		}
		ab.vClients.addElement(wc);
	}

	// * rmi public method called by client. This deletes client wc from this
	// server. Maybe called on right click on wb window.
	public void delClient(WbClient wc, String brdnm) throws java.rmi.RemoteException {
		ABoard ab = findAboard(brdnm);
		if (ab == null)
			return;

		ab.vClients.removeElement(wc);

		// If this is the last client in board, delete board.
		if (ab.vClients.size() == 0)
			vBoards.removeElement(ab);

		// If this was the last board, terminate this server.
		if (vBoards.size() == 0)
			pleaseDie();
	}

	// * rmi public method called by client. This adds line ln to board boardnm when
	// a client draws a new line.
	public void addLine(LineCoords ln, String brdnm) throws java.rmi.RemoteException {
		ABoard ab = findAboard(brdnm);
		if (ab == null)
			return;

		ab.vLines.addElement(ln);

		// Broadcast to all the clients on this board.
		for (Enumeration e = ab.vClients.elements(); e.hasMoreElements();) {
			WbClient wc = (WbClient) e.nextElement();
			try {
				wc.updateBoard(ln);
			} catch (Exception x) {
				x.printStackTrace();
			}
		}
	}

	public static void main(String args[]) {
		try {
			WbServerImpl wsi = new WbServerImpl(args);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}

// -eof-
