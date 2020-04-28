// @file WbClient.java

package WhiteBoard;

public interface WbClient
    extends java.rmi.Remote, java.io.Serializable {
    void updateBoard(LineCoords ln)  throws java.rmi.RemoteException;
    void sendAllLines()  throws java.rmi.RemoteException;
    void sendLine(LineCoords ln)  throws java.rmi.RemoteException;
    void pleaseDie()  throws java.rmi.RemoteException;
    void recvDisplayObj(LinesFrame f)  throws java.rmi.RemoteException;    
	void receiveBoard_q(ABoard board) throws java.rmi.RemoteException;
	void receiveBoard_t(ABoard board) throws java.rmi.RemoteException;
    void sendClientInfo(WbServer ws) throws java.rmi.RemoteException;
    void sendClientInfo(WbClient wc) throws java.rmi.RemoteException;
    void recvClientInfo(String S) throws java.rmi.RemoteException;        
    void updateMyServer(WbServer new_server, String new_url) throws java.rmi.RemoteException;        
}

// -eof-
