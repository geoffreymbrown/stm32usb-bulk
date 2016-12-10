#!/usr/bin/python

import sys
import untangle
import os

incs = dict()
cvpath = dict()
svpath = dict()
csources = []
asmsources = []

def processfile(file):
    """
    Gather all the information about files
    """
    if (os.path.sep == '/'):
        name = file['name'].replace('\\','/')
    else:
        name = file['name']

    (dir,base)  = os.path.split(name)
    
    if (file['category'] == 'header'):
        incs[dir] = 1
    elif (file['category'] == 'sourceC') | (file['category'] == 'source'):
        cvpath[dir] = 1
        csources.append(base)
    elif (file['category'] == 'sourceAsm'):
        if (file['condition'] == 'GCC Toolchain'):
            svpath[dir] = 1
            asmsources.append(base)

def makegen(path):
    """
    process a single gpdsc file
    """
    obj = untangle.parse(path)

    print "# Vendor: " , obj.package.vendor.cdata

    for file in obj.package.generators.generator.project_files.file:
        processfile(file)

    for component in obj.package.components.component:
        for file in component.files.file:
            processfile(file)

    print "# Include paths\n"

    for i in incs.keys():
        print "INC += -I" + i

    print "\n# Source paths\n"

    for cv in cvpath.keys():
        print "vpath %.c", cv

    for sv in svpath.keys():
        print "vpath %.s", sv

    print "\n# Sources\n"

    for cf in csources:
        print "CSOURCE +=", cf

    for af in asmsources:
        print "ASMSOURCE +=", af


def main():
    makegen(sys.argv[1])

if __name__ == "__main__":
    main()
