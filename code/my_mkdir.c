include "my_mkdir.h"
#include "tar.h"

int newHeader(const char * path, struct * posix_header hd) {
  int n, n1;
  char * pathDos = "DossierVide"; // creer un dossier vide modele
  if(n = mkdir(pathDos, 0700) < 0){
    perror("mkdir");
    return -1;
  }
  struct stat st;

  if(stat(pathDos, &st)) {
    perror("stat");
    return -1;
  }
  uid_t uid = getuid();
  struct passwd * usr = getpwuid(uid);
  struct group * grp = getgrgid(st_gid);
  
  memset(hd, '\0', sizeof(struct posix_header)); //initialise le struct avec des '\0'
  // on remplit les champs du struct posix_header
  strncpy(hd_name, path, 100);
  snprintf(hd_mode, sizeof(hd_mode), "%07o", st_mode);
  snprintf(hd_uid, sizeof(hd_uid), "%07o", st_uid);
  snprintf(hd_gid, sizeof(hd_gid),"%07o", st_gid);
  snprintf(hd_size, sizeof(hd_size), "%011o", st_size);
  snprintf(hd_mtime, sizeof(hd_mtime), "%011o", st_mtime);
  hd_typeflag = '5';
  strncpy(hd_magic, TMAGIC, strlen(TMAGIC));
  strncpy(hd_version, TVERSION, strlen(TVERSION));
  strncpy(hd_uname, user -> pw_name, 32);
  strncpy(hd.gname, grp -> gr_name, 32);
  memeset(hd_linkname, '\0', sizeof(hd_linkname));
  memset(hd_devmajor, '\0', sizeof(hd_devmajor));
  memeset(hd_devminor, '\0', sizeof(hd_devminor));
  memeset(hd_prefix, '\0', sizeof(hd_prefix));
  memeset(hd_junk, '\0', sizeof(hd_junk));
  

  set_checksum(hd);

  if((n1 = rmdir(pathDos)) < 0) {
    perror("rmdir");
    return -1;
  } 


  return 1;
}

int my_mkdir (const char * path) {
    int n;
    if((n = mkdir(path, 0777) > -1)){
        return 0;
    }
    return -1;
}

