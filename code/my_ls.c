#include "my_ls.h"
#define blocksize 256
int typeFic(struct stat st){

  int n;
  if(S_ISREG(st.st_mode))
    n = write(1,"-",1);
  if(S_ISDIR(st.st_mode))
    n = write(1,"d",1);
  if(S_ISBLK(st.st_mode))
    n = write(1,"b",1);
  if(S_ISCHR(st.st_mode))
    n = write(1,"c",1);
  if(S_ISLNK(st.st_mode))
    n = write(1,"l",1);
  if(S_ISFIFO(st.st_mode))
    n = write(1,"p",1);
  if(S_ISSOCK(st.st_mode))
    n = write(1,"s",1);
  if(n==0) return -1;
  return 0;
}

int rights(struct stat st){
  char droits [blocksize];
    //droits usr
      if(st.st_mode & S_IRUSR) strcpy(droits,"r"); else strcpy(droits,"-");
      if(st.st_mode & S_IWUSR) strcat(droits,"w"); else strcat(droits,"-");
      if(st.st_mode & S_IXUSR) strcat(droits,"x"); else strcat(droits,"-");

    //droits groupe
      if(st.st_mode & S_IRGRP) strcat(droits,"r"); else strcat(droits,"-");
      if(st.st_mode & S_IWGRP) strcat(droits,"w"); else strcat(droits,"-");
      if(st.st_mode & S_IXGRP) strcat(droits,"x"); else strcat(droits,"-");

    //droits autres
      if(st.st_mode & S_IROTH) strcat(droits,"r"); else strcat(droits,"-");
      if(st.st_mode & S_IWOTH) strcat(droits,"w"); else strcat(droits,"-");
      if(st.st_mode & S_IXOTH) strcat(droits,"x  "); else strcat(droits,"-  ");

  if(write(1,droits,strlen(droits))<9)return -1;
  return 0;
}

int Nbrlink(struct stat st){
  char l[blocksize];
  sprintf(l,"%hi  ",st.st_nlink);
  if(write(1,l,strlen(l))==0)return -1;
  return 0;
}

int usrAndGrp(struct stat st){
  char s[blocksize];
  struct passwd * usr;
  usr = getpwuid(st.st_uid);
  strcpy(s,usr->pw_name);

  struct group * grp;
  grp = getgrgid(st.st_gid);
  strcat(s,"  ");
  strcat(s,grp->gr_name);
  strcat(s,"  ");
  if(write(1,s,strlen(s))==0)return -1;
  return 0;
}

int psize(struct stat st){
  char s[blocksize];
  sprintf(s,"%lld",st.st_size);
  strcat(s,"  ");
  if(write(1,s,strlen(s))==0)return -1;
  return 0;
}

int mtime(struct stat st){
  char *s;
  s = ctime(&st.st_mtime);
  if(write(1,s,strlen(s)-1)==0)return -1 ;
  write(1,"  ",2);
  return 0;
}



int ls(char buff[]){
  DIR * current;
  struct dirent *directory;
  struct stat st;
  char s[blocksize];
  char options[3][blocksize];
  int i=0;
  char * token = strtok(buff," \n");
  while((token = strtok(NULL," \n"))!=NULL){
    strcpy(options[i],token);
    i++;
  }
  printf("%d\n", i);
  if(i!=0 && strcmp(options[i-1],"-l")){
    chdir(options[i-1]);
  }
  current = opendir(".");
  while((directory = readdir(current)) > 0){
    // printf("%s\n", "read");
    if(directory->d_name[0]!='.'){
      if(i!=0 && !strcmp(options[0],"-l")){
        if(stat(directory->d_name,&st)) printf("%s\n","pb stat");
        if(typeFic(st))return -1;
        if(rights(st))return -1;
        if(Nbrlink(st))return -1;
        if(usrAndGrp(st))return -1;
        if(psize(st))return -1;
        if(mtime(st))return -1;
        write(1,directory->d_name,strlen(directory->d_name));
        write(1,"\n",1);
      }
      else{
        strcpy(s,directory->d_name);
        strcat(s,"\n");
        write(1,s,strlen(s));
      }
    }
  }
  // // write(1,"\n",1);
  // rewinddir(current);
  // free(options);
  // while(i!=0){
  //   free(options[i]);
  //   i--;
  // }
  closedir(current);

  return 0;
}
