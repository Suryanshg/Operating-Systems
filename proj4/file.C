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
#define BUFSIZE 1024

//typedef struct {
//    mode_t    st_mode;    /* protection */
//    off_t     st_size;    /* total size, in bytes */
//} statsInfo;

int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
	int fdIn, cnt, i;
	struct stat statsInfo;

//	if (argc < 2) {
//		fdIn = 0;  /* just read from stdin */
//	}

	while(1){
		char fileName[100];
		cin.getline(fileName,100);
		if ((fdIn = open(fileName, O_RDONLY)) < 0) {
			//cerr << "file open\n";
			exit(1);
		}

		// copy input to stdout
		while ((cnt = read(fdIn, buf, BUFSIZE)) > 0) {
			write(1, buf, cnt);
		}
		if (fdIn > 0)
			close(fdIn);


	}


	return 0;
}




