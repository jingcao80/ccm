#!/usr/bin/env python

import json
import os
import sys

SUB_MODS = [ 'all', 'core', 'test' ]
MLIST_DATA = None

def drop_module(module):
    command = 'adb.exe push ' + module['name'] + ' ' + module['dest']
    if (module.has_key('alias')):
        command = command + '/' + module['alias']
    os.system(command)

def drop_submodules(modules, submods):
    if (modules.has_key(submods)):
        for module in modules[submods]:
                drop_module(module)

def clean_module(module):
    name = module['alias'] if (module.has_key('alias')) else module['name']
    command = 'adb.exe shell rm ' + module['dest'] + '/' + name
    os.system(command)

def clean_submodules(modules, submods):
    if (modules.has_key(submods)):
        for module in modules[submods]:
                clean_module(module)

def drop_modules(submods):
    if (MLIST_DATA.has_key('modules')):
        modules = MLIST_DATA['modules']
        if submods == 'all':
            drop_submodules(modules, 'core')
            drop_submodules(modules, 'test')
        elif submods == 'core':
            drop_submodules(modules, 'core')
        elif submods == 'test':
            drop_submodules(modules, 'test')
        elif submods == 'clean':
            clean_submodules(modules, 'core')
            clean_submodules(modules, 'test')
    else:
        print "No modules need to be dropped."

def process(mlist, submods):
    global MLIST_DATA
    if (os.path.isfile(mlist)):
        with open(mlist, 'r') as fd:
            MLIST_DATA = json.load(fd)
        drop_modules(submods)
    else:
        print "File ", mlist, "is not existed."

argc = len(sys.argv)
if argc == 2:
    print "Drop modules to the android device... "
    process(sys.argv[1], 'all')
elif argc == 3 and sys.argv[2] in SUB_MODS:
    print "Drop modules to the android device... "
    process(sys.argv[1], sys.argv[2])
elif argc == 3 and sys.argv[2] == 'clean':
    print "Clean modules in the android device..."
    process(sys.argv[1], 'clean')
else:
    print "Drop modules in mlist.json to an android device."
    print "usage:"
    print "    drop [all | core | test]"
