rm *.tar a.out d
tar cvf a.tar a >> d
tar cvf b.tar b >> d
gcc ../monmv.c
chmod 777 *
echo "__________________________________________________________"
echo "Avant a.tar" && cat a.tar && echo "" && echo "" && echo ""
echo "Avant b.tar" && cat b.tar && echo "" && echo "" && echo ""
echo "__________________________________________________________"
./a.out
echo "a.tar __2__" && cat a.tar && echo "" && echo "" && echo ""
echo "b.tar __2__" && cat b.tar && echo "" && echo "" && echo ""
echo "__________________________________________________________"
./a.out
echo "a.tar __3__" && cat a.tar && echo "" && echo "" && echo ""
echo "b.tar __3__" && cat b.tar && echo "" && echo "" && echo ""
echo "__________________________________________________________"
./a.out
echo "a.tar __4__" && cat a.tar && echo "" && echo "" && echo ""
echo "b.tar __4__" && cat b.tar && echo "" && echo "" && echo ""
echo "__________________________________________________________"
