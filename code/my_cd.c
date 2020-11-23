#include "my_cd.h"
#include "gestionnaire.h"
// #include "tar.h"
int moveTo(char * path, char * tarball){
  char * pwd = getcwd(NULL, 0);

  if(chdir(path)){
    perror("pathError:");
    return -1;
  }
  if(!existTar(tarball))
    return 0;
  chdir(pwd);
	printf("Move to\n"); // ici
  return -1;
}

int cdNoOptions(){
  if(*TARPATH != '\0') // docker
    strcpy(TARPATH,"\0");
  //if(TARPATH[0]=='\0')
    // TARPATH = NULL;
  chdir(getenv("HOME"));
  return 0;
}


// fonction qui appelle cdPerso = commandPersonnalisee
int cdPerso(char * path){
  if(!hasTar(path)) return cd(path); // si dans le path il y un tar

  chdir(path);
  return 0;
}

int cd(char * path ){
	
	char * tmp = malloc(strlen(path) + 1), *stdPath, *tbPath, *token;
	strcpy(tmp, path);
	int l = 0;
	while((token = strtok_r(tmp, "/", &tmp)) != NULL){
		if(estTar(token) == 0) break;
		l += 1 + strlen(token);
	}
	if(l > 0){ // si il y un path avant le tar
		if(path[0] != '/') l--;
		stdPath = malloc(l + 1);
		strncpy(stdPath, path, l);
		chdir(stdPath);
		free(stdPath);
	}
	
	if(l < strlen(path)){
		if(existTar(token) == -1){
			perror("le fichier n'existe pas");
			return -1;
		}
		TARPATH = realloc(TARPATH, strlen(token) + 1);
		strcpy(TARPATH, token);
		if( (l + strlen(token)) == strlen(path) ) return 0;
		l += strlen(token) + 1; // on rajoute la taille du tar au nb de char a sauter + /
		tbPath = malloc(l + 1);
		strcpy(tbPath, path+l);
		if((tbPath[0] == '/' && navigate(tbPath+1) == -1 )|| (tbPath[0] != '/' && navigate(tbPath) == -1)){
			TARPATH = realloc(TARPATH,1);
			strcpy(TARPATH,"\0");
			printf("cd 2 : %s \n", path); // ici
			free(tbPath);
			return -1;
		}
		free(tbPath);
	}
	else{
		TARPATH = realloc(TARPATH, 1);
		TARPATH[0] = '\0';
	}
	return 0;
}

int checkPath(char * path, char * token){
//	printf("dans le Check\n");
//	  printf("token :%s\n", token);
//	  printf("%s!\n", path);
	int file, n;
	if((file = open(token,O_RDONLY)) == -1){perror("error"); return -1;}
	
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while((n = read(file,p,BLOCKSIZE))>0){
//		    printf("%s!\n",p->name );
		if(((p-> typeflag == '5') && (strlen(path)+1 == strlen(p->name)) && (!strncmp(p->name, path, strlen(p->name)-1)))){
			close(file);
			return 0;
		}
		lseek(file,ceil(atoi(p->size)/512.)*BLOCKSIZE,SEEK_CUR);
	}
//	free(token);
	close(file);
	printf("fin check -1\n");
//	free(path);
	return -1;
}


void setTarpath(char * tarp){
	TARPATH = realloc(TARPATH, strlen(tarp) + 1);
	strcpy(TARPATH,tarp);
}
int checkfp(char *tar, char *fullpath[50], int i){
	char * path = malloc(strlen(fullpath[0]) + 1);
	strcpy(path, fullpath[0]);
	
	for(int x = 1; x < i; x++){
		path = realloc(path, strlen(path) + strlen(fullpath[x]) + 2);
		strncat(path, "/", 1);
		strncat(path, fullpath[x], strlen(fullpath[x]));
	}
	if(checkPath(path, tar) == -1) {free(path); return -1;}
	free(path);
	return 0;
}
int navigate(char * path){
	if(path[0] == '/') return cdPerso(path); // si le path est absolu
	char *fullpath[50], *tmp, *token, *tar;
	tmp = malloc(strlen(TARPATH) + 1);
	strncpy(tmp, TARPATH, strlen(TARPATH));
	tar = strtok_r(tmp, "/\0",&tmp); // on stock le .tar
	int i = 0; // nb d'elements dans TARPATH;
	int l = 0; //nb de char parcourus dans path
	if(strlen(tar) < strlen(TARPATH)){ //on stock le tarpath dans FP
		for(;(token = strtok_r(tmp,"/\n",&tmp)) != NULL; i++){
			fullpath[i] = malloc(strlen(token) + 1);
			strcpy(fullpath[i], token);
		}
	}
	// on passe au path
	tmp = realloc(tmp, strlen(path) + 1);
	strcpy(tmp, path); // on copie le path
	for(;(token = strtok_r(tmp,"/\n",&tmp)) != NULL; i++, l+= strlen(token) + 1){
		if(strcmp(token, "..") == 0){
			if(i == 0){
				l+= strlen(token);
				if(l == strlen(path)){ // si on atteint la fin du path
					setTarpath("\0");
					return 0;
				}
				return cdPerso(path + strlen(tmp));
			}
			if(checkfp(tar, fullpath, i) == -1){printf("failed\n");return -1;} // exit
			free(fullpath[i-1]);
			i -= 2;
		}
		else{
			fullpath[i] = malloc(strlen(token) + 1);
			strcpy(fullpath[i], token);
		}
	}
	free(tmp); tmp = NULL;
	if(i == 0){
		setTarpath(tar);
		return 0;
	}
	if(checkfp(tar, fullpath, i) == -1){ printf("failed2\n"); return -1; }//exit
	
	for(int x = 1; x < i; x++){
		fullpath[0] = realloc(fullpath[0], strlen(fullpath[0]) + strlen(fullpath[x]) + 2);
		strncat(fullpath[0], "/", 1);
		strncat(fullpath[0], fullpath[x], strlen(fullpath[x]));
		free(fullpath[x]); fullpath[x] = NULL;
	}
	tmp = realloc(tmp, strlen(tar) + strlen(fullpath[0]) + 2);
	strcpy(tmp, tar);
	strncat(tmp, "/", 1);
	strncat(tmp, fullpath[0], strlen(fullpath[0]));
	setTarpath(tmp);
	free(fullpath[0]);
	free(tmp); tmp = NULL;
	   return 0;
	
}
