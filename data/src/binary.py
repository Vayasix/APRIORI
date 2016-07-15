# -*- coding: utf-8 -*-

import sys

if __name__ == "__main__":
    params = sys.argv
    argc = len(params)

    print("params:", params)
    print("params count:", argc)

    if argc != 5:
        print("Usage: python %s [input filename] [output filename] [n_column] [n_trans]" % params[0])
        quit()

    ifname = params[1]
    ofname = params[2]
    ncolumns = int(params[3])
    ntrans = int(params[4])
    counter = 0
    cntr_ntrans = 0

    with open(ifname, 'r') as fin, open(ofname, 'w') as fout:
        print("starting to generate data")
        for line in fin:
            
            for word in line.split():
                num = int(word)
                if num == 0:
                    fout.write("1 ")
                    counter += 1
                elif counter == 0:
                    fout.write("0 ")
                    counter += 1
                else:
                    fout.write(" ")

                while counter < num:
                    fout.write("0 ")
                    counter += 1

                fout.write("1")
                counter += 1

            while counter < ncolumns:
                fout.write(" 0")
                counter += 1

            fout.write("\n")
            counter = 0
            cntr_ntrans += 1
            if cntr_ntrans > ntrans:
                continue
            #print("ntrans:", ntrans)
