#!/bin/sh
list=('test32 tinymt32dc.0.2000.txt 10 100 100000' \
      'test32_jump 10 100 1000000' \
      'test32_jump2 10 100 1000000' \
      'test64 tinymt64dc.0.2000.txt 10 100 100000' \
      'test64_jump 10 100 1000000' \
      'test64_jump2 10 100 1000000')
result=OK
for f in "${list[@]}"; do
    g=($f)
    echo testing ${g[0]}
    if ./${g[*]} > /dev/null
    then echo ${g[0]} is OK
    else echo ${g[0]} is NG; result=NG; break
    fi
done
if [ $result = "OK" ]
then echo All tests are OK.
else echo test failed.
fi
