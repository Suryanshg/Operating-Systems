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
#include <sys/time.h>
#include <sys/resource.h>
#define BUFSIZE 1024

void printStats();
void* processFile(void*);
void processFile1(char file[]);

// Global Variables
int numBadFiles = 0, numDir = 0, numRegFiles = 0, numSpecFiles = 0,
		numTextFiles = 0;
long int bytesReg = 0, bytesText = 0;
sem_t semArr[5]; // 0-statsInfo, 1-Bad, 2-Dir, 3-Reg/Text, 4-Spec
int numThreadsRunning = 0; //counter
int oldestThread = 1; //tracking oldest thread
double startTime;


int main(int argc, char *argv[]) {
	char fileName[100];
	if (argc == 1) { // serial architecture version
		while (1) {
			cin.getline(fileName, 100); // get the file's name
			if (fileName == NULL || (strcmp(fileName, "\0") == 0)) { // if EOF, break
				break;
			}

			else {
				struct timeval s_time; //start time of the process
				gettimeofday(&s_time,NULL);
				startTime=(s_time.tv_sec) + (s_time.tv_usec * 0.000001);
				processFile1(fileName);
			}
		}
		printStats();
	} else if (argc > 1) { //multithreaded architecture version
		cout << "Multi-Threaded Version\n";
		if (strcmp(argv[1], "thread") != 0 || atoi(argv[2]) < 1
				|| atoi(argv[2]) > 15) { // if first arg is not "thread" or second thread isn't in range [1,15]
			cout << "Usage: ./file thread maxThreads[1-15]\n";
		} else { // everything fine
			int maxThreads = atoi(argv[2]);
			pthread_t threadArr[maxThreads];
			char fileArr[500][100];

			for (int i = 0; i < 5; i++) { // initialize the semaphores' array
				sem_init(&semArr[i], 0, 1);
			}

			int x = 0; // the total number of files
			while (1) {
				cin.getline(fileArr[x], 100); // get the file's name
				if (fileArr[x] == NULL || (strcmp(fileArr[x], "\0") == 0)) { // if EOF, break
					break;
				}
				x++;
			}

			struct timeval s_time; //start time of the process
			gettimeofday(&s_time,NULL);
			startTime=(s_time.tv_sec) + (s_time.tv_usec * 0.000001);

			int curFile = 0;
			while (1) {
				if (curFile == x) {
					cout << "DONE\n";
					break;
				} else {
					if (numThreadsRunning == maxThreads) {
						pthread_join(threadArr[oldestThread - 1], NULL);
						numThreadsRunning--;
					}
					pthread_create(&threadArr[oldestThread - 1], NULL,
							&processFile, (void*) fileArr[curFile]);
					numThreadsRunning++;
					curFile++;
					if (oldestThread == maxThreads) {
						oldestThread = 1;
					} else {
						oldestThread++;
					}

				}
			}

			for (int i = 0; i < numThreadsRunning; i++) { //join all threads after finishing
				pthread_join(threadArr[i], NULL);
			}
			printStats();
		}
	}
	return 0;

}

void printStats() {
	struct rusage usage;
	struct timeval e_time, utime, stime; // for Wall Clock time, user time and sys time respectively
	gettimeofday(&e_time,NULL);
	double endTime=(e_time.tv_sec)+(e_time.tv_usec * .000001);
	double timeElapsed= endTime-startTime; // elapsed wall-clock time
	getrusage(RUSAGE_SELF,&usage);
	utime= usage.ru_utime;
	stime= usage.ru_stime;
	double userTime = (utime.tv_sec) + (utime.tv_usec * .000001); // user CPU time
	double sysTime = (stime.tv_sec ) + (stime.tv_usec * .000001); // system CPU time
	cout << "Bad Files: " << numBadFiles << "\n";
	cout << "Directories: " << numDir << "\n";
	cout << "Regular Files: " << numRegFiles << "\n";
	cout << "Special Files: " << numSpecFiles << "\n";
	cout << "Regular File Bytes: " << bytesReg << "\n";
	cout << "Text Files: " << numTextFiles << "\n";
	cout << "Text File Bytes: " << bytesText << "\n";
	cout << "************* Time Statistics **************\n";
	cout << "Wall clock time: "<<timeElapsed<< " seconds\n";
	cout << "System Time: " <<sysTime<<" seconds\n";
	cout << "User Time: " <<userTime<<" seconds\n";
}

void* processFile(void* arg) { // for multi architecture version
	int cnt, statResult, fd;
	char* file = (char*) arg;
	struct stat statsInfo;
	bool isText;
	char buf[BUFSIZE];

	sem_wait(&semArr[0]);
	statResult = stat(file, &statsInfo); //! [0]
	sem_post(&semArr[0]);

	if (statResult < 0) { // if bad file   ! [1]

		sem_wait(&semArr[1]);
		numBadFiles++;
		sem_post(&semArr[1]);

	} else if (S_ISDIR(statsInfo.st_mode)) { // if directory    ! [2]

		sem_wait(&semArr[2]);
		numDir++;
		sem_post(&semArr[2]);

	} else if (S_ISREG(statsInfo.st_mode)) { // if regular file   ! [3]

		sem_wait(&semArr[3]);
		numRegFiles++;
		bytesReg += statsInfo.st_size;

		isText = true;
		if ((fd = open(file, O_RDONLY)) < 0) { // EOF
			cout << "Error opening file:" << file << "\n";
		} else {
			while ((cnt = read(fd, buf, 1)) > 0) { // check for text file
				if (!(isprint(buf[0])) && !(isspace(buf[0]))) {
					isText = false;
					break;
				}
			}
			if (isText) {

				numTextFiles++; //   ! [3]
				bytesText += statsInfo.st_size;

			}
		}
		if (fd > 0) { // close the file if it is open
			close(fd);
		}
		sem_post(&semArr[3]);
	} else { // it's a special file   ! [5]

		sem_wait(&semArr[4]);
		numSpecFiles++;
		sem_post(&semArr[4]);

	}

}

void processFile1(char file[]) { // for serial architecture version
	int cnt, statResult, fd;

	struct stat statsInfo;
	bool isText;
	char buf[BUFSIZE];


	statResult = stat(file, &statsInfo); //! [0]


	if (statResult < 0) { // if bad file   ! [1]


		numBadFiles++;


	} else if (S_ISDIR(statsInfo.st_mode)) { // if directory    ! [2]


		numDir++;


	} else if (S_ISREG(statsInfo.st_mode)) { // if regular file   ! [3]


		numRegFiles++;
		bytesReg += statsInfo.st_size;

		isText = true;
		if ((fd = open(file, O_RDONLY)) < 0) { // EOF
			cout << "Error opening file:" << file << "\n";
		} else {
			while ((cnt = read(fd, buf, 1)) > 0) { // check for text file
				if (!(isprint(buf[0])) && !(isspace(buf[0]))) {
					isText = false;
					break;
				}
			}
			if (isText) {

				numTextFiles++; //   ! [3]
				bytesText += statsInfo.st_size;

			}
		}
		if (fd > 0) { // close the file if it is open
			close(fd);
		}

	} else { // it's a special file   ! [5]


		numSpecFiles++;

	}

}






