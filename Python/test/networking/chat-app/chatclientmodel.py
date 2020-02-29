import sys
sys.path.append(f"C:\src\yt-tutorials\python") # replace with path to your networking library

from michaelg.networking.sockets.server.tcpclientmodel import TcpClient

class ChatClientModel(TcpClient):
    def __init__(self, clientSock, clientAddr):
        super().__init__(clientSock, clientAddr)

        self.hasName = False