#include "myMkdir.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"

struct posix_header newHeader(const char * path) {
  struct posix_header hd;
  uid_t uid = getuid();
  struct passwd * usr = getpwuid(uid);
  time_t lastModif;
  time(&lastModif);

  struct group * grp = getgrgid(usr -> pw_gid);

//initialise le struct avec des '\0'
  memset(&hd, '\0', sizeof(struct posix_header));
  // on remplit les champs du struct posix_header
  strncpy(hd.name, path, 100);
  strcpy(hd.mode, "0000755");
  snprintf(hd.uid, sizeof(hd.uid), "%07o", usr -> pw_uid); // uid
  snprintf(hd.gid, sizeof(hd.gid),"%07o", usr -> pw_gid); // gid

  strcpy(hd.size, "00000000000");
  snprintf(hd.mtime, sizeof(hd.mtime), "%lo", lastModif); // mtime
  hd.typeflag = '5'; // typeflag
  memcpy(hd.magic, OLDGNU_MAGIC, strlen(OLDGNU_MAGIC));
  strncpy(hd.uname, usr -> pw_name, 32); // uname
  strncpy(hd.gname, grp -> gr_name, 32); // gname
  memset(hd.linkname, '\0', sizeof(hd.linkname)); // linkname
  memset(hd.devmajor, '\0', sizeof(hd.devmajor)); // devmajor
  memset(hd.devminor, '\0', sizeof(hd.devminor)); // devminor
  memset(hd.prefix, '\0', sizeof(hd.prefix)); // prefix
  memset(hd.junk, '\0', sizeof(hd.junk)); // junk
  set_checksum(&hd); // checksum
  check_checksum(&hd);

  return hd;
}

char * createPathForMkdir(const char * path) {

  char * suiteName = subWithoutTar();

/* +3 car on rajoute 2 slash et il y a le caractere zero qui termine une
 * chaine de caracteres. */
int length = strlen(suiteName) + strlen(path) + 3;
  char * pathWithFolder = malloc(length);
  pathWithFolder[0] = '\0';
  strncat(pathWithFolder, suiteName, strlen(suiteName));
  if (suiteName[0] != '\0') {
    strcat(pathWithFolder, "/");
  }
  strncat(pathWithFolder, path, strlen(path));
  strcat(pathWithFolder, "/");
  return pathWithFolder;
}

int mkdirTar(int nbOption,char ** command) {

  for (int i = 1; i < nbOption; i++) {
    if(createRepo(command[i]) == -1)
      return -1;
  }

 return 1;
}

int createRepo(char * path){

  pos = getcwd(NULL, 0);
  posTar = malloc(strlen(TARPATH) + 1);
  strcpy(posTar, TARPATH);

  char * pathNavigate= subWithoutRepo(path);
  if(navigate(pathNavigate) == -1) return -1;

  char * pathMkdir = subWithRepo(path);
  int fd, n;
  char * tarName = substringTar();

  fd = open(tarName, O_RDWR); // on ouvre le fichier tar
  if (fd < 0){
    perror("open fichier Tar");
    return -1;
  }
  // concatene path et TARPATH et rajoute un slash a la fin
  char * pathWithFolder = createPathForMkdir(pathMkdir);

  struct posix_header hd = newHeader(pathWithFolder);

  if((n = checkEntete2(tarName, pathWithFolder, &hd)) == 1) {
    return -1;
  }
  chdir(pos);
  TARPATH = realloc(TARPATH, strlen(posTar) + 1);
  strcpy(TARPATH, posTar);

 return 1;
}


