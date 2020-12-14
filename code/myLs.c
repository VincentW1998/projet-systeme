#include "myLs.h"
#include "myCd.h"
#include "gestionnaire.h"

// ls sans path en arguments
int LsWithoutPath(){
	if(*TARPATH == '\0')
		simpleLs();
	else printOccurences();
	return 0;
}

// Ls sans options avec un path
int lsRep(char * path){
	//je me deplace dans le dossier a lister
	if(*TARPATH == '\0'){
		if(cdPerso(path) == -1){
			perror("bad address");
			return -1;
		}
	}
	else if(navigate(path) == -1){
			perror("bad address");
			return -1;
	}
	// fin deplacement
	
	if(*TARPATH == '\0') simpleLs();
	else printOccurences();

	return 0;
}
					
void simpleLs(void){
	char * cmd[2] = {"ls"};
	execCommand(cmd);
}

void printOccurences(void){
	char * tmp = malloc(strlen(TARPATH) + 1);
	strcpy(tmp, TARPATH);
	char * tar = strtok_r(tmp, "/\0",&tmp);
	int f;
	ssize_t n;
	
	if((f = open(tar, O_RDONLY)) == -1) perror("open tar:");
	struct posix_header * p = malloc(sizeof(struct posix_header));
//	printf("TARPATH : %s \n", TARPATH + strlen(tar)+1);
//	printf("TAR : %s \n", tar);
	printf("%s  : \n", TARPATH);
	while((n = read(f,p,BLOCKSIZE)) > 0){
		if(strlen(TARPATH) > strlen(tar)){

			if(validPath(TARPATH + strlen(tar)+1 ,p->name) == 0){
				write(1,p->name, strlen(p->name));
				write(1,"\n",1);
			}
		}
		else if(validPath("", p->name) == 0){
			write(1,p->name, strlen(p->name));
			write(1,"\n",1);
		}
		lseek(f,ceil(atoi(p->size)/512.)*BLOCKSIZE,SEEK_CUR);
	}
}

// check si target est visible depuis path 1
int validPath(char * path, char * target){
	char * tmp = malloc(strlen(path)+2);
	strcpy(tmp,path);
	if(strlen(path) != 0) strcat(tmp, "/"); // le path ne se trouve pas a la racine du tarball on rajoute / pour etre sur le que le fichier correspond bien
	char * targ, * token;
	if(strncmp(tmp,target, strlen(tmp)) == 0){
		targ = malloc(strlen(target) - strlen(tmp) + 1);
		strcpy(targ, target + strlen(tmp));
		if((token = strtok_r(targ,"/",&targ)) == NULL) return -1; // si token est null ici on a donc target == path
		if((token = strtok_r(targ,"/",&targ)) == NULL) return 0; // si token null alrs on a donc bien target qui est de profondeur 1 de plus que path
		return -1;
	}
	return -1;
}

		

int ls(int nbOptions, char ** path){
	char *myPos, *myPosTar;
	myPos = getcwd(NULL, 0); // je sauvegarde ma position
	myPosTar = malloc(strlen(TARPATH) + 1);
	int i;
	if(nbOptions < 2) return LsWithoutPath();
	if(strcmp(path[1],"-l") == 0) i = 2;
	else i=1;
	for(; i < nbOptions; i++){
	
		if(lsRep(path[i]) == -1) {perror("no such file or directory"); returnToPos(myPos, myPosTar); return -1;}
		returnToPos(myPos, myPosTar);
	}
	return 0;
}
 
