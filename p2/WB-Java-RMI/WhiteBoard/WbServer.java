// @file WbServer.java @author pmateti@wright.edu

package WhiteBoard;

import java.util.*;

public interface WbServer extends java.rmi.Remote {
    void addClient(WbClient wc, String brnm) throws java.rmi.RemoteException;
    void delClient(WbClient wc, String brnm) throws java.rmi.RemoteException;
    void addLine(LineCoords ln, String brnm) throws java.rmi.RemoteException;
    void sendAllLines(WbClient wc, String brnm) throws java.rmi.RemoteException;
    
    //* new remote mthods added in server.
    void sendAllBoards_q(WbClient wc) throws java.rmi.RemoteException;
    void sendBoard_t(WbClient wc, String brdnm) throws java.rmi.RemoteException;
    void receiveBoard_t(ABoard board) throws java.rmi.RemoteException;
    void recvClientInfo(String S) throws java.rmi.RemoteException;        

}

// -eof-
