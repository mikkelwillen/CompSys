# make

TIMEFORMAT=%R

# Fauxgrep test
{ { time ./fauxgrep int . ; } >/dev/null ; } 2> fauxgrepTest.txt
{ { time ./fauxgrep int ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fauxgrep int ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fauxgrep int ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fauxgrep int ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fauxgrep int ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fauxgrepTest.txt
cat temp2.txt >> fauxgrepTest.txt
cat temp3.txt >> fauxgrepTest.txt
cat temp4.txt >> fauxgrepTest.txt
# cat temp5.txt >> fauxgrepTest.txt

# Fauxgrep-mt test med 1 tråd
{ { time ./fauxgrep-mt int . ; } >/dev/null ; } 2> fauxgrep-mt1Test.txt
{ { time ./fauxgrep-mt int ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fauxgrep-mt int ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fauxgrep-mt int ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fauxgrep-mt int ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fauxgrep-mt int ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fauxgrep-mt1Test.txt
cat temp2.txt >> fauxgrep-mt1Test.txt
cat temp3.txt >> fauxgrep-mt1Test.txt
cat temp4.txt >> fauxgrep-mt1Test.txt
# cat temp5.txt >> fauxgrep-mt1Test.txt

# Fauxgrep-mt test med 2 tråde
{ { time ./fauxgrep-mt -n 2 int . ; } >/dev/null ; } 2> fauxgrep-mt2Test.txt
{ { time ./fauxgrep-mt -n 2 int ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fauxgrep-mt -n 2 int ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fauxgrep-mt -n 2 int ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fauxgrep-mt -n 2 int ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fauxgrep-mt -n 2 int ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fauxgrep-mt2Test.txt
cat temp2.txt >> fauxgrep-mt2Test.txt
cat temp3.txt >> fauxgrep-mt2Test.txt
cat temp4.txt >> fauxgrep-mt2Test.txt
# cat temp5.txt >> fauxgrep-mt2Test.txt

# Fauxgrep-mt test med 3 tråde
{ { time ./fauxgrep-mt -n 3 int . ; } >/dev/null ; } 2> fauxgrep-mt3Test.txt
{ { time ./fauxgrep-mt -n 3 int ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fauxgrep-mt -n 3 int ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fauxgrep-mt -n 3 int ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fauxgrep-mt -n 3 int ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fauxgrep-mt -n 3 int ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fauxgrep-mt3Test.txt
cat temp2.txt >> fauxgrep-mt3Test.txt
cat temp3.txt >> fauxgrep-mt3Test.txt
cat temp4.txt >> fauxgrep-mt3Test.txt
# cat temp5.txt >> fauxgrep-mt3Test.txt

# Fauxgrep-mt test med 4 tråde
{ { time ./fauxgrep-mt -n 4 int . ; } >/dev/null ; } 2> fauxgrep-mt4Test.txt
{ { time ./fauxgrep-mt -n 4 int ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fauxgrep-mt -n 4 int ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fauxgrep-mt -n 4 int ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fauxgrep-mt -n 4 int ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fauxgrep-mt -n 4 int ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fauxgrep-mt4Test.txt
cat temp2.txt >> fauxgrep-mt4Test.txt
cat temp3.txt >> fauxgrep-mt4Test.txt
cat temp4.txt >> fauxgrep-mt4Test.txt
# cat temp5.txt >> fauxgrep-mt4Test.txt


# Fhistogram test
{ { time ./fhistogram . ; } >/dev/null ; } 2> fhistogramTest.txt
{ { time ./fhistogram ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fhistogram ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fhistogram ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fhistogram ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fhistogram ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fhistogramTest.txt
cat temp2.txt >> fhistogramTest.txt
cat temp3.txt >> fhistogramTest.txt
cat temp4.txt >> fhistogramTest.txt
# cat temp5.txt >> fhistogramTest.txt

# fhistogram-mt test med 1 tråd
{ { time ./fhistogram-mt . ; } >/dev/null ; } 2> fhistogram-mt1Test.txt
{ { time ./fhistogram-mt ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fhistogram-mt ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fhistogram-mt ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fhistogram-mt ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fhistogram-mt ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fhistogram-mt1Test.txt
cat temp2.txt >> fhistogram-mt1Test.txt
cat temp3.txt >> fhistogram-mt1Test.txt
cat temp4.txt >> fhistogram-mt1Test.txt
# cat temp5.txt >> fhistogram-mt1Test.txt

# fhistogram-mt test med 2 tråde
{ { time ./fhistogram-mt -n 2 . ; } >/dev/null ; } 2> fhistogram-mt2Test.txt
{ { time ./fhistogram-mt -n 2 ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fhistogram-mt -n 2 ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fhistogram-mt -n 2 ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fhistogram-mt -n 2 ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fhistogram-mt -n 2 ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fhistogram-mt2Test.txt
cat temp2.txt >> fhistogram-mt2Test.txt
cat temp3.txt >> fhistogram-mt2Test.txt
cat temp4.txt >> fhistogram-mt2Test.txt
# cat temp5.txt >> fhistogram-mt2Test.txt

# fhistogram-mt test med 3 tråde
{ { time ./fhistogram-mt -n 3 . ; } >/dev/null ; } 2> fhistogram-mt3Test.txt
{ { time ./fhistogram-mt -n 3 ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fhistogram-mt -n 3 ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fhistogram-mt -n 3 ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fhistogram-mt -n 3 ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fhistogram-mt -n 3 ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fhistogram-mt3Test.txt
cat temp2.txt >> fhistogram-mt3Test.txt
cat temp3.txt >> fhistogram-mt3Test.txt
cat temp4.txt >> fhistogram-mt3Test.txt
# cat temp5.txt >> fhistogram-mt3Test.txt

# fhistogram-mt test med 4 tråde
{ { time ./fhistogram-mt -n 4 . ; } >/dev/null ; } 2> fhistogram-mt4Test.txt
{ { time ./fhistogram-mt -n 4 ..  ; } >/dev/null ; } 2> temp1.txt
{ { time ./fhistogram-mt -n 4 ../..  ; } >/dev/null ; } 2> temp2.txt
{ { time ./fhistogram-mt -n 4 ../../..  ; } >/dev/null ; } 2> temp3.txt
{ { time ./fhistogram-mt -n 4 ../../../..  ; } >/dev/null ; } 2> temp4.txt
# { { time ./fhistogram-mt -n 4 ../../../../..  ; } >/dev/null ; } 2> temp5.txt

cat temp1.txt >> fhistogram-mt4Test.txt
cat temp2.txt >> fhistogram-mt4Test.txt
cat temp3.txt >> fhistogram-mt4Test.txt
cat temp4.txt >> fhistogram-mt4Test.txt
# cat temp5.txt >> fhistogram-mt4Test.txt

rm temp1.txt
rm temp2.txt
rm temp3.txt
rm temp4.txt
rm temp5.txt