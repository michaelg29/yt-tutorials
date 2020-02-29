class HttpApp:
    def __init__(self, name):
        self.name = name
        self.routes = {}

    def getRoutes(self):
        ret = {}

        for key in self.routes:
            ret_key = self.name
            if key and key != '/':
                ret_key += f"/{key}" if key[0] != '/' else key
            ret[ret_key] = self.routes[key]

        return ret