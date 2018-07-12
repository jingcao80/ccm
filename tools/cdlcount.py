#!/usr/bin/env python

import os
import sys

FILE_NUMBER = 0
LINE_NUMBER = 0

def read_file(file):
    lines = []
    fd = open(file, 'r')
    for line in fd:
        lines.append(line)
    fd.close()
    return lines

def sloccount_file(path):
    global LINE_NUMBER
    lines = read_file(path)
    if (len(lines) <= 0):
        return
    blockcomment = False
    for line in lines:
        l = line.lstrip()
        if (l == ""):
            continue
        if (l.startswith("//")):
            continue
        if (l.startswith("/*")):
            if (l.find("*/") != -1):
                continue
            else:
                blockcomment = True
                continue
        if (l.find("*/") != -1 and blockcomment):
            blockcomment = False
            continue
        if (blockcomment is True):
            continue
        LINE_NUMBER += 1

def process(path):
    global FILE_NUMBER
    if (os.path.isfile(path)):
        index = path.rfind("/")
        if index == -1:
            fileName = path
        else:
            fileName = path[(index + 1):len(path)]
        if (fileName.rfind(".cdl") != -1):
            sloccount_file(path)
            FILE_NUMBER += 1
    else:
        entries = os.listdir(path)
        for entry in entries:
            if (entry == "." or entry == ".."):
                continue
            entryPath = path + "/" + entry
            process(entryPath)

argc = len(sys.argv)
if argc == 2:
    process(sys.argv[1])
    print FILE_NUMBER, "files,", LINE_NUMBER, "lines"
else:
    print "Count source lines of .cdl files in path."
    print "usage:"
    print "    cdlcount path"
