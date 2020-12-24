tar cvf t.tar a b c
gcc ../monrm.c
./a.out
chmod 777 t.tar
od -c t.tar
#rm t.tar
echo
