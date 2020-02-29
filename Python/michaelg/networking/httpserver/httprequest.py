from jinja2 import Template

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
        self.bytes = False

        self.query = {}

    def parse(self):
        self.method = self.request[0]
        self.route = self.request[1]

        if self.route[-1] == '/' and self.route != '/':
            self.route = self.route[:-1]

        idx = self.route.find('?')
        if idx != -1:
            self.query = parseAttributeString(self.route[idx + 1:])
            self.route = self.route[:idx]

        if self.method == "POST":
            post_vars = self.request[-1]
            if post_vars.find(';') == -1:
                self.postData = parseAttributeString(post_vars)

        # Accept: text/css,*/*;q=0.1
        idx = self.body.find("Accept: ")
        if idx != -1:
            idx += len("Accept: ")
            self.type = ""
            for idx in range(idx, len(self.body)):
                c = self.body[idx]
                if c == ',' or c == ';' or c == '\n':
                    break
                self.type += c

        print(f'{self.method} {self.route}')

    def follow(self):
        if self.route.find('.') == -1:
            mod_route = self.route[1:]

            if self.route in self.server_atts.actions:
                self.server_atts.actions[self.route](self.client, self)
            elif mod_route in self.server_atts.actions:
                self.server_atts.actions[mod_route](self.client, self)
            else:
                self.readText(self.server_atts.errorFile)
                self.response_code = 404
        else:
            if self.type.find("text") > -1 or self.type.find("*/*") == 0:
                self.readText(self.route[1:])
            elif self.type.find("image") > -1:
                self.readBytes(self.route[1:])
                self.bytes = True

    def genResponse(self):
        rc = "\r\n"

        msg = f'HTTP/1.1 {self.response_code} OK{rc}' + \
            f'Cache-Control: no-cache, private{rc}' + \
            f'Content-Length: {len(self.response_content)}{rc}' + \
            f'Content-Type: {self.type}, charset=iso-8859-1{rc}'

        if self.bytes:
            msg += f'Accept-Ranges: bytes{rc}{rc}'
            msg = msg.encode("utf8") + self.response_content
        else:
            msg += f'{rc}{self.response_content}'

        return msg

    def readText(self, path):
        try:
            with open(f"{self.server_atts.contextRoute}/{path}", "r") as f:
                self.response_content = f.read()
        except:
            self.response_content = ""

    def readBytes(self, path):
        try:
            with open(f"{self.server_atts.contextRoute}/{path}", "rb") as f:
                self.response_content = f.read()
        except:
            self.response_content = b''

    def renderTemplate(self, path):
        template = Template('')

        try:
            template = self.server_atts.jinja_env.get_template(path)
        except:
            self.response_code = 404
            template = self.server_atts.jinja_env.get_template(self.server_atts.errorFile)

        self.response_content = template.render(self.client.context)