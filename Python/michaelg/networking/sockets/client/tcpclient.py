import socket, threading

class TcpClient:
    def __init__(self, ipAddr, port, packetSize, hasInput=False):
        """
        TcpClient constructor

        initializes values

        Parameters
        ----------
        ipAddr : str
            ipAddress of server to connect to
        port : int
            port of server to connect to
        packetSize : int
            size of packets for communication
        hasInput : bool
            determines whether or not to start commands thread

        Returns
        -------
        TcpClient object
        """
        
        self.ipAddr = ipAddr
        self.port = port
        self.__client = socket.socket(
            family=socket.AF_INET,
            type=socket.SOCK_STREAM
        )

        self.packetSize = packetSize

        self.__connected = False
        self.__hasInput = hasInput

    # don't override
    def run(self):
        """
        public run function

        connects to server and initializes communications and input threads

        Parameters
        ----------
        None

        Returns
        -------
        None
        """

        # connect to the server
        self.sock.connect((self.ipAddr, self.port))
        self.__connected = True
        self.clientConnected()

        # initialize message thread
        self.__serverThread = threading.Thread(
            target=self.__serverThread
        )
        self.__serverThread.setDaemon(True)
        self.__serverThread.start()

        # initialize input thread
        if self.__hasInput:
            self.__inputThread = threading.Thread(
                target=self.inputThread
            )
            self.__inputThread.start()

    # don't override
    def __serverThread(self):
        """
        private server thread function

        handles all incoming communications from server

        Parameters
        ----------
        None

        Return
        ------
        None
        """

        msg = ""

        while True:
            try:
                while True:
                    # get message
                    data = self.sock.recv(self.packetSize).decode("latin1")
                    if data[-8:] == "finished":
                        # marks end of message
                        msg += data[:-8]
                        break
                    msg += data
                if not msg:
                    break
                elif msg.isspace():
                    continue

                self.msgReceived(msg)

                msg = ""
            except:
                # error in communication, must disconnect
                self.serverDisconnected()
                self.sock.close()
                return

        self.serverDisconnected()
        self.sock.close()

    # override
    def inputThread(self):
        """
        public input thread function

        handles all local input from console, should be overriden

        Parameters
        ----------
        None

        Return
        ------
        None
        """

        pass

    # don't override
    def send(self, msg, doEncode=True):
        """
        public send message function

        sends message to server

        Parameters
        ----------
        msg : str or b
            message to send
        doEncode : bool
            specify whether or not to encode message into bytes

        Return
        ------
        None
        """

        # modify message
        send_b = msg.encode("utf8") if doEncode else msg
        send_b += "finished".encode("utf8")

        # break up message and send individual packets
        size = len(send_b)
        idx = 0

        while idx < size - self.packetSize:
            self.sock.send(send_b[idx:idx + self.packetSize])
            idx += self.packetSize

        self.sock.send(send_b[idx:])

    # override
    def clientConnected(self):
        """
        client connected event callback

        when the client connects, the program calls this function

        Parameters
        ----------
        None

        Return
        ------
        None
        """

        pass

    # override
    def serverDisconnected(self):
        """
        server disconnected event callback

        when the client disconnects, the program calls this function

        Parameters
        ----------
        None

        Return
        ------
        None
        """

        pass

    # override
    def msgReceived(self, msg):
        """
        message received event callback

        when client receives message, the program calls this function

        Parameters
        ----------
        msg : str
            sent message

        Return
        ------
        None
        """

        pass