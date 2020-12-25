#include "gestionnaire.h"
#include "myCd.h"
#include "myCat.h"
#include "check.h"
#include "storeRestore.h"

int showContent(int file, struct posix_header * p){
	char buffer [BLOCKSIZE];
	ssize_t n;
	for(int i=0; i < ceil(atoi(p->size)/512.) ; i++){
		if((n = read(file, buffer, BLOCKSIZE)) > 0){
			write(1,buffer,n);
		}
	}
	close(file);
	return 0;
}

int simpleCat(char * path){
	char * cmd[3] = { "cat", path }; // on ajoute 1 a la taille pour que le                                    cat s'arrete apres un null
	return execCommand(cmd);

}

int CatFile (char * path){
	char * deplacement;
	char * lastToken = malloc (1);
	char * token;
	
	//je me deplace jusqu'a ce que je puisse voir le fichier a afficher
	char * tmp = malloc( strlen(path) + 1); // copie de path
	strcpy(tmp, path);
	
	while((token = strtok_r(tmp, "/",&tmp) )!= NULL){
		lastToken = realloc(lastToken,strlen(token) + 1); //recuperation du dernier token
		strcpy(lastToken, token);
	}
	
	deplacement = malloc(strlen(path) - strlen(lastToken) + 1); // on copie path - la taille du dernier token
	strncpy(deplacement, path, strlen(path) - strlen(lastToken));
	
	if(*TARPATH == '\0'){
		if(cdPerso(deplacement) == -1){
			perror("bad address");
			return -1;
		}
	}
	else {
		if(navigate(deplacement) == -1){
			perror("bad address");
			return -1;
		}
	}
	free(deplacement);
	//a decommenter
	// fin deplacement
	//	if(whichCd(deplacement) == -1) return -1;
	// free(deplacement);
	//	if(TARPATH[0] == '\0') return commandNoTar("cat",path);
	if(*TARPATH == '\0') return simpleCat(lastToken);
	tmp = malloc(strlen(TARPATH) + 1);
	strcpy(tmp, TARPATH);
	char * tar = strtok_r(tmp, "/\0",&tmp);
	long charToSkip = strlen(tar);
	if(strlen(tar)<strlen(TARPATH)) charToSkip++;
	char * npath = malloc(strlen(TARPATH + charToSkip) + strlen(lastToken) + 2);
	if(charToSkip<strlen(TARPATH)){
		strcpy(npath,TARPATH+charToSkip);
		strcat(npath, "/");
		strcat(npath,lastToken);
	}
	else strcpy(npath,lastToken);
	return readFile(npath, tar);
}

int readFile(char * path, char * tar){
	int f;
	size_t n;
	if((f = open(tar, O_RDONLY)) == -1) perror("open tar:");
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while((n = read(f,p,BLOCKSIZE)) > 0){
		if(p->name[0] == '\0') break;
		if(strcmp(p->name, path) == 0) return showContent(f,p);
		next_header(f, atoi(p->size));
	}
	close(f);
	perror("cat : file do not exist");
	return -1;
}


int cat(int nbOption, char ** path){
	if(nbOption < 2) return execCommand(path);
	storePosition();
	for(int i=1 ; i<nbOption;i++){
		if(CatFile(path[i]) == -1){
			restorePosition();
			return -1;
		}
		restorePosition();
	}
	return 0;
}

