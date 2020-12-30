#include "redirection.h"
#include "gestionnaire.h"
#include "storeRestore.h"
#include "myCd.h"
#include "check.h"

char * tarRedirectedDestination = "";
char * fileToBeDeleted = "";

/* check if the path(last token) is a directory
 should be used with cd to get to path without last token */
int isDirectory(char * lastToken){
	struct stat st;
	if(TARPATH[0] == '\0' )//check if stat has worked and if the file is a directory
		return (stat(lastToken, &st) != -1) && (S_ISDIR(st.st_mode));
	else return lastToken[strlen(lastToken)-1] == '/';
}

void setFileToBeDeleted(){
	char * pos = getcwd(NULL, 0);
	fileToBeDeleted = malloc(strlen(pos) + strlen("fileToBeDeleted") + 1);
	strcpy(fileToBeDeleted,pos);
	strcat(fileToBeDeleted,"/fileToBeDeleted");
}

//store Asbolute tarDestination if the redirection happened in a tar
void storeTarDestination(char * lastToken){
	char * pwd = getcwd(NULL, 0);
	tarRedirectedDestination = malloc(strlen(pwd) + strlen(TARPATH) + strlen(lastToken) + 3);
	strcpy(tarRedirectedDestination, pwd);
	strcat(tarRedirectedDestination, "/");
	strcat(tarRedirectedDestination, TARPATH);
	strcat(tarRedirectedDestination, "/");
	strcat(tarRedirectedDestination, lastToken);
}

// empty tarRedirectedDestination
void flush(){
	fileToBeDeleted = malloc(1);
	fileToBeDeleted[0] = '\0';
	tarRedirectedDestination = malloc(1);
	tarRedirectedDestination[0] = '\0';
	fpipe[0] = -1;
	fpipe[1] = -1;
}

//stop the redirection to file[1] and flushes
void stopRedirection(){
	if(fpipe[1] != -1)
		dup2(saveDup,fpipe[0]); //redirect the flow back to it's origin
	if(tarRedirectedDestination[0] != '\0')
		//		moveToTarDest();
		flush();
}

//redirection dup2 si hors du tar
int redirect(char * lastToken){
	if((fpipe[1] = open(lastToken, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
		perror("redirect");
		restorePosition();
		return -1;
	}
	dup2(fpipe[1], fpipe[0]); //filePipe[0] -> filePipe[1]
	close(fpipe[1]);
	printf("%d ",fpipe[0]);
	restorePosition();
	return 1;
}


/*redirection tar : create a file outside tarball to receive the desired output
 the do mv -> tar
 */
int redirectTar(char * lastToken){
	storeTarDestination(lastToken);
	setFileToBeDeleted();
	if(redirect("fileToBeDeleted") == -1){
		flush();
		return -1;
	}
	return 1;
}

int redirection(char * path){
	flush();
	storePosition();
	fpipe[0] = 1;
	saveDup = dup(fpipe[0]);
	char * lastToken = getLastToken(path);
	//a decommenter
	if(whichCd(pathWithoutLastToken(path,lastToken)) == -1) return -1;
	if(isDirectory(lastToken) == 1) {//check if lastToken is a directory
		restorePosition();
		return -1;
	}
	if(TARPATH[0] == '\0') return redirect(lastToken);
	return redirectTar(lastToken);
}


//the function to be called in Gestionnaire
//returns the position where the first > is encountered
int nbOptionRedirect(int nbOption, char ** cmd){
	for(int i=0;i<nbOption;i++){
		if(strcmp(cmd[i],">")==0){
//			printf("REDIRECTION %s \n",cmd[i-1]);
			printf("Redirections %d\n",i);
			redirection(cmd[i+1]);
			return i;
		}
	}
	return nbOption;
}



