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
//	printf("ici\n");
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
//		printf("path: %s   path test: %s  token %s\n",path, )
//		printf("taille path: %lu   taille test: %lu \n", strlen(path), l + strlen(token) + 1);
		if( (l + strlen(token)) == strlen(path) ) return 0;
		l += strlen(token) + 1; // on rajoute la taille du tar au nb de char a sauter + /
		tbPath = malloc(l + 1);
		strcpy(tbPath, path+l);
//		printf("tbPath : %s TARP : %s\n",tbPath, TARPATH);
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
//	
//	printf("dans le Check\n");
//	  printf("token :%s\n", token);
//	  printf("%s!\n", path);
	int file, n;
	if((file = open(token,O_RDONLY)) == -1){perror("error"); return -1;}
	
	struct posix_header * p = malloc(sizeof(struct posix_header));
	while((n = read(file,p,BLOCKSIZE))>0){
//		    printf("%s!\n",p->name );
		if(((p-> typeflag == '5') && (strlen(path)+1 == strlen(p->name)) && (!strncmp(p->name, path, strlen(p->name)-1)))){
//			size_t length = strlen(path);
//			strcpy(TARPATH,"\0");//problem
//			if(path[strlen(path)-1]=='/') length--;
//			TARPATH = realloc(TARPATH, strlen(token) + length + 1);
//			strcpy(TARPATH,token);
//			strncat(TARPATH,"/",strlen("/"));
//			strncat(TARPATH, path, length);
//			printf("match :%s\n",p->name);
//			free(token);
			close(file);
//			free(path);
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
//		printf("AJOUT : %s \n", fullpath[x]);
		path = realloc(path, strlen(path) + strlen(fullpath[x]) + 2);
		strncat(path, "/", 1);
		strncat(path, fullpath[x], strlen(fullpath[x]));
	}
//	printf("CheckFP : %s \n", path);
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
//					printf("ici\n");
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
			if(estTar(token) != 0){
			fullpath[i] = malloc(strlen(token) + 1);
			strcpy(fullpath[i], token);
			}
			else i--;

		}
		
	}
	
//	printf("TARP : %s\n", TARPATH);
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
	printf("tmp : %s\n",tmp);
	setTarpath(tmp);
	free(fullpath[0]);
//	   TARPATH = realloc(TARPATH, strlen(fullpath[0]) + 1);
//	   strncpy(TARPATH, fullpath[0], strlen(fullpath[0]));
	free(tmp); tmp = NULL;
//	free(fullpath[0]); fullpath[0] = NULL;
//	printf("TARP3 : %s\n", TARPATH);
	   return 0;
	
}
// fonction pere = commandTar
//int navigate(char * path){ // ..
//  if(path[0] == '/') return cd(path); // si le path est absolu
//  char * fullpath[100]; // liste qui va separer le path
//
//  char * token;
//  char * tmp = malloc(strlen(path)+1);
//  memcpy(tmp,path,strlen(path));
//
//  int i = 0;
//  while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
////    printf("dans le while\n");
//    if(!strcmp(token,"..")){
//
//      if(i == 0) dotdot(tmp); //si le path nous mene hors du tarball
//      else{
//        fullpath[i-1] = NULL;// on recule d'un cran dans le path
//        i--;
//      }
//    }
//    else { // si c'est pas ..
//      fullpath[i] = malloc(strlen(token)+1);
//      memcpy(fullpath[i],token,strlen(token));
//      i++;
//    }
//  }
//  if(i == 0)return 0;//si le fullpath est null on sort
//
//  //transformation du tableau de string("java") en chaine de caractere
//  for(int x = 1;x<i;x++){
//    strcat(fullpath[0],"/");
//    strcat(fullpath[0],fullpath[x]);
//	free(fullpath[x]);
//  }
//  if(*TARPATH == '\0') cdPerso(fullpath[0]);
//
////  printf("FULLPATH:%s!\n",fullpath[0]);
//  char * tarp = malloc(strlen(TARPATH)+1); //problem
//  //memcpy(tarp,TARPATH,strlen(TARPATH));// tarp = TARPATH copie
//  strcpy(tarp, TARPATH);
//
//  token = strtok_r(tarp,"/",&tarp);
//  // char * tarfile = malloc(strlen(token)+1);
//  // strcpy(tarfile, token);
//  // token = strtok_r(tarp,"/",&tarp);
//  // rest = strtok_r(tarp,"/",&tarp);
////docker
//  if((strlen(TARPATH) - strlen(token)) > 0){ // pb docker
//    // if(tarp[0]!='\0'){
////    printf("tarp pas null\n");
//    char * tmp2 = malloc(strlen(tarp) + strlen(fullpath[0]) + 2);
//    strcpy(tmp2,tarp);
//    strcat(tmp2, "/");
//    strncat(tmp2,fullpath[0],strlen(fullpath[0]));
//	  printf("path tmp: %s!\n",tmp2);
//    return checkPath(tmp2, token); // token toujours le fichier.tar
//  }
//	printf("path fp: %s!\n",fullpath[0]);
//	if(checkPath(fullpath[0], token) == -1){
////		free(fullpath);
////		fullpath = NULL;
//		return -1;
//	}
////	fullpath = NULL;
////	free(fullpath);
//	return 0;
//}

// fonction pere : navigate
//int dotdot(char * path){//..
//  char * token;
//  char * tmp2 = malloc(sizeof(char)+1);
//  if(*TARPATH == '\0'){   //tarpath null
//    // printf("yes\n");
//    // if(*path == '\0') docker
//    if(path == NULL) return chdir("..");
//    if(findTar(path) == NULL){ // il n'y a pas de tar
////      printf("pas de tar\n");
//      return chdir(path);
//    }
//    else { //il y un tar
//      while((token = strtok_r(path,"/\n",&path))!=NULL){
//        if(!estTar(token)) break;
//        strcat(tmp2,token);
//        strcat(tmp2,"/");
//      }
//		if(chdir(tmp2)) return -1;
//      memcpy(TARPATH,token,strlen(token));
//      return 0;
//    }
//  } // fin TARPATH == NULL
////printf("dotdot\n");
//  // ici TARPATH != NULL
//  char * tmp = malloc(strlen(TARPATH)+1);
//  memcpy(tmp,TARPATH,strlen(TARPATH));
//  // int length = 0;
//  while((token = strtok_r(tmp,"/",&tmp))!=NULL){
//    tmp2 = realloc(tmp2,strlen(token)+1);
//    strcpy(tmp2,token);
//  }
//
//  if((strlen(TARPATH) - strlen(tmp2))!=0){
//    char * newTARPATH = malloc(strlen(TARPATH) - strlen(tmp2));
//    strncpy(newTARPATH, TARPATH,strlen(TARPATH) - strlen(tmp2) - 1);
//    TARPATH = realloc(TARPATH, strlen(newTARPATH) + 1);
//    strcpy(TARPATH, newTARPATH);
//  }
//  else {
//    TARPATH = realloc(TARPATH, 1);
//    strcpy(TARPATH, "\0");
//  }
//  return 0;
//}
//
//int cd(char * path){
////  printf("cd\n");
//  char * basicPath = malloc(sizeof(char)+1);
//  char * token;
//  if(path[0]=='/') strcpy(basicPath,"/");
//  while((token = strtok_r(path,"/\n",&path))!=NULL){
//    if(!estTar(token)){
//		if(basicPath[0] != '\0' && moveTo(basicPath, token)) { perror("cd");return -1;}
//      if(existTar(token)) return -1;
//      TARPATH = realloc(TARPATH, strlen(token)+1);
//      strncpy(TARPATH,token,strlen(token));
////      printf("peut etre\n");
//      if(path == NULL) return 0; //pb
//      printf("break\n");
//      break;
//    }
//    strncat(basicPath, token,strlen(token));
//    strncat(basicPath,"/",1);
//  }
////  printf("le reste du path:%s!\n",path );
//	printf("TARPATH before navigate : %s  path : %s\n ", TARPATH, path);//ici
//	printf("basicpath : %s\n", basicPath);
//
//  if(navigate(path) == -1){
//    strcpy(TARPATH,"\0");
//	  printf("cd 2 : %s \n", path); // ici
//    return -1;
//  }
//  return 0;
//}

//int navigate(char * path){
//	if(path[0] == '\0') return cdNoOption();
//	if(path[0] == '/') return cdPerso(path); // si le path est absolu
//	char *fullpath[50], *tmp = malloc(strlen(path) + 1), *token;
//	strncpy(tmp, path, strlen(path));
//	int i;
//
//	while((token = strtok_r(tmp,"/\n",&tmp))!=NULL){
//		// on recule lorsque l'on tombe sur ..
//		if( !strncmp(token, "..", strlen(token)) ){
//			if(i == 0) dotdot(tmp); // si le path nous mene hors du tarball
//			else {
//				free(fullpath[i-1]);
//				i--;
//			}
//		}
//		// fin ..
//		else { // on initialise le tableau du chemin
//			fullpath[i] = malloc(strlen(token) + 1);
//			strncpy(fullpath[i], token, strlen(token))
//		}
//	}
//
//}
