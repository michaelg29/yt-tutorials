import sys
sys.path.append("C:\\src\\yt-library\\python")

from michaelg.networking.httpserver.httpserver import HttpServer

IP_ADDRESS = "127.0.0.1"
PORT = 8080

def index(client, req):
    req.readText("index.html")

    if req.method == "POST":
        print(f"{req.postData['fname']} {req.postData['lname']}")

if __name__ == "__main__":
    server = HttpServer(IP_ADDRESS, PORT)

    server.atts.actions = {
        "/": index
    }

    server.run()