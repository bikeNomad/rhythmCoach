set terminal qt size 800,600 persist
set datafile separator tab
set datafile commentschars "#"
# set key autotitle columnheader
set termoption noenhanced
plot "rhythmCoach.tsv" using 1:2 with lines
print "hit CR to quit"
# wait for CR
pause -1
