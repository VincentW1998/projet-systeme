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
	char * lastToken = getLastToken(path);
	char * deplacement = pathWithoutLastToken(path, lastToken);
	if(whichCd(deplacement) == -1){
		free(deplacement);
		return -1;
	}
	free(deplacement);
	if(TARPATH[0] == '\0') return simpleCat(lastToken); //si hors du tarball exec
	char * tar = findTar(TARPATH);
	char * pathAfterTar = pathFromTar(path) + strlen(tar) + 1; // recup le path apres tar
	return readFile(pathAfterTar, tar); //readfile sur le path apres le TAR
}

int readFile(char * path, char * tar){
	int f;
	size_t n;
	if((f = open(tar, O_RDONLY)) == -1) perror("open tar:");
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while( ((n = read(f,p,BLOCKSIZE)) > 0) && (p->name[0] != '\0') ){
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

