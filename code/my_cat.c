#include "gestionnaire.h"
//#include "tar.h"
#include "my_cd.h"
#include "my_cat.h"

char * myPos, * myPosTar;

void goToInitialPos(){
	chdir(myPos);
	TARPATH = realloc(TARPATH, strlen(myPosTar) + 1);
	strcpy(TARPATH, myPosTar);
}
int isAbsolute(char * path){
	return (path[0] == '/');
}

int showContent(int file, struct posix_header * p){
	char buffer [BLOCKSIZE];
	int n;
	for(int i=0; i < ceil(atoi(p->size)/512.) ; i++){
		if((n = read(file, buffer, BLOCKSIZE)) > 0){
			write(1,buffer,n);
		}
	}
	close(file);
	return 0;
}

int simpleCat(char * path){
	int f;
	char * cmd[3] = { "cat", path }; // on ajoute 1 a la taille pour que le                                    cat s'arrete apres un null
	switch (f = fork()) {
		case -1 : perror("fork :");
			      exit(1);
			
		case 0 : if(execCommand(cmd) == -1) perror("cat");
			
		default: signal(SIGINT, SIG_IGN); // ignore le signal SIGINT
			     wait(NULL);
			     write(1,"\n",1);
			     return 0;
	}
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
//	if((*TARPATH == '\0' && cdPerso(deplacement) == -1) || (*TARPATH != '\0' && navigate(deplacement) == -1)){
//		perror("bad address");
//		return -1;
//	}
	if(*TARPATH == '\0'){
		if(cdPerso(deplacement) == -1){
			perror("bad address");
			return -1;
		}
	}
	else
		if(navigate(deplacement) == -1){
			perror("bad address");
			return -1;
		}
	
	// fin deplacement
	if(checkpath(lastToken) == -1){
		free(lastToken);
		free(deplacement);
		return -1;
	}
	free(lastToken);
	free(deplacement);
	goToInitialPos();
//	printf("nbOption cat : %d\n", nbOption);
	return 0;
}


int checkpath(char * path){
	char * token;
	char * filePath;
	int f, n;
	struct posix_header * p = malloc(sizeof(struct posix_header));
	char * tmp = malloc(strlen(TARPATH) + 1);
	strcpy(tmp,TARPATH);
	if(*TARPATH != '\0'){
		token = strtok_r(tmp, "/",&tmp);

		if(tmp == NULL){
			filePath = malloc(strlen(path) + 1);
			strcpy(filePath, path);
		}
		else{
			filePath = malloc(strlen(tmp) + strlen(path) + 2);
			strcpy(filePath, tmp);
			strcat(filePath, "/");
			strcat(filePath, path);
		}
		if((f = open(token,O_RDONLY)) == -1) {perror("open tar:"); return -1; }
		
		while((n = read(f,p,BLOCKSIZE)) > 0){
			if(p->name[0] == '\0'){ perror("le fichier n'existe pas:"); break;} //on atteint les blocs de fin
			if(p->typeflag == '0' && !strcmp(filePath,p->name)){
				showContent(f, p);
				return 0;
			}
			lseek(f,ceil(atoi(p->size)/512.)*BLOCKSIZE,SEEK_CUR);//saut
		}
		close(f);
		return -1;
	}
	//si on est hors d'un tarball on exec juste cat
//		if((f = fork()) == -1){ perror("fork :"); exit(1);}
//		if(f == 0)	execlp("cat","path");
//		wait(NULL);
	printf("i'm here \n");
	return simpleCat(path);
//		return 0;
}



int cat(int nbOption, char ** path){
	int f;
	if(nbOption < 2)
		switch (f = fork()) {
			case -1 : perror("fork :");
					  exit(1);
				
			case 0 : execCommand(path);
				
			default: signal(SIGINT, SIG_IGN); // ignore le signal SIGINT
				     wait(NULL);
			         write(1,"\n",1);
				     return 0;
		}
	
	myPos = getcwd(NULL, 0); // je sauvegarde ma position
	myPosTar = malloc(strlen(TARPATH) + 1);
	strcpy(myPosTar, TARPATH);
	for(int i=1 ; i<nbOption;i++){
		printf(YELLOW "path : %s \n" RESET,path[i]);
		if(CatFile(path[i]) == -1){goToInitialPos(); return -1; }
	}
	free(myPosTar);
	return 0;
}
