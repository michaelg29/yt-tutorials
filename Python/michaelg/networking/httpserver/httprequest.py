def parseAttributeString(att_str):
    data = {}

    for param in att_str.split('&'):
        idx = param.find('=')
        key = param[:idx]
        value = param[idx + 1:]
        data[key] = value

    return data

class HttpRequest:
    def __init__(self, client, body, server_atts):
        self.client = client
        self.body = body
        self.server_atts = server_atts

        self.request = self.body.split()
        
        self.response_code = 200
        self.response_content = ""

        self.method = ""
        self.route = ""
        self.type = "text/html"

        self.query = {}

    def parse(self):
        self.method = self.request[0]
        self.route = self.request[1]

        idx = self.route.find('?')
        if idx != -1:
            self.query = parseAttributeString(self.route[idx + 1:])
            self.route = self.route[:idx]

        if self.method == "POST":
            post_vars = self.request[-1]
            if post_vars.find(';') == -1:
                self.postData = parseAttributeString(post_vars)

        print(f'{self.method} {self.route}')

    def follow(self):
        if self.route in self.server_atts.actions:
            self.server_atts.actions[self.route](self.client, self)
        else:
            self.readText(self.server_atts.errorFile)
            self.response_code = 404

    def genResponse(self):
        rc = "\r\n"

        msg = f'HTTP/1.1 {self.response_code} OK{rc}' + \
            f'Cache-Control: no-cache, private{rc}' + \
            f'Content-Length: {len(self.response_content)}{rc}' + \
            f'Content-Type: {self.type}, charset=iso-8859-1{rc*2}' + \
            f'{self.response_content}'

        return msg

    def readText(self, path):
        with open(f"{self.server_atts.contextRoute}/{path}", "r") as f:
            self.response_content = f.read()