tsh :  Projet de programmation système L3
==================

## Etape 1 : Build le docker

Pour créer l'image adatpée a notre projet, tapez dans le terminal

* `docker build -t projet .`

Maintenant que vous avez l'image, créez un container de l'image

* `docker run -it projet bash`

## Etape 3 : Instruction de compilation et d'exécution

Aller dans le dossier /home/projet et exécutez le Makefile

* `make`

Cela va créer 1 exécutable indépendant : **shell**

L'exécution de **shell** vous permet :

* D'avoir un shell classique, avec toutes les commandes UNIX
* De traiter les tarballs comme s'ils s'agissait de repertoires.

Les commandes suivantes sont implementees pour un tarball :

* `cd` : permet de naviguer dans un tarball
* `exit` : quitte le shell
* `pwd` : affiche le chemin du repertoire courant
* `mkdir` : creer un nouveau repertoire (ou plusieurs a la fois)
* `rmdir` : supprime un ou des repertoires
* `mv` : ?
* `cp` : copie un fichier
* `rm` : supprime un fichier ou un dossier avec l'option -r
* `ls` : affiche le contenu du ou des repertoires, l'option -l
* `cat` : affiche le contenu d'un fichier ou dossier
* `redirection`
* `|` : permet de faire des combinaisons de commandes

## Precision sur l'implementation du projet

Nous utilisons le vieux format GNU du tar le seul changement notable c'est qu'on a supprime le champs "version" et allouee une memoire de 8 bits au champs "magic" pour garder une taille de 512 bits par entete.
Donc le projet doit etre tester avec le bon format Tar.
