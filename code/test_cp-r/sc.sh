rm *.tar
rm a.out
tar cvf a.tar a >> d
tar cvf b.tar b >> d
gcc ../moncp.c
chmod 777 *
echo "Avant" && cat a.tar && echo "" && echo "" && echo ""
./a.out
echo "__2__" && cat a.tar && echo "" && echo "" && echo ""
./a.out
echo "__3__" && cat a.tar && echo "" && echo "" && echo ""
./a.out
echo "__4__" && cat a.tar && echo "" && echo "" && echo ""
