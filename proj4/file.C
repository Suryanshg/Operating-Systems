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
#include <cctype>
#include <cstring>
#include <pthread.h>
#define BUFSIZE 1024

void printStats();
void processFile(char file[],struct stat statsInfo, int fd);
int numBadFiles=0, numDir=0, numRegFiles=0, numSpecFiles=0, numTextFiles=0;
long int bytesReg=0, bytesText=0;


int main(int argc, char *argv[])
{
	int fdIn;
	int  cnt, statResult;
	bool isText;
	char buf[BUFSIZE];
	char fileName[100];
	struct stat statsInfo;

	if(argc==1){// serial architecture version
		while(1){

			cin.getline(fileName,100); // get the file's name


			if(fileName==NULL || (strcmp(fileName,"\0")==0)){ // if EOF, break
				cout<<"EOF on "<<fileName<<"\n";
				break;
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
				isText=true;
				if ((fdIn = open(fileName, O_RDONLY)) < 0){ // EOF
					cout<<"Error opening file:"<<fileName<<"\n";
				}
				while ((cnt = read(fdIn, buf, 1)) > 0) { // check for text file
					if(!(isprint(buf[0])) && !(isspace(buf[0]))){
						isText=false;
						break;
					}
				}
				if(isText){
					numTextFiles++;
					bytesText+=statsInfo.st_size;
				}

			}
			else{ // it's a special file
				numSpecFiles++;
			}

			if (fdIn > 0){
				close(fdIn);
			}
		}

		printStats();

	}
	else if(argc>1){
		cout<<"MultiThreaded Version\n";
	}
	return 0;

}

void printStats(){
	cout<<"Bad Files: "<<numBadFiles<<"\n";
	cout<<"Directories: "<<numDir<<"\n";
	cout<<"Regular Files: "<<numRegFiles<<"\n";
	cout<<"Special Files: "<<numSpecFiles<<"\n";
	cout<<"Regular File Bytes: "<<bytesReg<<"\n";
	cout<<"Text Files: "<<numTextFiles<<"\n";
	cout<<"Text File Bytes: "<<bytesText<<"\n";
}

void processFile(char file[], struct stat statsInfo, int fd){
	int  cnt, statResult;
	bool isText;
	char buf[BUFSIZE];

	statResult=stat(file,&statsInfo);
	if(statResult<0){ // if bad file
		numBadFiles++;
	}
	else if(S_ISDIR(statsInfo.st_mode)){ // if directory
		numDir++;
	}
	else if(S_ISREG(statsInfo.st_mode)){ // if regular file
		numRegFiles++;
		bytesReg+=statsInfo.st_size;
		isText=true;
		while ((cnt = read(fd, buf, 1)) > 0) { // check for text file
			if(!(isprint(buf[0])) && !(isspace(buf[0]))){
				isText=false;
				break;
			}
		}
		if(isText){
			numTextFiles++;
			bytesText+=statsInfo.st_size;
		}

	}
	else{ // it's a special file
		numSpecFiles++;
	}

	if (fd > 0){
		close(fd);
	}
}

