#include "myCd.h"
#include "gestionnaire.h"
#include "check.h"
#include "storeRestore.h"
//#include "tar.h"

int cdNoOptions(){
  if(*TARPATH != '\0') setTarpath("\0");
  chdir(getenv("HOME"));
  return 1;
}

// fonction qui appelle cdPerso = commandPersonnalisee
int cdPerso(char * path){
  memset(TARPATH,0, strlen(TARPATH));
  if(path[0] == '/')return cdAbs(path);
	else return cd(path);
//  if(hasTar(path) == 1) return cd(path); // si dans le path il y un tar
//  if(chdir(path) == -1) return -1;
  return 1;
}

int cd(char * path){
	if(path[0] == '\0') return 1; //seg fault probable
	char * save = storeManually();
	char * bfTar = getPathBeforeTar(path);
	char * fromTar, * tar;
	if( (bfTar[0] != '\0') && (chdir(bfTar) == -1))// deplacement jusqu'au .tar
		return -1;
	if(strlen(bfTar) < strlen(path)){
		fromTar = pathFromTar(path);
		tar = findTar(fromTar);
		if(existTar(tar) == -1) return -1;
		setTarpath(tar);
		if(navigate(fromTar + strlen(tar) + 1) == -1){
			restoreManually(save); // restore
			return -1;
		}
	}
	return 1;
}

// se charge du path depuis un tarball
// NE SE CHARGE PAS DE RENTRER DANS LE .TAR (voir cd pour ça)
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
    if(strcmp(token, "..") == 0){
      if(i == 0){
				setTarpath("\0");
				if(cd(tmp) == -1){
					restoreManually(save);
					return -1;
				}
				return 1;
      }
			if(checkfp(tar, tabToString(i, fullpath)) == -1) // on verifie que le path est bon
				return displayError("cd", "No such file or directory");
      free(fullpath[i-1]); // on vide la derniere case du tableau fullpath
      i -= 2;
    }
    else{
        fullpath[i] = malloc(strlen(token) + 1);
        strcpy(fullpath[i], token);
    }
  }
  if(i == 0){
    setTarpath(tar);
    return 1;
  }
	char * fp = tabToString(i, fullpath);
	if(checkfp(tar, fp) == -1)
		return displayError("cd", "No such file or directory");
  tmp = malloc(strlen(tar) + strlen(fp) + 2);
  strcpy(tmp, tar);
  strcat(tmp, "/");
	strcat(tmp, tabToString(i, fullpath)); // modif
  setTarpath(tmp);
  free(tmp);
  return 1;
}

//int navigate(char * path){
//	if(path[0] == '/') return cdAbs(path);
//	if(path[0] == '\0') return 1;
//	char * save = storeManually();
//	char * fullpath = malloc(strlen(TARPATH) + strlen(path) + 2);
//	memset(fullpath, '\0', strlen(TARPATH) + strlen(path) + 2);
//	strcpy(fullpath, TARPATH);
//	strcat(fullpath,"/");
//	char * tar = strtok_r(fullpath, "/", &fullpath);
//	char * tmp = malloc(strlen(path) + 1);
//	strcpy(tmp,path);
//	char * token;
//	char * lastToken = getLastToken(fullpath);
//	strcat(lastToken, "/");
//	while((token = strtok_r(tmp, "/", &tmp)) != NULL){
//		if(strcmp(token, "..") == 0){
//			if(fullpath[0] == '\0'){
//				setTarpath("\0");
//				if(cd(tmp) == 1) return 1;
//				restoreManually(save);
//				free(fullpath);
//				return -1;
//			}
//			else strcpy(fullpath,pathWithoutLastToken(fullpath, lastToken));
//			if(checkfp(tar, fullpath) == -1) return -1;
//		}
//		else{
//			strcat(fullpath,token);
//			strcat(fullpath,"/");
//			lastToken = token;
//			strcat(lastToken, "/");
//		}
//	}
//	if(checkfp(tar, fullpath) == -1) return -1;
//	char * nvTarpath = malloc(strlen(tar) + strlen(fullpath) + 2);
//	memset(nvTarpath, '\0', strlen(tar) + strlen(fullpath) + 2);
//	strcpy(nvTarpath, tar);
//	if(fullpath[0] != '\0'){
//		strcat(nvTarpath, "/");
//		strcat(nvTarpath, fullpath);
//	}
//	setTarpath(nvTarpath);
//	return 1;
//}

//int checkfp2 (char * tar, char * fullpath){
//	if(fullpath[0] == '\0') return 1;
//	char * path = malloc(strlen(fullpath) + 2);
//	strcpy(path, fullpath);
////	if(fullpath[strlen(fullpath)-1] != '/')
////		strcat(path, "/");
//	return checkEntete(tar, path);
//}
// s'occupe des path commançant par / (path absolu)
// fait appel a cd dès qu'il trouve un .tar
int cdAbs(char * path){
//  storePosition();
	char * save = storeManually();
  TARPATH[0] = '\0'; // vide le TARPATH
  char  * tmp = malloc(strlen(path) + 1);
  strcpy(tmp, path);
  char * token;
  for(int n = 0;(token = strtok_r(tmp, "/\n", &tmp)) != NULL; n += strlen(token) +1){
    if(estTar(token) == 1){
      if(n == 0)cdNoOptions();
      else{
        tmp = malloc( n + 1);
        strncpy(tmp, path, n);
        tmp[n] = '\0';
        if(chdir(tmp) == -1){
          return -1;
        }
      }
      if(n < strlen(path) && cd(path+n+1) == -1){
//        restorePosition();
				restoreManually(save);
        return -1;
      }
      return 1;
    }
  }
	if(chdir(path) == -1) return -1;
	return 1;
}

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

// Check le FullPath fonction utilisee dans NAVIGATE
//int checkfp(char *tar, char *fullpath[50], int i){
//  char * path = malloc(strlen(fullpath[0]) + 2);
//  strcpy(path, fullpath[0]);
//
//  for(int x = 1; x < i; x++){
//    path = realloc(path, strlen(path) + strlen(fullpath[x]) + 2);
//    strcat(path, "/");
//    strncat(path, fullpath[x], strlen(fullpath[x]));
//  }
//  strcat(path, "/");
//  if (checkEntete(tar, path) == -1) {free(path); return -1;}
//  free(path);
//  return 1;
//}

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

void setTarpath(char * tarp){
  TARPATH = malloc(strlen(tarp) + 1);
	memset(TARPATH, '\0', strlen(tarp) + 1);
	if(tarp[strlen(tarp)-1] == '/')
		strncpy(TARPATH,tarp, strlen(tarp) -1);
	else
		strcpy(TARPATH,tarp);
}

int whichCd(char * pathCd) {
	if(pathCd[0] == '\0') return 1;
  //if tarpath vide -> cdPerso because we are not in tar file
  if (TARPATH[0] == '\0') {
    if(cdPerso(pathCd) == -1) return -1;
  }
  else
    if(navigate(pathCd) == -1) return -1;
  return 1;
}
