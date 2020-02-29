import sys
sys.path.append("C:\\src\\yt-library\\python")

from michaelg.networking.httpserver.httpapp import HttpApp

class Store(HttpApp):
    def __init__(self):
        super().__init__("store")

        self.routes = {
            "": self.index
        }

    def index(self, client, req):
        req.readText("store/index.html")