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
tlen=50

#I 
# nitemsに対して"1/100"くらいの小ささだと、length-1で終わる。
# "1/10"だとcandidateができすぎて、かなり時間がかかる。
patlen=50

#N (should be devided by 1000)
nitems=1
#D (should be devided by 1000, ~k)
ntrans=1.40
#L
npats=1000
#fname="T50I100N1kD1kL5k"
fname="TEST"
#fname="T${tlen}I${patlen}N${nitems}D${ntrans}L${npats}"

# exec
path="$HOME/Work/FreqPatMine/data"
$path/IBM_Quest_data_generator/seq_data_generator lit -tlen $tlen -patlen $patlen -nitems $nitems -ntrans $ntrans -npats $npats -ascii -fname $path/src/$fname

nitems=`echo "scale = 2; $nitems*1000" | bc `
ntrans=`echo "scale = 2; $ntrans*1000" | bc `
nitems=${nitems%.*}
ntrans=${ntrans%.*}
python $path/src/generator.py "$path/src/${fname}.data" "$path/src/tmpf"
python $path/src/binary.py "$path/src/tmpf" "$path/src/$fname" $nitems $ntrans
rm -rf "$path/src/${fname}.data" "$path/src/${fname}.pat" "$path/src/tmpf"
if [[ -e $path/input/$fname ]]; then
    echo "This file already exists in $path/input/"
    mv -f $path/src/$fname $path/input/
    echo "move $path/src/$fname --> $path/input/"
else
    mv $path/src/$fname $path/input/
fi

