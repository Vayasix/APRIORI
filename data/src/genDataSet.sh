#!/bin/bash
# Command Line Options:
#   -tlen avg_items_per_transaction (default: 2.5) <--- T
#   -nitems number_of_different_items_in_000s (default: 10) <--- N
#   -ncust number_of_customers_in_000s (default: 100) 
#   -slen avg_trans_per_customer (default: 10)
#   -rept repetition-level (default: 0)
#
#   -seq.npats number_of_seq_patterns (default: 5000) <--- L
#   -seq.patlen avg_length_of_maximal_pattern (default: 4) <--- I
#   -seq.corr correlation_between_patterns (default: 0.25)
#   -seq.conf avg_confidence_in_a_rule (default: 0.75)
#
#   -fname <filename> (write to filename.data and filename.pat)
#   -ascii (Write data in ASCII format; default: binary)
#   -version (to print out version info)

#T
tlen=10
#I
patlen=6
#N (should be devided by 1000)
nitems=1
#D (should be devided by 1000, ~k)
ntrans=10.75
#L
npats=1000
fname="T10I6N1kD10kL1k"
#fname="T${tlen}I${patlen}N${nitems}D${ntrans}L${npats}"

# exec
../IBM_Quest_data_generator/seq_data_generator lit -tlen $tlen -patlen $patlen -nitems $nitems -ntrans $ntrans -npats $npats -ascii -fname $fname

nitems=`echo "scale = 2; $nitems*1000" | bc `
ntrans=`echo "scale = 2; $ntrans*1000" | bc `
nitems=${nitems%.*}
ntrans=${ntrans%.*}
python ./generator.py "${fname}.data" tmpf
python ./binary.py tmpf ${fname} $nitems $ntrans
rm -rf *.data *.pat tmpf
if [[ -e ../input/$fname ]]; then
    echo "This file already exists in ../input/"
else
    mv ./$fname ../input/
fi

