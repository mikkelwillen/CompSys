make

TIMEFORMAT=%R

{ { time ./fauxgrep int . ;} >/dev/null ; } 2> fauxgrepTest.txt
{ { time ./fauxgrep int ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fauxgrep int ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fauxgrep int ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fauxgrep int ../../../..  ; } >/dev/null ; } 2> temp4.txt

cat temp1.txt >> fauxgrepTest.txt
cat temp2.txt >> fauxgrepTest.txt
cat temp3.txt >> fauxgrepTest.txt
cat temp4.txt >> fauxgrepTest.txt

rm temp1.txt
rm temp2-txt
rm temp3.txt
rm temp4.txt