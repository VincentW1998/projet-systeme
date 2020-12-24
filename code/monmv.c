#include "moncp.c"

int mvintratar(int fichier, char *dosarc, char *arc, char *cible, char *dest){
  int a = cpfichier_intratar(fichier,cible,dest);
  if(a!=2){
    rmfichier_tar(fichier,dosarc,arc,cible);
  }
}

int mv_intertar
(int fichier1, int fichier2,char *dosarc1, char *arc1, char *dosarc2, char *arc2, char *fic, char *c){
  cpfichier_intertar(fichier1, fichier2, dosarc2, arc2, fic, c);
  rmfichier_tar(fichier1, dosarc1, arc1,fic);
}

int mvdos(int fichier1, int fichier2,char *dosarc1, char *arc1, char *dosarc2, char *arc2, char *fic, char *c){
  cp_r_intertar(fichier1,fichier2,dosarc2,arc2,fic,c);
  rm_r_tar(fichier1,dosarc1,arc1,fic);
}

int main(){
  int f = open("a.tar",O_RDWR);
  int g = open("b.tar",O_RDONLY);
  if(f<=0){
    perror("( f )");
  }

  if (g<=0){
    perror("( g )");
  }
  mvdos(g,f,"./","./b.tar","./","./a.tar","b/","a/");
}

