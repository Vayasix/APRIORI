# -*- coding: utf-8 -*-

import sys

if __name__ == "__main__":
  params = sys.argv
  argc = len(params)

  print("params:", params)
  print("params count:", argc)

  if argc != 3:
    print("Usage: python %s [input filename] [output filename]" % params[0])
    quit()

  ifname = params[1]
  ofname = params[2]
  nitems = 0
  counter = -1
  tlensum = 0
  ntrans = 0
  wc = 0
  seq = ""
  with open(ifname, 'r') as fin, open(ofname, 'w') as fout:
    print("starting to generate data")
    for line in fin:
      for word in line.split():
        wc += 1
        if counter == -1:
          nitems = int(word)
          tlensum += nitems
          #print("nitems:", word)
          counter += 1
          continue
        
        seq += word
        fout.write(word)
        counter += 1

        if counter == nitems:
          #print(seq)
          #print("change transaction!")
          fout.write("\n")
          counter = -1
          seq = ""
          ntrans += 1
        else:
          fout.write(" ")
          seq += " "

  print("ntrans:", ntrans)
  print("Avg. tlen:", tlensum / ntrans)
  print("word count:", wc)

