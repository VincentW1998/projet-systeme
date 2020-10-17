#include "my_ls.h"
#define blocksize 500
char s [blocksize];

int typeFic(struct stat st){

  int n;
  if(S_ISREG(st.st_mode))
    strcpy(s,"-");

  if(S_ISDIR(st.st_mode))
    strcpy(s,"d");

  if(S_ISBLK(st.st_mode))
    strcpy(s,"b");

  if(S_ISCHR(st.st_mode))
    strcpy(s,"c");

  if(S_ISLNK(st.st_mode))
    strcpy(s,"l");

  if(S_ISFIFO(st.st_mode))
    strcpy(s,"p");

  if(S_ISSOCK(st.st_mode))
    strcpy(s,"s");

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

  return 0;
}

int Nbrlink(struct stat st){
  char l[blocksize];
  sprintf(l,"%hi  ",st.st_nlink);
  strcat(s, l);
  strcat(s,"  ");
  return 0;
}

int usrAndGrp(struct stat st){
  struct passwd * usr;
  usr = getpwuid(st.st_uid);
  strcat(s,usr->pw_name);
  struct group * grp;
  grp = getgrgid(st.st_gid);
  strcat(s,"  ");
  strcat(s,grp->gr_name);
  strcat(s,"  ");
  return 0;
}

int psize(struct stat st){
  char l[blocksize];
  sprintf(l,"%lld",st.st_size);
  strcat(s,l);
  strcat(s,"  ");
  return 0;
}

int mtime(struct stat st){
  strncat(s,ctime(&st.st_mtime),strlen(ctime(&st.st_mtime))-1);
  strcat(s,"  ");
  return 0;
}



int ls(char *buff){
  // printf("BUFFER = %s\n",buff );
  DIR * current;
  struct dirent *directory;
  struct stat st;
  char * currpath = getcwd(NULL,0);
  char options[3][blocksize];
  int i=1;
  char * token = strtok(buff," ");
  strcpy(options[0],token);
  while((token = strtok(NULL," \n"))!=NULL){//separe la ligne de commande recu en plusieurs morceau et les stock dans options
    strcpy(options[i],token);
    i++;
    // printf("opt = %sS\n", options[i-1]);
    if(strcmp(options[i-1],"-l"))break;
  }
  printf("i = %d\n", i);
  if((i==1 && strncmp(options[0],"ls\n",strlen("ls\n")))|| (i>1 && strcmp(options[0],"ls"))){//check si le ls appels
    printf("%s\n", "1");
    perror("error:  ");
    return -1;
  }
  if(i>1 && strcmp(options[i-1],"-l")){
    if(chdir(options[i-1])){
      printf("%s\n", "2");

      chdir(currpath);
      perror("error:  ");
     return -1;
    }
  }
  current = opendir(".");
  while((directory = readdir(current)) > 0){
    if(directory->d_name[0]!='.'){
      if(i>1 && !strcmp(options[1],"-l")){
        if(stat(directory->d_name,&st)) printf("%s\n","pb stat");
        typeFic(st);
        rights(st);
        Nbrlink(st);
        usrAndGrp(st);
        psize(st);
        mtime(st);
        strcat(s,directory->d_name);
        strcat(s,"\n");
        write(1,s,strlen(s));
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
