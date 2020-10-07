#include "my_ls.h"

int ls(char buff[], DIR * current){
  struct dirent *directory;
  if(!current)  return -1;

  char * token = strtok(buff," ");//decoupe buff a partir de " "
  token = strtok(NULL," ");
  while((directory = readdir(current))!=NULL) //affecte a directory un des fichiers de current
    if(token != NULL && !strcmp(token,"-a\n"))//check si l'usr utilise l'option -a de ls
      write(1,strcat(directory->d_name,"  "),strlen(directory->d_name)+1);//ecrit dans la sortie standard le nom du fichier
    else
      if(!strncmp(buff,"ls\n",strlen("ls\n")) && directory->d_name[0]!='.')//omet les fichiers commencant par un .
        write(1,strcat(directory->d_name,"  "),strlen(directory->d_name)+1);
  write(1,"\n",1);//saut de ligne
  rewinddir(current); //remet le pointeur dans current au debut du repertoire
  return 0;
}
