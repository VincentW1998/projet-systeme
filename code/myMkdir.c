#include "myMkdir.h"
#include "gestionnaire.h"
#include "check.h"
#include "tar.h"
#include "myCd.h"
#include "storeRestore.h"

/* create new posix_header of Empty directory */
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

int directoryShell(char * path) {
  int fd;
  fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    perror("Erruer ouverture fichier tar");
    return -1;
  }
  char blockVide[20 * BLOCKSIZE];
  memset(blockVide, '\0', 20 * BLOCKSIZE);
  write(fd, blockVide, 20 * BLOCKSIZE);
  close(fd);
  return 1;
}

/* loop for each command[i] (path) */
int mkdirTar(int nbOption,char ** command) {
  storePosition();

  for (int i = 1; i < nbOption; i++) {
    if(createRepo(command[i]) == -1) {
      char * msg = "mkdir: cannot create directory : File exists";
      displayError(msg);

    }
    restorePosition();
  }
 return 1;}


/* check which command we should use, cmd shell or cmd tar 
 * and check the tarfile, if the directory doesn't existe, 
 * it will be create */
int createRepo(char * path){
  char * pathMkdir = getLastToken(path);
  char * pathCd = pathWithoutLastToken(path, pathMkdir);
  if(whichCd(pathCd) == -1) {
    return -1;
  }


  // after Cd function if we are not in tar file
  if (TARPATH[0] == '\0') {
    //commandNoTar("mkdir", pathMkdir);
    return directoryShell(pathMkdir);
  }

  int n;
  // return tar file name for open function
  char * tarName = substringTar();

  // concatene path et TARPATH et rajoute un slash a la fin
  char * pathWithFolder = createPath(pathMkdir);

  // create new posix_header of emply repository and put in global variable
  newHd = newHeader(pathWithFolder);

  if((n = checkEntete(tarName, pathWithFolder)) == 1) {
    return -1;
  }
 return 1;
}


