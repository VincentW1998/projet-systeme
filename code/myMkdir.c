#include "myMkdir.h"
#include "gestionnaire.h"
#include "tar.h"

struct posix_header newHeader(const char * path) {
  struct posix_header hd;
  struct stat st;
  uid_t uid = getuid();
  struct passwd * usr = getpwuid(uid);

  char * pathDos = "DossierVide"; 
  mkdir(pathDos, 0700); // creer un dossier vide model

  stat(pathDos, &st);

  struct group * grp = getgrgid(st.st_gid);

  memset(&hd, '\0', sizeof(struct posix_header)); //initialise le struct avec des '\0'
  // on remplit les champs du struct posix_header
  strncpy(hd.name, path, 100);
  snprintf(hd.mode, sizeof(hd.mode), "%07o", st.st_mode); //mode
  snprintf(hd.uid, sizeof(hd.uid), "%07o", st.st_uid); // uid
  snprintf(hd.gid, sizeof(hd.gid),"%07o", st.st_gid); // gid
  snprintf(hd.size, sizeof(hd.size), "%011o",(int) st.st_size); // size
  snprintf(hd.mtime, sizeof(hd.mtime), "%011o",(int) st.st_mtime); // mtime
  hd.typeflag = '5'; // typeflag
  strncpy(hd.magic, TMAGIC, strlen(TMAGIC)); // magic
  strncpy(hd.version, TVERSION, strlen(TVERSION)); // version
  strncpy(hd.uname, usr -> pw_name, 32); // uname
  strncpy(hd.gname, grp -> gr_name, 32); // gname
  memset(hd.linkname, '\0', sizeof(hd.linkname)); // linkname
  memset(hd.devmajor, '\0', sizeof(hd.devmajor)); // devmajor
  memset(hd.devminor, '\0', sizeof(hd.devminor)); // devminor
  memset(hd.prefix, '\0', sizeof(hd.prefix)); // prefix
  memset(hd.junk, '\0', sizeof(hd.junk)); // junk


  set_checksum(&hd); // checksum

  rmdir(pathDos); // on supprime le dossier model

  return hd;
}

int mkdirTar(const char * pathTar, const char * path) {
  int fd;
  fd = open(pathTar, O_WRONLY); // on ouvre le fichier tar
  if (fd < 0){
    perror("open fichier Tar");
    return -1;
  }
  lseek(fd, -1024, SEEK_END); // on se positionne au debut de l'avant dernier block de fin 

  struct posix_header hd = newHeader(path); // on creer le nouvel entete
  write(fd, &hd, BLOCKSIZE); // on ecrit l'entete dans le fichier tar
  lseek(fd, 0, SEEK_END); // on se place a la fin du fichier 
  char blockEnd[BLOCKSIZE];
  memset(blockEnd, 0, BLOCKSIZE); // on creer un block de 512 octets rempli de '\0'
  write(fd, blockEnd, BLOCKSIZE); // on rajoute ce block a la fin du fichier
  close(fd);
  return 1;
}

int my_mkdir (const char * path) {
    int n;
    if((n = mkdir(path, 0777) > -1)){
        return 0;
    }
    return -1;
}

