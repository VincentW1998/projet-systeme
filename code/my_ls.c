#include "my_ls.h"
#define blocksize 500
char s [blocksize];
int typeFic(struct stat st){

  int n;
  if(S_ISREG(st.st_mode))
    strcpy(s,"-");
    // n = write(1,"-",1);
  if(S_ISDIR(st.st_mode))
    strcpy(s,"d");
    // n = write(1,"d",1);
  if(S_ISBLK(st.st_mode))
    strcpy(s,"b");
    // n = write(1,"b",1);
  if(S_ISCHR(st.st_mode))
    strcpy(s,"c");
    // n = write(1,"c",1);
  if(S_ISLNK(st.st_mode))
    strcpy(s,"l");
    // n = write(1,"l",1);
  if(S_ISFIFO(st.st_mode))
    strcpy(s,"p");
    // n = write(1,"p",1);
  if(S_ISSOCK(st.st_mode))
    strcpy(s,"s");
    // n = write(1,"s",1);
  if(!strlen(s)) return -1;
  return 0;
}

int rights(struct stat st){

    //droits usr
      if(st.st_mode & S_IRUSR) strcat(s,"r"); else strcpy(s,"-");
      if(st.st_mode & S_IWUSR) strcat(s,"w"); else strcat(s,"-");
      if(st.st_mode & S_IXUSR) strcat(s,"x"); else strcat(s,"-");

    //droits groupe
      if(st.st_mode & S_IRGRP) strcat(s,"r"); else strcat(s,"-");
      if(st.st_mode & S_IWGRP) strcat(s,"w"); else strcat(s,"-");
      if(st.st_mode & S_IXGRP) strcat(s,"x"); else strcat(s,"-");

    //droits autres
      if(st.st_mode & S_IROTH) strcat(s,"r"); else strcat(s,"-");
      if(st.st_mode & S_IWOTH) strcat(s,"w"); else strcat(s,"-");
      if(st.st_mode & S_IXOTH) strcat(s,"x  "); else strcat(s,"-  ");

  // if(write(1,droits,strlen(droits))<9)return -1;
  return 0;
}

int Nbrlink(struct stat st){
  char l[blocksize];
  sprintf(l,"%hi  ",st.st_nlink);

  strcat(s, l);
  strcat(s,"  ");
  // if(write(1,l,strlen(l))==0)return -1;
  return 0;
}

int usrAndGrp(struct stat st){
  // char s[blocksize];
  struct passwd * usr;
  usr = getpwuid(st.st_uid);
  // strcpy(s,usr->pw_name);
  strcat(s,usr->pw_name);
  struct group * grp;
  grp = getgrgid(st.st_gid);
  strcat(s,"  ");
  strcat(s,grp->gr_name);
  strcat(s,"  ");
  // if(write(1,s,strlen(s))==0)return -1;
  return 0;
}

int psize(struct stat st){
  char l[blocksize];
  sprintf(l,"%lld",st.st_size);
  strcat(s,l);
  strcat(s,"  ");
  // if(write(1,s,strlen(s))==0)return -1;
  return 0;
}

int mtime(struct stat st){
  // char *s;
  strncat(s,ctime(&st.st_mtime),strlen(ctime(&st.st_mtime))-1);
  strcat(s,"  ");
  // if(write(1,s,strlen(s)-1)==0)return -1 ;
  // write(1,"  ",2);
  return 0;
}



int ls(char buff[]){
  DIR * current;
  struct dirent *directory;
  struct stat st;
  char * currpath = getcwd(NULL,0);
  // char s[blocksize];
  char options[3][blocksize];
  int i=1;
  char * token = strtok(buff," ");
  strcpy(options[0],token);
  if(strcmp(options[0],"ls"))return -1;
  while((token = strtok(NULL," \n"))!=NULL){
    strcpy(options[i],token);
    i++;
  }
  if(i>0 && strcmp(options[i-1],"-l")){
    if(chdir(options[i-1])){
     return -1;
    }
  }
  current = opendir(".");
  while((directory = readdir(current)) > 0){
    if(directory->d_name[0]!='.'){
      if(i>0 && !strcmp(options[1],"-l")){
        if(stat(directory->d_name,&st)) printf("%s\n","pb stat");
        if(typeFic(st))return -1;
        if(rights(st))return -1;
        if(Nbrlink(st))return -1;
        if(usrAndGrp(st))return -1;
        if(psize(st))return -1;
        if(mtime(st))return -1;
        // write(1,directory->d_name,strlen(directory->d_name));
        strcat(s,directory->d_name);
        strcat(s,"\n");
        write(1,s,strlen(s));
        // write(1,"\n",1);
      }
      else{
        strcpy(s,directory->d_name);
        strcat(s,"\n");
        write(1,s,strlen(s));
      }
    }
  }

  closedir(current);
  chdir(currpath);
  return 0;
}
