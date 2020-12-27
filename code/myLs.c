#include "myLs.h"
#include "myCd.h"
#include "gestionnaire.h"
#include "check.h"
#include "storeRestore.h"

// ls sans path en arguments
int LsWithoutPath(){
	if(*TARPATH == '\0')
		simpleLs();
	else printOccurences();
	return 1;
}

int lsNoL(){
	char * command[2] = {"ls"};
	return execCommand(command);
}

// Ls sans options avec un path
int lsRep(char * path){
	if(whichCd(path) == -1) return -1; //deplacement vers le dossier a afficher
	if(TARPATH[0] == '\0')
		simpleLs();
	else printOccurences();
	return 1;
}


void simpleLs(){
	if(withL == 1)
		commandNoTar("ls", "-l");
	char * cmd[2] = {"ls"};
	execCommand(cmd);
	return;
}


/* utilise le bon validPath en fonction de la position actuelle
 si on se trouve a la racine renvoie validpath avec une chaine vide */
int whichValid(char * tar, char * name){
	if(strlen(TARPATH) > strlen(tar))
		return validPath(TARPATH + strlen(tar)+1 , name);
	else return validPath("", name);
}

//void printOccurences2(int withL){ //si withL = 1 on affiche ls -l
////	char * tmp = malloc(strlen(TARPATH) + 1);
////	strcpy(tmp, TARPATH);
////	char * tar = strtok_r(tmp, "/\0",&tmp);
//	char * tar = findTar(TARPATH);
//	char * lastTok;
//	int f;
//	ssize_t n;
//	write(1,TARPATH,strlen(TARPATH)); // print le nom du dossier a afficher
//	write(1,":\n", 2);
//	if((f = open(tar, O_RDONLY)) == -1) perror("open tar:");
//	struct posix_header * p = malloc(sizeof(struct posix_header));
//	while( ((n = read(f,p,BLOCKSIZE)) > 0) && (p->name[0] != '\0')){
//		if(strlen(TARPATH) > strlen(tar)){
//			if(validPath(TARPATH + strlen(tar)+1 ,p->name) == 0){
//				if(withL == 1) optionL(p,f);
//				write(1,p->name + strlen(TARPATH + strlen(tar) + 1)+ 1, strlen(p->name + strlen(TARPATH + strlen(tar) + 1)));
//				write(1,"\n",1);
//			}
//		}
//		else if(validPath("", p->name) == 0){
//			if(withL == 1) optionL(p,f);
//			write(1,p->name, strlen(p->name));
//			write(1,"\n",1);
//		}
//		next_header(f, atoi(p->size));
//	}
//	close(f);
//}


// print les fichiers visible depuis la position actuelle
void printOccurences(){
	char * tar = findTar(TARPATH), * lastTok;
	int f, filesize = 0;
	ssize_t n;
	write(1,TARPATH,strlen(TARPATH)); // print le nom du dossier a afficher
	write(1,":\n", 2);
	if((f = open(tar, O_RDONLY)) == -1) perror("open tar:");
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while( ((n = read(f,p,BLOCKSIZE)) > 0) && (p->name[0] != '\0')){
			if(whichValid(tar, p->name) == 1){
				if(withL == 1) optionL(p,f); // lance l'option l si elle est utilisée
				lastTok = getLastToken(p->name);
				write(1, lastTok, strlen(lastTok));
//				write(1, p->name, strlen(p->name));
				write(1,"\n",1);
			}
		sscanf(p->size, "%o", &filesize);
		next_header(f, filesize);
	}
	close(f);
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
		if((token = strtok_r(targ,"/",&targ)) == NULL) return 1; // si token null alrs on a donc bien target qui est de profondeur 1 de plus que path
		return -1;
	}
	return -1;
}

// ********* -l ************
void optionL (struct posix_header * p, int file){
	typeFic(p);
	rights(p);
	nbrlink(p,file);
	usrAndGrp(p);
	psize(p);
	mtime(p);
}

void typeFic(struct posix_header * p){
	char * type[8] = {"-","l","l","c","b","d","p","s"};
	char * typeflag = malloc(2);
	typeflag[0] = p->typeflag;
	write(1,type[atoi(typeflag)],1);
	//	free(typeflag);
}

void rights (struct posix_header * p){
	char * perm[8] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
	int tst;
	sscanf(p->mode, "%o", &tst);
	char * mode = malloc(2);
	char * str = malloc(strlen(perm[0]) * 3 + 1);
	size_t modesize = strlen(p->mode); //les droits correspondent aux 3 derniers chiffres de p->mode
	mode[0] = p->mode[modesize - 3]; //char to string
	strcpy(str,perm[atoi(mode)]); //droits usr
	mode[0] = p->mode[modesize - 2];
	strcat(str,perm[atoi(mode)]);; //droits groupe
	mode[0] = p->mode[modesize - 1];
	strcat(str,perm[atoi(mode)]); // droits autres
	strcat(str," ");
	write(1,str,strlen(str));
	free(str);
	free(mode);
}

void nbrlink(struct posix_header * p, int file){
	if(p->typeflag != '5'){
		write(1,"1 ",2);
		return;
	}
	off_t current = lseek(file, 0, SEEK_CUR);
	lseek(file,0,SEEK_SET);
	char * tmp = malloc(strlen(TARPATH)+ 1);
	strcpy(tmp, TARPATH);
	char * tar = strtok_r(tmp, "/",&tmp);
	tmp = malloc(strlen(TARPATH) - strlen(tar) + strlen(p->name) + 3);
	if(strlen(tar) == strlen(TARPATH)) strcpy(tmp,p->name); //retourne TARPATH(SANSTAR)+p->name
	else {
		strcat(tmp,"/");
		strcat(tmp,p->name);
	}
	if(tmp[strlen(tmp) - 1] == '/') tmp[strlen(tmp) - 1] = '\0';
	int c = countLinks(tmp , file);
	lseek(file, current, SEEK_SET);
	tmp = malloc(10);
	snprintf(tmp, 9, "%d ", c);
	write(1,tmp,strlen(tmp));
	free(tmp);
	
}


void usrAndGrp(struct posix_header * p){
	char * str = malloc(strlen(p->uname) + strlen(p->gname) + 3);
	strcpy(str,p->uname);
	strcat(str," ");
	strcat(str,p->gname);
	strcat(str," ");
	write(1,str,strlen(str));
	free(str);
}

void psize(struct posix_header * p){
	char * str = malloc(12);
//	char * endptr;
//	long size = strtol(p->size,&endptr,10); // equivalent de strtok -> atoi version long
//	if(strcmp(p->size,endptr) == 0)perror("error psize"); // check si un long a bien ete lu
//	snprintf(str, 11, "%ld", size); // convertit le long en string
//	size = octalConverter(str);
	int size;
	sscanf(p->size, "%o", &size);
	snprintf(str, 11 , "%d ", size);
	write(1, str, strlen(str));
	free(str);
}
//
void mtime(struct posix_header * p){
	char * str = malloc(14);
//	time_t t = (int) octalConverter(p->mtime);
	int tmp;
	sscanf(p->mtime, "%o", &tmp);
	time_t t = (int) tmp;
	struct tm * time = localtime(&t);
	strftime(str,14,"%b %d %R ",time);
	write(1,str,strlen(str));
	free(str);
}

//******** end -l ************
//long octalConverter (char * octal){ // convertit octal vers decimal : Char octal -> int decimal
//	long decimal = 0;
//	char * c = malloc(2);
//	for(size_t i = strlen(octal);i>0;i--){
//		c[0] = octal[i-1];
//		decimal += atoi(c) * pow(8,strlen(octal)-i);
//	}
//	return decimal;
//}

int countLinks(char * name ,int file){
	size_t n;
	int filesize = 0;
	int count = 2;
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while( ((n = read(file,p,BLOCKSIZE)) > 0) && (p->name[0] != '\0') ){
		if(validPath(name, p->name) == 1) count++;
		sscanf(p->size, "%o", &filesize);
		next_header(file, filesize);
	}
	return count;
}

//initialise i en fonction des options
int manageOption(int nbOptions, char ** path){
	withL = 0;
	if(nbOptions < 2) return 0;
	if(path[1][0] == '-'){
		if(strcmp(path[1],"-l") == 0) withL = 1;
		else return -1;
	}
	return 1 + withL;
		
}

int ls(int nbOptions, char ** path){
	storePosition();
	int i;
	if((i = manageOption(nbOptions, path)) == -1) return -1; //init i en fonction des options
	if((nbOptions < 2) || (nbOptions == 2 && withL == 1)) return LsWithoutPath();
	for(; i < nbOptions; i++){
		lsRep(path[i]);
		restorePosition();
		write(1,"\n",1);
	}
	return 1;
}

