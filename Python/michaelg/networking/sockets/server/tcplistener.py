"""
Tcp Listener class

Provides template class to be inherited from for a tcp server
"""

import socket, threading
from .tcpclientmodel import TcpClient

class TcpListener:
    def __init__(self, ipAddr, port, packetSize, hasCommands=False):
        """
        TcpListener constructor

        initializes values

        Parameters
        ----------
        ipAddr : str
            ipAddress of server to be listened on
        port : int
            port of server to be listened on
        packetSize : int
            size of packets for communication
        hasCommands : bool
            determines whether or not to start commands thread

        Returns
        -------
        TcpServer object
        """
        
        self.ipAddr = ipAddr
        self.port = port
        self.__server = socket.socket(
            family=socket.AF_INET,
            type=socket.SOCK_STREAM
        )

        self.packetSize = packetSize
        self.clients = []

        self.__running = False
        self.__binded = False
        self.__hasCommands = hasCommands

    # don't override
    def run(self):
        """
        public run function

        binds socket and initializes all communication and command threads

        Parameters
        ----------
        None

        Returns
        -------
        None
        """
        
        # bind the socket to the ipAddr, port
        self.__server.bind((self.ipAddr, self.port))
        self.__binded = True

        # initialize accept clients thread
        self.__incomingConnectionThread = threading.Thread(
            target=self.__acceptThread
        )
        self.__incomingConnectionThread.setDaemon(True)

        # initialize the commands thread if necessary
        if self.__hasCommands:
            self.__commandsThread = threading.Thread(
                target=self.cmdThread
            )
            self.__commandsThread.start()

        self.__running = True
        self.serverStarted()

        self.__incomingConnectionThread.start()

    # don't override
    def __acceptThread(self):
        """
        private accept thread function

        thread to accept incoming clients

        Parameters
        ----------
        None

        Return
        ------
        None
        """
        
        while self.__running:
            # get client object
            self.__server.listen(1)

            clientSock, clientAddr = self.__server.accept()

            client = self.generateClientObject(clientSock, clientAddr)
            self.clients.append(client)
            self.clientConnected(client)

            # start client thread
            clientThread = threading.Thread(
                target=self.__clientThread,
                args={client}
            )
            clientThread.setDaemon(True)
            clientThread.start()

    # don't override
    def __clientThread(self, client):
        """
        private client thread function

        initialized for each client to handle communications

        Parameters
        ----------
        client : TcpClient
            client class with information about client

        Return
        ------
        None
        """
        
        msg = ""

        while True:
            try:
                while True:
                    # get message
                    data = client.sock.recv(self.packetSize).decode("latin1")
                    if data[-8:] == "finished":
                        # marks end of message
                        msg += data[:-8]
                        break
                    msg += data
                if not msg:
                    break
                elif msg.isspace():
                    continue

                self.msgReceived(client, msg)

                msg = ""
            except:
                # error in communications, disconnect client
                self.clientDisconnected(client)
                self.clients.remove(client)
                return

        client.sock.close()
        self.clientDisconnected(client)
        self.clients.remove(client)

    # override this
    def cmdThread(self):
        """
        command thread

        if hasCommands, server starts this thread to accept commands as input

        Parameters
        ----------
        None

        Return
        ------
        None
        """
        pass

    # don't override
    def getTcpClient(self, sock):
        """
        function to retrieve client object

        compares socket to each socket in list of clients

        Parameters
        ----------
        sock : socket
            desired socket object

        Return
        ------
        client object with matching socket, None if no match
        """
        
        for client in self.clients:
            if client.sock == sock:
                return client

        return None

    # don't override
    def send(self, client, msg, doEncode=True):
        """
        public send message function

        calls send function for client parameter

        Parameters
        ----------
        client : TcpClient
            recipient client
        msg : str or b
            message to send
        doEncode : bool
            specify whether or not to encode message into bytes

        Return
        ------
        None
        """
        
        client.send(self.packetSize, msg, doEncode)

    # recommended to override
    def generateClientObject(self, clientSock, clientAddr):
        """
        function to generate client object

        should be overriden to generate object of type that inherits from TcpClient

        Parameters
        ----------
        clientSock : socket
            socket of client
        clientAddr : address
            address of client

        Return
        ------
        client object inherited from TcpClient with clientSock, clientAddr
        """
        
        return TcpClient(clientSock, clientAddr)

    # override this
    def serverStarted(self):
        """
        server started event callback

        when server is fully started, the program calls this function

        Parameters
        ----------
        None

        Return
        ------
        None
        """
        pass

    # override this
    def clientConnected(self, client):
        """
        client connected event callback

        when a client connects, the program calls this function

        Parameters
        ----------
        client : TcpClient
            client that has connected

        Return
        ------
        None
        """
        pass

    # override this
    def clientDisconnected(self, client):
        """
        client disconnected event callback

        when a client disconnects, the program calls this function

        Parameters
        ----------
        client : TcpClient
            client that disconnected

        Return
        ------
        None
        """
        pass

    # override this
    def msgReceived(self, client, msg):
        """
        message received event callback

        when server receives message, the program calls this function

        Parameters
        ----------
        client : TcpClient
            sending client
        msg : str
            sent message

        Return
        ------
        None
        """
        pass