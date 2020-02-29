from ..sockets.server.tcplistener import TcpListener
from .httpclientmodel import HttpClientModel
from .httprequest import HttpRequest

class WebServerAttributes:
    def __init__(self, contextRoute, errorFile):
        self.contextRoute = contextRoute
        self.errorFile = errorFile

        self.actions = {}

class HttpServer(TcpListener):
    def __init__(self, ipAddr, port):
        super().__init__(ipAddr, port, 512, True, True)

        self.atts = WebServerAttributes("content", "error.html")

    def cmdThread(self):
        cmd = ""

        while True:
            cmd = input()
            if cmd == "stop":
                break

    def clientThread(self, client):
        while True:
            data = client.sock.recv(1024).decode("latin1")

            if data:
                self.msgReceived(client, data)
                data = None

        self.clientDisconnected(client)
        client.sock.close()
        self.client.remove(client)

    def serverStarted(self):
        print(f"HTTP Server started at {self.ipAddr}:{self.port}")

    def clientConnected(self, client):
        print("Client connected")

    def clientDisconnected(self, client):
        print("Client disconnected")

    def msgReceived(self, client, msg):
        print(msg)

        req = HttpRequest(client, msg, self.atts)
        req.parse()
        req.follow()

        self.send(client, req.genResponse(), True, False)