#-*- coding:utf-8 -*-
import sys
import engine
# import webserver
import md5

def update(tick):
    print 'cur_tick:%d'%tick


def init():
    print "py init script."
    print engine.py_version()
    print sys.path
    # engine.regist_update(update)
    webserver.app.run()

