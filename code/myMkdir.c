#include "myMkdir.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"

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

int mkdirTar(int nbOption,char ** command) {

  for (int i = 1; i < nbOption; i++) {
    if(createRepo(command[i]) == -1)
      return -1;
  }

 return 1;
}

int createRepo(char * path){
  storePosition(); // store position
  //path for  Cd
  char * pathCd= subWithoutRepo(path);

  if(whichCd(pathCd) == -1)
    return -1;

  //path for mkdir
  char * pathMkdir = subWithRepo(path);

  // after Cd function if we are not in tar file
  if (TARPATH[0] == '\0') {
    //mkdirNoTar(pathMkdir);
    commandNoTar("mkdir", pathMkdir);
    restorePosition();
    return 1;
  }

  int fd, n;
  // return tar file name for open function
  char * tarName = substringTar();

  fd = open(tarName, O_RDWR); // on ouvre le fichier tar
  if (fd < 0){
    perror("open fichier Tar");
    return -1;
  }
  // concatene path et TARPATH et rajoute un slash a la fin
  char * pathWithFolder = createPath(pathMkdir);

  // create new posix_header of emply repository
  struct posix_header hd = newHeader(pathWithFolder);

  if((n = checkEntete2(tarName, pathWithFolder, &hd)) == 1) {
    return -1;
  }
  restorePosition();
 return 1;
}

/* use the exec mkdir */
/*
int mkdirNoTar(char * path){
  char * command[2];
  command[0] = malloc(strlen("mkdir") + 1);
  command[1] = malloc(strlen(path) + 1);
  strcpy(command[0], "mkdir");
  strcpy(command[1], path);
  execCommand(command);
  return 1;
}
*/
