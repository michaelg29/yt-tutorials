import sys
sys.path.append("C:\\src\\yt-library\\python")

from michaelg.networking.httpserver.httpserver import HttpServer

from apps.store import Store

IP_ADDRESS = "127.0.0.1"
PORT = 8080

def index(client, req):
    if req.method == "POST":
        client.context['name'] = f"{req.postData['fname']} {req.postData['lname']}"

    req.renderTemplate("index.html")

if __name__ == "__main__":
    server = HttpServer(IP_ADDRESS, PORT)

    server.atts.actions = {
        "/": index
    }

    server.addApp(Store())

    server.run()