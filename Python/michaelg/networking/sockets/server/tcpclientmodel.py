"""
Tcp Client model class
Used to store data about client for server
"""

class TcpClient:
    def __init__(self, clientSock, clientAddr):
        """
        TcpClient constructor
        initializes socket and client objects for storage in server
        Parameters
        ----------
        clientSock : socket
            socket object of client
        clientAddr : address
            address bound to client socket
        Returns
        -------
        TcpClient object
        """
        pass

    def send(self, packetSize, msg, doEncode=True):
        """
        public send message function
        sends message to client socket
        Parameters
        ----------
        bufSize : int
            packet size to send message
        msg : str or b
            message to send
        doEncode : bool
            if message must be encoded to bytes
        Return
        ------
        None
        """
        pass