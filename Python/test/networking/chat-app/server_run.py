from chatserver import ChatServer

IP_ADDRESS = "127.0.0.1"
PORT = 5500

if __name__ == "__main__":
    server = ChatServer(IP_ADDRESS, PORT)
    server.run()