import sys
sys.path.append(f"D:\Documents\Youtube\yt-tutorials\python") # replace with path to your networking library

from michaelg.networking.sockets.server.tcplistener import TcpListener
from chatclientmodel import ChatClientModel

class ChatServer(TcpListener):
    def __init__(self, ipAddr, port):
        super().__init__(ipAddr, port, 512, True)

    def cmdThread(self):
        while True:
            cmd = input()

            if cmd == "stop":
                break

    def generateClientObject(self, clientSock, clientAddr):
        return ChatClientModel(clientSock, clientAddr)

    def serverStarted(self):
        print(f"Chat server started at {self.ipAddr}:{self.port}")

    def clientConnected(self, client):
        print("Client connected")

        self.send(client, "Welcome to the chat server. What is your name?")

    def clientDisconnected(self, client):
        print(f"{client.name} disconnected")
        self.broadcast(client, f"{client.name} has left the chat room.")

    def msgReceived(self, client, msg):
        if client.hasName:
            print(f"{client.name} says {msg}")
            self.broadcast(client, f"{client.name} says {msg}")
        else:
            client.name = msg

            msg = f"Welcome {client.name}, these are the connected users: "
            for c in self.clients:
                if c.hasName:
                    msg += c.name + ", "
            self.send(client, msg)

            client.hasName = True

            self.broadcast(client, f"{client.name} has entered the chat room.")

    def broadcast(self, sendingClient, msg):
        for client in self.clients:
            if client != sendingClient:
                self.send(client, msg)