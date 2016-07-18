#Apriori experiments

---

minsup:-> 10%
num. of trans:-> 1000
num. of items:-> 500

item length: [1]
candidates: 500
cached: 506500

item length: [2]
frequents: 214
candidates: 22791 
cached: 23087283 :-> 22657771 (1.9%)

item length: [3]
frequents: 906
candidates: 4899
cached: 4962687 :-> 4003376 (19.3%)


item length: [4]
frequents: 82
candidates: 9
cached: 9917 :-> 1013 (88.9%)


Num. of caches
In naive way: [ 28565587 ] ---> In estimation way: [ 27168660 ]
4.9% saved!


Ciphertext size on memory
In naive way: [ 28164135 MB ] ---> In estimation way: [ 26816389 MB ]
4.8% saved!

---


minsup:-> 10%
num. of trans:-> 100
num. of items:-> 50

item length: [1]
candidates: 50
cached: 50000

item length: [2]
frequents: 34 
candidates: 561
cached: 561000 :-> 496000 (11.6%)

item length: [3]
frequents: 149
candidates: 474 
cached: 474000 :-> 285000 (39.9%)


item length: [4]
frequents: 4
candidates: 22
cached: 22000 :-> 1000 (95.5%)


Num. of caches
In naive way: [ 1107000 ] ---> In estimation way: [ 832000 ]
24.8% saved!


Ciphertext size on memory
In naive way: [ 1065000 MB ] ---> In estimation way: [ 802600 MB ]
24.6% saved!


---


INPUT DATASET:  #trans: 10000,  #items: 50
minimum support: 10 %, threshold: 1000

---------- Generate Candidate ----------
size of freqent item sets : 0

======= Item length: [ 1 ] ======
Size of candidate itemsets: 50

... Done.

---------- Estimate Patterns ----------
... Done.

---------- Count Support ----------
#cached 500000 :-> 500000 (0.0 %)
... Done.

---------- Check Support ----------

... Done.

---------- Generate Candidate ----------
size of freqent item sets : 33

======= Item length: [ 2 ] ======
Size of candidate itemsets: 528

... Done.

---------- Estimate Patterns ----------
... Done.

---------- Count Support ----------
#cached 5280000 :-> 4660000 (11.7 %)
... Done.

---------- Check Support ----------

... Done.

---------- Generate Candidate ----------
size of freqent item sets : 143

======= Item length: [ 3 ] ======
Size of candidate itemsets: 447

... Done.

---------- Estimate Patterns ----------
... Done.

---------- Count Support ----------

#cached 4470000 :-> 2670000 (40.3 %)
... Done.

---------- Check Support ----------

... Done.

---------- Generate Candidate ----------
size of freqent item sets : 41

======= Item length: [ 4 ] ======
Size of candidate itemsets: 10

... Done.

---------- Estimate Patterns ----------
... Done.

---------- Count Support ----------
#cached 100000 :-> 10000 (90.0 %)
... Done.

---------- Check Support ----------

... Done.


...Done

Num. of caches
In naive way: [ 10350000 ] ---> In estimation way: [ 7840000 ]
24.3 % saved!

Ciphertext size on memory
In naive way: [ 9961500 MB ] ---> In estimation way: [ 7555000 MB ]
24.2 % saved! 
