

FILE=${1}
echo ${FILE}
cat ${FILE} | sed -e 's/\\/\//g' | sed -e 's/\/$/\\/g' > tmpmf
mv tmpmf ${FILE}


