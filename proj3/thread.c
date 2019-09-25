/* thread.c */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

/* gcc -o thread thread.c -lpthread */

#define MAXTHREAD 10
#define REQUEST 1
#define REPLY 2

struct msg {
	int iFrom;  /*who sent the message (0 .. number-of-threads)*/
	int value;  /*its value*/
	int cnt;    /*count of operations (not needed by all msgs)*/
	int tot;    /*total time (not needed by all msgs)*/
};


void* adder(void*);
void SendMsg(int iTo, struct msg *pMsg);
void RecvMsg(int iRecv, struct msg *pMsg);

sem_t* psemArr;
sem_t* csemArr;
pthread_t *threadArr;
struct msg* mailBoxes;


int main(int argc, char* argv[])
{
	int i, numThreads, inputVal, threadIndex;
	char input[10];
	char* token;
	char* args[3];
	char* ptr;
	struct msg msgToSend;

	if (argc < 2) {
		printf("The number of arguments should be 2\n");
		exit(1);
	}

	numThreads = atoi(argv[1]);



	//	if (numThreads > MAXTHREAD) {
	//		printf("Number of threads entered were too high!\n");
	//		numThreads = MAXTHREAD;
	//	}
	//
	//	if (targetVal < 1) {
	//		printf("Argument 2 too small, defaulting to 100\n");
	//		targetVal = 100;
	//	}

	psemArr=(sem_t*)malloc((numThreads+1)*sizeof(sem_t));// array of producer semaphores
	csemArr=(sem_t*)malloc((numThreads+1)*sizeof(sem_t));// array of consumer semaphores
	threadArr=(pthread_t*)malloc(numThreads * sizeof(pthread_t));// array of threads
	mailBoxes=(struct msg*)malloc((numThreads+1)*sizeof(struct msg)); // array of mailboxes

	for (i = 0; i <= numThreads; i++) { // create the semaphore arrays
		sem_init(&psemArr[i], 0, 1);
		sem_init(&csemArr[i], 0, 0);
	}

	for (i = 0; i < numThreads; i++) { // create threads
		if (pthread_create(&threadArr[i+1], NULL, &adder,(void *)(intptr_t)(i + 1))) {
			printf("Thread Creation error for thread %d\n", i + 1);
		}
	}


	int size=0;
	while(1){ // to check if there is any input error
		ptr=fgets(input,10,stdin);
		size=0;
		token=strtok(input," ");
		while(token!=NULL){
			char* pos;
			if((pos=strchr(token,'\n'))!=NULL){ // remove the end of line character because of using enter
				*pos='\0';
			}
			args[size]=token;
			token=strtok(NULL, " ");
			size++;
		}

		inputVal=atoi(args[0]);
		threadIndex=atoi(args[1]);
		//printf("size:%d\n",size);
		if(size!=2 || inputVal<0 || ptr==NULL){// send termination message, and receive messages

			for(i=1;i<=numThreads;i++){// send termination message to each thread
				msgToSend.iFrom=0;
				msgToSend.cnt=1;
				msgToSend.tot=0;
				msgToSend.value=-1;
				SendMsg(i,&msgToSend);
			}
			struct msg msg;
			i=0;
			while(i<numThreads){
				RecvMsg(0,&msg);
				printf("The result from thread %d is %d from %d operations during %d secs.\n",msg.iFrom,msg.value,msg.cnt,msg.tot);
				i++;
			}
			break;
		}

		msgToSend.iFrom=0;
		msgToSend.cnt=1;
		msgToSend.tot=0;
		msgToSend.value=inputVal;
		SendMsg(threadIndex,&msgToSend);

	}

}

void SendMsg(int iTo, struct msg *pMsg){
	sem_wait(&psemArr[iTo]);

	mailBoxes[iTo] = *pMsg;

	sem_post(&csemArr[iTo]);
}

void RecvMsg(int iRecv, struct msg *pMsg){

	sem_wait(&csemArr[iRecv]);

	*pMsg = mailBoxes[iRecv];

	sem_post(&psemArr[iRecv]);


}


void* adder(void* arg){
	int result=0;
	int cnt=0;
	long int stime=0;
	time(&stime);
	long int ftime=0;
	struct msg msgToRecv;
	int index;
	index = (int)(intptr_t)arg;
	printf("index: %d\n",index);

	while(1){
		RecvMsg(index,&msgToRecv);

		if(msgToRecv.value==-1){
			struct msg msg;
			msg.iFrom=index;
			msg.value=result;
			msg.cnt=cnt;
			time(&ftime);
			msg.tot=(ftime - stime); // needs to be changed!!!

			SendMsg(0,&msg);

			break;
		}

		result+=msgToRecv.value;
		cnt+=msgToRecv.cnt;
		sleep(1);


		//printf("In thread %d, val: %d and cnt: %d\n",index,result,cnt);

	}
//	printf("The result from thread %d is %d from %d operations.",index,result,msgToRecv.cnt);
}

