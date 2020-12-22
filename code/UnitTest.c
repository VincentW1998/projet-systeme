//
//  UnitTest.c
//  shell
//
//  Created by Félix Ngo on 22/12/2020.
//

#include "UnitTest.h"
#include "gestionnaire.h"
#include "storeRestore.h"
#include "myCd.h"
#define absPath "/home/projet/Test"
void TCd1(){ //goes to Lvl1-B
	storePosition();
	printf(KBLU"TCd1 -> goes to Test/lvl1-B"RESET "\n");
	cdPerso("Test/lvl1-B");
	validityCd("/home/projet/Test/lvl1-B","");
	restorePosition();
}

void TCd2(){ // goest to Lvl1-b with absolute path
	storePosition();
	printf(KBLU"TCd2 -> goes to %s/lvl1-B"RESET "\n",absPath);
	cdPerso("/home/projet/Test/lvl1-B");
	validityCd("/home/projet/Test/lvl1-B","");
	restorePosition();
}

void TCd3(){
	storePosition();
	printf(KBLU"TCd3 -> goes to tarball with %s/lvl1-A.tar/lvl2-A"RESET "\n",absPath);
	cdPerso("/home/projet/Test/lvl1-A.tar/lvl2-A");
	validityCd(absPath,"lvl1-A.tar/lvl2-A");
	restorePosition();
}

void TCd4(){
	storePosition();
	printf(KBLU "TCd4 -> goes to tarball with Test/lvl1-A.tar/lvl2-A"RESET "\n");
	cdPerso("Test/Lvl1-A.tar/Lvl2-A");
	validityCd(absPath,"lvl1-A.tar/lvl2-A");
	restorePosition();
}

void validityCd(char * pos, char * posTar){
	if(strcmp(getcwd(NULL, 0),pos) == 0 && strcmp(TARPATH, posTar) == 0)
		printf(GREEN "Passed" RESET "\n");
	else
		printf(RED "Failed" RESET "\n");
}



int Test(){
	//	if(nbOption < 2) return -1;
	TCd1();
	TCd2();
	TCd3();
	TCd4();
	
	return 0;
}
