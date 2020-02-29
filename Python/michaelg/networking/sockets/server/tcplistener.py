"""
Tcp Listener class
Provides template class to be inherited from for a tcp server
"""

import socket, threading
from tcpclientmodel import TcpClient

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
        pass

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
        pass

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
        pass

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
        pass

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
        pass

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
        pass

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
        pass

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