#include "myLs.h"
#include "myCd.h"
#include "gestionnaire.h"
#include "check.h"
#include "storeRestore.h"

int withL;

//print le dossier a lister
void writeName(){
	char * lastToken;
	if(TARPATH[0] != '\0') lastToken = getLastToken(TARPATH);
	else lastToken = getLastToken(getcwd(NULL, 0));
	write(1,lastToken,strlen(lastToken));
	write(1,":\n",2);
}


// check si target est visible depuis path 1
int validPath(char * path, char * target){
	char * tmp = malloc(strlen(path) + 1);
	strcpy(tmp,path);
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

/* utilise le bon validPath en fonction de la position actuelle
 si on se trouve a la racine renvoie validpath avec une chaine vide */
int whichValid(char * tar, char * name){
	if(strlen(TARPATH) > strlen(tar))
		return validPath(TARPATH + strlen(tar)+1 , name);
	else return validPath("", name);
}


// ********* -l ************

// retourne le type de fichier
void typeFic(struct posix_header * p){
	char * type[8] = {"-","l","l","c","b","d","p","s"};
	char * typeflag = malloc(2);
	typeflag[0] = p->typeflag; // transforme char en string
	write(1,type[atoi(typeflag)],1);
}

// affiche les droits usr grp et other du fichier
void rights (struct posix_header * p){
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

/* compte le nombre de hard links en retounant le nombre d'elements contenu
 dans un dossier + 2 ( . et .. ) */
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

/* retourne le nombre de hard link
 renvoie 1 si ce n'est pas un dossier */
void nbrlink(struct posix_header * p, int file){
	if(p->typeflag != '5'){ // pas un repertoire donc 1
		write(1,"1 ",2);
		return;
	}
	off_t current = lseek(file, 0, SEEK_CUR);//sauvegarde la position (de lecture) dans le tar
	lseek(file,0,SEEK_SET);// on retourne au debut du tar
	char * tmp = malloc(strlen(TARPATH)+ 1);
	strcpy(tmp, TARPATH);
	char * tar = strtok_r(tmp, "/",&tmp);
	tmp = malloc(strlen(TARPATH) - strlen(tar) + strlen(p->name) + 3); // recupere le nom du dossier a afficher les liens
	if(strlen(tar) == strlen(TARPATH)) strcpy(tmp,p->name); //TARPATH(SANSTAR) + p->name
	else { // si le tarpath n'est pas a la racine
		strcat(tmp,"/");
		strcat(tmp,p->name);
	}
	int c = countLinks(tmp , file);
	lseek(file, current, SEEK_SET);
	tmp = malloc(10);
	snprintf(tmp, 9, "%d ", c);
	write(1,tmp,strlen(tmp));
	free(tmp);
	
}

// retourne le nom usr et grp
void usrAndGrp(struct posix_header * p){
	char * str = malloc(strlen(p->uname) + strlen(p->gname) + 3);
	strcpy(str,p->uname);
	strcat(str," ");
	strcat(str,p->gname);
	strcat(str," ");
	write(1,str,strlen(str));
	free(str);
}

//retourne la taille du fichier
void psize(struct posix_header * p){
	char * str = malloc(12);
	int size;
	sscanf(p->size, "%o", &size);
	snprintf(str, 11 , "%d ", size);
	write(1, str, strlen(str));
	free(str);
}

// retourne la date de la derniere modification
void mtime(struct posix_header * p){
	char * str = malloc(14);
	int tmp;
	sscanf(p->mtime, "%o", &tmp); // convertit en decimal le nombre de sec unix
	time_t t = (int) tmp;
	struct tm * time = localtime(&t); // convertit unixTime
	strftime(str,14,"%b %d %R ",time); // changement du format d'affichage
	write(1,str,strlen(str));
	free(str);
}

// affiche en version longue le fichier
void optionL (struct posix_header * p, int file){
	typeFic(p);
	rights(p);
	nbrlink(p,file);
	usrAndGrp(p);
	psize(p);
	mtime(p);
}

//******** end -l ************

// print les fichiers visible depuis la position actuelle
void printOccurences(){
	char * tar = findTar(TARPATH), * lastTok;
	int f, filesize = 0;
	ssize_t n;
	if((f = open(tar, O_RDONLY)) == -1){ // lecture du .tar
		displayError("failed to open tar");
		return;
	}
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while( ((n = read(f,p,BLOCKSIZE)) > 0) && (p->name[0] != '\0')){
		if(whichValid(tar, p->name) == 1){
			if(withL == 1) optionL(p,f); // lance l'option l si elle est utilisée
			lastTok = getLastToken(p->name);
			write(1, lastTok, strlen(lastTok));
			write(1,"\n",1);
		}
		sscanf(p->size, "%o", &filesize);
		next_header(f, filesize);
	}
	close(f);
}

//ls -l ou ls sans arguments 
void simpleLs(){
	if(withL == 1)
		commandNoTar("ls", "-l");
	else{
		char * cmd[2] = {"ls"};
		execCommand(cmd);
	}
	return;
}

// ls sans path en arguments
int LsWithoutPath(){
	if(*TARPATH == '\0')
		simpleLs();
	else printOccurences();
	return 1;
}


// Ls sans options avec un path
int lsRep(char * path){
	if(whichCd(path) == -1) return -1; //deplacement vers le dossier a afficher
	writeName();
	if(TARPATH[0] == '\0')
		simpleLs();
	else printOccurences();
	write(1,"\n",1);
	return 1;
}

//initialise i en fonction des options
int manageOption(int nbOptions, char ** path){
	withL = 0;
	if(nbOptions < 2) return 0;
	if(path[1][0] == '-'){
		if(strcmp(path[1],"-l") == 0) withL = 1;
		else return displayError("invalid option");
	}
	return 1 + withL;
}

//fonction appelée dans gestionnaire
int ls(int nbOptions, char ** path){
	storePosition();
	int i; // indique la position ou commence les paths
	//init i en fonction de la presence ou non de -l
	if((i = manageOption(nbOptions, path)) == -1) return -1;
	if((nbOptions < 2) || (nbOptions == 2 && withL == 1)) return LsWithoutPath();
	for(; i < nbOptions; i++){
		lsRep(path[i]);
		restorePosition();
	}
	return 1;
}

