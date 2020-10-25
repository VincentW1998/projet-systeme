# tsh

**Projet de programmation système L3**


**Prérequis**

**Etape 1 : Build le docker**

Pour créer l'image adatpée a notre projet, tapez dans le terminal

* `docker build -t projet .`

Maintenant que vous avez l'image, créez un container de l'image

* `docker run -it projet bash`

**Etape 3 : Instruction de compilation et d'exécution**

Aller dans le dossier /home/projet et exécutez le Makefile

* `make`

Cela va créer 4 exécutables indépendants : **shell**, **mon_cp**, **mon_mv**, **mon_rm**

L'exécution de **shell** vous permet :

* D'avoir un shell, avec toutes les commandes UNIX sauf les redirections, prends en compte les commandes avec UN pipe
* De rentrer, sortir, naviguer dans un tar avec la commande cd
* D'afficher sa position courante avec la commande pwd

L'exécution de **mon_cp**, **mon_mv**, **mon_rm** vous permet :
* De tester les commandes associées indépendamment de notre shell tsh