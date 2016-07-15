# -*- coding: utf-8 -*-

import sys
import os

if __name__ == "__main__":
    params = sys.argv
    argc = len(params)

    if argc != 2:
        print("Usage: python %s [filename]" % params[0])
        quit()

    ifname = os.path.join('../input', params[1])
    nitems = 0
    with open(ifname, 'r') as fin:
        for line in fin:
            nitems = len(line.split(' '))
            break

    with open(ifname, 'r') as fin:
        a4clc = [0]*nitems
        for line in fin:
            nary = line.split(' ')
            for i in range(0, nitems):
                a4clc[i] += int(nary[i])

        print(a4clc)
