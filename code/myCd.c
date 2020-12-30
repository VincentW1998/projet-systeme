#include "myCd.h"
#include "gestionnaire.h"
#include "check.h"
#include "storeRestore.h"
//#include "tar.h"

//transforme un tab de string en string : char ** -> string
char * tabToString(int l, char ** fp){
	char * path = malloc(sizeof(fp) + 1);
	memset(path, '\0', sizeof(fp) + 1);
	strcpy(path,fp[0]);
	for(int i = 1; i<l; i++){
		strcat(path,"/");
		strcat(path, fp[i]);
	}
	return path;
}

//check si le path est bon jusqu'a present
int checkfp(char * tar, char * fp){
	char * fullpath = malloc(strlen(fp) + 2);
	strcpy(fullpath, fp);
	strcat(fullpath, "/");
	if (checkEntete(tar, fullpath) == -1) {
		free(fullpath);
		return -1;
	}
	free(fullpath);
	return 1;
}

//cd sans arguments (n'appel pas execCommand car elle ne prend pas cd en charge)
int cdNoOptions(){
  if(*TARPATH != '\0') setTarpath("\0");
  chdir(getenv("HOME"));
  return 1;
}

//fonction cd simple hors du tarball, appele la fonction Navigate si rencontre un tarball
int cd(char * path){
	if(path[0] == '\0') return 1;
	char * save = storeManually();
	char * bfTar = getPathBeforeTar(path);
	char * fromTar, * tar;
	if( (bfTar[0] != '\0') && (chdir(bfTar) == -1))
		return displayError("No such file or directory");
	if(strlen(bfTar) < strlen(path)){ // si le path ne contient pas de tarball
		fromTar = pathFromTar(path);
		tar = findTar(fromTar);
		if(existTar(tar) == -1) return displayError("No such file or directory"); //verifie que le tarball existe
		setTarpath(tar);
		if(strcmp(fromTar,tar) == 0) return 1;
		if(navigate(fromTar + strlen(tar) + 1) == -1){ // fait appel a navigate avec le path apres le tarball
			restoreManually(save);
			return -1;
		}
	}
	return 1;
}

// fonction appelée dans commandPersonnalisee
int cdPerso(char * path){
	memset(TARPATH,0, strlen(TARPATH));
	if(path[0] == '/')return cdAbs(path); //si le path est absolu on fait appel a cdAbs
	else return cd(path);
	return 1;
}

/* se charge du path depuis un tarball
NE SE CHARGE PAS DE RENTRER DANS LE .TAR (voir cd pour ça) */
int navigate(char * path){
	if(path[0] == '\0') return 1;
  if(path[0] == '/') return cdAbs(path);
	char * save = storeManually();
	char *fullpath[50], *token;
  char * tmp = malloc(strlen(TARPATH) + 1);
  strcpy(tmp, TARPATH);
  char * tar = strtok_r(tmp, "/\0",&tmp); // on stock le .tar
  int i = 0; // nb d'elements dans TARPATH;
	
  if(strlen(tar) < strlen(TARPATH)){ //on stock le tarpath dans FP
    for(;(token = strtok_r(tmp,"/\n",&tmp)) != NULL; i++){
      fullpath[i] = malloc(strlen(token) + 1);
      strcpy(fullpath[i], token);
    }
  }
  // on passe au path
  tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path); // on copie le path
	// l = au nombres de char parcourus
  for(int l = 0;(token = strtok_r(tmp,"/\n",&tmp)) != NULL; i++, l+= strlen(token) + 1){
    if(strcmp(token, "..") == 0){ // si rencontre un ..
      if(i == 0){ // si le path nous mene hors du tarball
				setTarpath("\0");
				if(cd(tmp) == -1){ // nous redirige vers cd car nous quittons le tarball
					restoreManually(save);
					return -1;
				}
				return 1;
      }
			if(checkfp(tar, tabToString(i, fullpath)) == -1) // on verifie que le path est bon
				return displayError("No such file or directory");
      free(fullpath[i-1]); // on vide la derniere case du tableau fullpath
      i -= 2;
    }
    else{// rajoute le token dans fullpath
        fullpath[i] = malloc(strlen(token) + 1);
        strcpy(fullpath[i], token);
    }
  }
  if(i == 0){ //si le fullpath est vide
    setTarpath(tar);
    return 1;
  }
	char * fp = tabToString(i, fullpath); // convertit fullpath en string
	if(checkfp(tar, fp) == -1)
		return displayError("No such file or directory");
	//le path est bon on change le tarpath
  tmp = malloc(strlen(tar) + strlen(fp) + 2);
  strcpy(tmp, tar);
  strcat(tmp, "/");
	strcat(tmp, tabToString(i, fullpath));
  setTarpath(tmp);
  free(tmp);
  return 1;
}

// s'occupe des path commançant par / (path absolu)
// fait appel a cd dès qu'il trouve un .tar
int cdAbs(char * path){
	char * save = storeManually();
	setTarpath("\0"); // vide tarpath
	char * beforeTar = getPathBeforeTar(path);
	char * pFromTar = pathFromTar(path);
	if( (chdir(beforeTar) == -1) || (cd(pFromTar) == -1) ){
		restoreManually(save);
		return displayError("No such file or directory");
	}
	return 1;
}


// choisit quel cd utiliser en fonction de la position actuelle
int whichCd(char * pathCd) {
  if (TARPATH[0] == '\0') {
    if(cdPerso(pathCd) == -1) return -1;
  }
  else
    if(navigate(pathCd) == -1) return -1;
  return 1;
}
