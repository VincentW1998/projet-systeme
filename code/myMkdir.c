#include "myMkdir.h"
#include "gestionnaire.h"
#include "tar.h"

struct posix_header newHeader(const char * path) {
  struct posix_header hd;
  struct stat st;
  uid_t uid = getuid();
  struct passwd * usr = getpwuid(uid);

  char * pathDos = "DossierVide"; // creer un dossier vide modele
  mkdir(pathDos, 0700);

  stat(pathDos, &st);

  struct group * grp = getgrgid(st.st_gid);

  memset(&hd, '\0', sizeof(struct posix_header)); //initialise le struct avec des '\0'
  // on remplit les champs du struct posix_header
  strncpy(hd.name, path, 100);
  snprintf(hd.mode, sizeof(hd.mode), "%07o", st.st_mode);
  snprintf(hd.uid, sizeof(hd.uid), "%07o", st.st_uid);
  snprintf(hd.gid, sizeof(hd.gid),"%07o", st.st_gid);
  snprintf(hd.size, sizeof(hd.size), "%011o",(int) st.st_size);
  snprintf(hd.mtime, sizeof(hd.mtime), "%011o",(int) st.st_mtime);
  hd.typeflag = '5';
  strncpy(hd.magic, TMAGIC, strlen(TMAGIC));
  strncpy(hd.version, TVERSION, strlen(TVERSION));
  strncpy(hd.uname, usr -> pw_name, 32);
  strncpy(hd.gname, grp -> gr_name, 32);
  memset(hd.linkname, '\0', sizeof(hd.linkname));
  memset(hd.devmajor, '\0', sizeof(hd.devmajor));
  memset(hd.devminor, '\0', sizeof(hd.devminor));
  memset(hd.prefix, '\0', sizeof(hd.prefix));
  memset(hd.junk, '\0', sizeof(hd.junk));


  set_checksum(&hd);

  rmdir(pathDos);

  return hd;
}

int my_mkdir (const char * path) {
    int n;
    if((n = mkdir(path, 0777) > -1)){
        return 0;
    }
    return -1;
}

