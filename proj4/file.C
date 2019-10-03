/*
 * file.C
 *
 *  Created on: Oct 2, 2019
 *      Author: sgoyal
 */
#include <iostream>
using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#define BUFSIZE 1024

void printStats();
int numBadFiles=0, numDir=0, numRegFiles=0, numSpecFiles=0, bytesReg=0, numTextFiles=0, bytesText=0;

//typedef struct {
//    mode_t    st_mode;    /* protection */
//    off_t     st_size;    /* total size, in bytes */
//} statsInfo;

int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
	char fileName[100];
	int fdIn, cnt, i, statResult;
	struct stat statsInfo;
	//	if (argc < 2) {
	//		fdIn = 0;  /* just read from stdin */
	//	}

	while(1){

		cin.getline(fileName,100); // get the file's name
		if(fileName==NULL){
			break;
		}
		if ((fdIn = open(fileName, O_RDONLY)) < 0){ // EOF
			exit(1);
		}
		statResult=stat(fileName,&statsInfo);
		if(statResult<0){ // if bad file
			numBadFiles++;
		}
		else if(S_ISDIR(statsInfo.st_mode)){ // if directory
			numDir++;
		}
		else if(S_ISREG(statsInfo.st_mode)){ // if regular file
			numRegFiles++;
			bytesReg+=statsInfo.st_size;

			// check for text file
			while ((cnt = read(fdIn, buf, 1)) > 0) {
				//cout<<buf;
				//write(1, buf, cnt);

			}
			cout<<"Hello";

		}
		else{ // it's a special file
			numSpecFiles++;
		}

		if (fdIn > 0){
			close(fdIn);
		}






		//		if ((fdIn = open(fileName, O_RDONLY)) < 0) {
		//			//cerr << "file open\n";
		//			exit(1);
		//		}

		// copy input to stdout

	}

	printStats();
	return 0;
}

void printStats(){

}
