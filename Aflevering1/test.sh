#!/usr/bin/env bash

# Exit immediately if any command below fails.
set -e

make


echo "Generating a test_files directory.."
mkdir -p test_files
rm -f test_files/*


echo "Generating test files.."
printf "Hello, World!\n" > test_files/ascii.input
printf "Hej verden!" > test_files/ascii2.input
printf "Hvor er du?!" > test_files/ascii3.input
printf "test paa et sas!" > test_files/ascii4.input
printf "test paa et #!" > test_files/ascii5.input
printf "test paa et \!" > test_files/ascii6.input

printf "Hello,\xa0World!\n" > test_files/ISO.input
printf "Hello,\xa1World!\n" > test_files/ISO2.input
printf "Hello,\xa2World!\n" > test_files/ISO3.input
printf "Hello,\xa3World!\n" > test_files/ISO4.input
printf "Hello,\xa4World!\n" > test_files/ISO5.input
printf "Hello,\xa5World!\n" > test_files/ISO6.input
printf "Hello,\xa6World!\n" > test_files/ISO7.input

printf "󨒳;݉瓬몍lqᣣ\n" > test_files/UTF.input
printf "˪򙠗񱳂٠I󷰄Q\n" > test_files/UTF2.input
printf "򙠗" > test_files/UTF3.input

printf "Hello,\x00World!\n" > test_files/data.input
printf "Hello,\x01World!\n" > test_files/data2.input
printf "Hello,\x02World!\n" > test_files/data3.input
printf "Hello,\x03World!\n" > test_files/data4.input
printf "Hello,\x04World!\n" > test_files/data5.input
printf "Hello,\x05World!\n" > test_files/data6.input
printf "Hello,\x06World!\n" > test_files/data7.input

printf "" > test_files/empty.input



echo "Running the tests.."
exitcode=0
for f in test_files/*.input
do
  echo ">>> Testing ${f}.."
  file    ${f} | sed -e 's/ASCII text.*/ASCII text/' \
                         -e 's/UTF-8 Unicode text.*/UTF-8 Unicode text/' \
                         -e 's/ISO-8859 text.*/ISO-8859 text/' \
                         -e 's/writable, regular file, no read permission/cannot determine (Permission denied)/' \
                         > "${f}.expected"
  ./file  "${f}" > "${f}.actual"

  if ! diff -u "${f}.expected" "${f}.actual"
  then
    echo ">>> Failed :-("
    exitcode=1
  else
    echo ">>> Success :-)"
  fi
done
exit $exitcode


