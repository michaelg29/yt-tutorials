from ..sockets.server.tcpclientmodel import TcpClient

class HttpClientModel(TcpClient):
    def __init__(self, clientsock, clientaddr):
        super().__init__(clientsock, clientaddr)

        self.context = {}