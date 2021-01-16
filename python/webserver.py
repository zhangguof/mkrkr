#-*- coding:utf-8 -*-

import web
urls = (
   '/','index' 
)

class index:
    def GET(self):
        return "Hello world"

app = web.application(urls, locals())
application = app.wsgifunc()
if __name__ == "__main__":
    app.run()