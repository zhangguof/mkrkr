#-*- coding:utf-8 -*-
import sys
import engine

def update(tick):
    print 'test:%d'%tick


def init():
    print "py init script."
    print engine.py_version()
    # engine.regist_update(update)

