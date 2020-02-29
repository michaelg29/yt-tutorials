import sys
sys.path.append(f"C:\src\yt-tutorials\python") # replace with path to your networking library

from michaelg.networking.sockets.client.tcpclient import TcpClient

class ChatClient(TcpClient):
    def __init__(self, ipAddr, port):
        super().__init__(ipAddr, port, 512, True)

    def inputThread(self):
        while True:
            msg = input()

            if msg == "\quit":
                break

            self.send(msg)

    def clientConnected(self):
        print(f"Client connected at {self.ipAddr}:{self.port}")

    def serverDisconnected(self):
        print("Client disconnected")

    def msgReceived(self, msg):
        print(msg)