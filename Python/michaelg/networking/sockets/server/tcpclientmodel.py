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
        
        self.sock = clientSock
        self.addr = clientAddr

    # don't override
    def send(self, packetSize, msg, doEncode=True, tagFinished=True):
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
        
        # modify message
        send_b = msg.encode("utf8") if doEncode else msg # true_result if condition else false_result
        
        if tagFinished:
            send_b += "finished".encode("utf8")

        # break up and send individual packets
        size = len(send_b)
        idx = 0

        while idx < size - packetSize:
            self.sock.send(send_b[idx:idx + packetSize])
            idx += packetSize

        self.sock.send(send_b[idx:])