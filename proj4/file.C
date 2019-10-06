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
#include <semaphore.h>
#define BUFSIZE 1024

void printStats();
void processFile(char file[]);

// Global Variables
int numBadFiles=0, numDir=0, numRegFiles=0, numSpecFiles=0, numTextFiles=0;
long int bytesReg=0, bytesText=0;
sem_t semArr[6]; // 0-statsInfo, 1-Bad, 2-Dir, 3-Reg, 4-Text, 5-Spec
int numThreads=0;



int main(int argc, char *argv[])
{
	char fileName[100];
	if(argc==1){// serial architecture version
		while(1){
			cin.getline(fileName,100); // get the file's name
			if(fileName==NULL || (strcmp(fileName,"\0")==0)){ // if EOF, break
				break;
			}

			else{
				processFile(fileName);
			}
		}
		printStats();
	}
	else if(argc>1){ //multithreaded architecture version
		cout<<"Multi-Threaded Version\n";
		if(strcmp(argv[1],"thread")!=0 || atoi(argv[2])<1 || atoi(argv[2])>15){ // if first arg is not "thread" or second thread isn't in range [1,15]
			cout<<"Usage: ./file thread maxThreads[1-15]\n";
		}
		else{ // everything fine
			int maxThreads=atoi(argv[2]);
			pthread_t threadArr[maxThreads];

			for(int i=0;i<6;i++){ // initialize the semaphores' array
				sem_init(&semArr[i],0,1);
			}

		}
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

void processFile(char file[]){
	int  cnt, statResult, fd;
	struct stat statsInfo;
	bool isText;
	char buf[BUFSIZE];
	statResult=stat(file,&statsInfo); //! [0]
	if(statResult<0){ // if bad file   ! [1]
		numBadFiles++;
	}
	else if(S_ISDIR(statsInfo.st_mode)){ // if directory    ! [2]
		numDir++;
	}
	else if(S_ISREG(statsInfo.st_mode)){ // if regular file   ! [3]
		numRegFiles++;
		bytesReg+=statsInfo.st_size;
		isText=true;
		if ((fd = open(file, O_RDONLY)) < 0){ // EOF
			cout<<"Error opening file:"<<file<<"\n";
		}
		else{
			while ((cnt = read(fd, buf, 1)) > 0) { // check for text file
				if(!(isprint(buf[0])) && !(isspace(buf[0]))){
					isText=false;
					break;
				}
			}
			if(isText){
				numTextFiles++; //   ! [4]
				bytesText+=statsInfo.st_size;
			}
		}
	}
	else{ // it's a special file   ! [5]
		numSpecFiles++;
	}
	if (fd > 0){
		close(fd);
	}
}

