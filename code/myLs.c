#include "myLs.h"
#include "myCd.h"
#include "gestionnaire.h"
#include "check.h"
#include "storeRestore.h"

// ls sans path en arguments
int LsWithoutPath(int withL){
	if(*TARPATH == '\0')
		simpleLs(withL);
	else printOccurences(withL);
	return 0;
}

// Ls sans options avec un path
int lsRep(char * path, int withL){
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
	
	if(*TARPATH == '\0') simpleLs(withL);
	else printOccurences(withL);
	return 0;
}

void simpleLs(int withL){
	char * cmd[3] = {"ls"};
	if(withL == 1)
		cmd[1] = "-l";
	execCommand(cmd);
	return;
}

void printOccurences(int withL){ //si withL = 1 on affiche ls -l
	char * tmp = malloc(strlen(TARPATH) + 1);
	strcpy(tmp, TARPATH);
	char * tar = strtok_r(tmp, "/\0",&tmp);
	int f;
	ssize_t n;
	write(1,TARPATH,strlen(TARPATH));
	write(1,":\n", 2);
	if((f = open(tar, O_RDONLY)) == -1) perror("open tar:");
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while((n = read(f,p,BLOCKSIZE)) > 0){
		if(p->name[0] == '\0') break;
		if(strlen(TARPATH) > strlen(tar)){
			if(validPath(TARPATH + strlen(tar)+1 ,p->name) == 0){
				if(withL == 1) optionL(p,f);
				write(1,p->name + strlen(TARPATH + strlen(tar) + 1)+ 1, strlen(p->name + strlen(TARPATH + strlen(tar) + 1)));
				write(1,"\n",1);
			}
		}
		else if(validPath("", p->name) == 0){
			if(withL == 1) optionL(p,f);
			write(1,p->name, strlen(p->name));
			write(1,"\n",1);
		}
		next_header(f, atoi(p->size));
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
		if((token = strtok_r(targ,"/",&targ)) == NULL) return 0; // si token null alrs on a donc bien target qui est de profondeur 1 de plus que path
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

void rights(struct posix_header * p){
	char * perm[8] = {"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
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
//
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
	if(strlen(tar) == strlen(TARPATH)) strcpy(tmp,p->name);
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

//
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
	//	if(strcmp(p->name,"matin") == 0){ write(1,"\n",1);exit(EXIT_FAILURE);}
	char * str = malloc(12);
	char * endptr;
	long size = strtol(p->size,&endptr,10); // equivalent de strtok -> atoi version long
	if(strcmp(p->size,endptr) == 0)perror("error psize"); // check si un long a bien ete lu
	snprintf(str, 11, "%ld", size); // convertit le long en string
	size = octalConverter(str);
	snprintf(str, 11 , "%ld ", size);
	write(1, str, strlen(str));
	free(str);
}
//
void mtime(struct posix_header * p){
	char * str = malloc(14);
	time_t t = (int) octalConverter(p->mtime);
	struct tm * time = localtime(&t);
	strftime(str,14,"%b %d %R ",time);
	write(1,str,strlen(str));
	free(str);
}

//******** end -l ************
long octalConverter (char * octal){ // convertit octal vers decimal : Char octal -> int decimal
	long decimal = 0;
	char * c = malloc(2);
	for(size_t i = strlen(octal);i>0;i--){
		c[0] = octal[i-1];
		decimal += atoi(c) * pow(8,strlen(octal)-i);
	}
	return decimal;
}

int countLinks(char * name ,int file){
	size_t n;
	int count = 2;
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while((n = read(file,p,BLOCKSIZE)) > 0)
		if(validPath(name, p->name) == 0) count++;
	return count;
}

int ls(int nbOptions, char ** path){
	storePosition();
	int i = 1, withL = 0;
	if(nbOptions < 2) return LsWithoutPath(0);
	if(strcmp(path[1],"-l") == 0){
		if(nbOptions == 2) return LsWithoutPath(1);
		i = 2;
		withL = 1;
	}
	else i=1;
	for(; i < nbOptions; i++){
		if(lsRep(path[i],withL) == -1) {
			perror("no such file or directory");
			restorePosition();
			return -1;
		}
		restorePosition();
	}
	return 0;
}

