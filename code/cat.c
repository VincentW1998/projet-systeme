char pathname[BLOCKSIZE][BLOCKSIZE];
int i=0;
int rightPath(char * path){
  char * tok = strtok(path,"/\n");
  int x = 0;
  while(x!=i){
    if((token = strtok(NULL, "/\n"))==NULL) return -1;
    if(strcmp(pathname[x],token)) return -1; // path incorect
    x++;
  }
  if((token = strtok(NULL,"/\n")) == NULL) return 0;
  else return -1; // trop grand
}

int cat(char * path){
  int file,n;

  char * token = strtok(Tarpath, "/\n");
  if(isTar(token)){
    while((token = strtok(NULL,"/\n"))!=NULL){
      strcpy(pathname[i],token);
      i++;
    }
}
