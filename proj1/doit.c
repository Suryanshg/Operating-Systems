// This is a mini-linux terminal shell that handles some basic commands. It doesn't have all the features of a regular linux terminal. 


#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <sys/wait.h>

void printStatistics(int startTime){
	struct rusage usage;
	struct timeval e_time, utime, stime; //end time of the process, user CPU time used, system CPU time used respectively
	gettimeofday(&e_time,NULL);
	int endTime=(e_time.tv_sec * 1000)+(e_time.tv_usec * 1000);
	int timeElapsed= endTime-startTime; // elapsed wall-clock time
	getrusage(RUSAGE_CHILDREN,&usage);
	utime= usage.ru_utime;
	stime= usage.ru_stime;
	int userTime = (utime.tv_sec * 1000) + (utime.tv_usec * 1000); // user CPU time
	int sysTime= (stime.tv_sec * 1000) + (stime.tv_usec * 1000); // system CPU time
	long invContextSwitches= usage.ru_nivcsw; // involuntary context switches
	long vContextSwitches= usage.ru_nvcsw; // voluntary context switches
	long majorPageFaults= usage.ru_majflt; // num of major page faults
	long minorPageFaults= usage.ru_minflt; // num of minor page faults

	printf("The amount of user CPU time used: %d milliseconds\n", userTime);
	printf("The amount of system CPU time used: %d milliseconds\n", sysTime);
	printf("The elapsed \"wall-clock\" time for the command to execute: %d milliseconds\n",timeElapsed);
	printf("The number of times the process was preempted involuntarily: %ld\n",invContextSwitches);
	printf("The number of times the process gave up the CPU voluntarily: %ld\n",vContextSwitches);
	printf("The number of major page faults: %ld\n",majorPageFaults);
	printf("The number of minor page faults: %ld\n",minorPageFaults);

}

void  execute(char **argv)
{
	int pid;
	struct timeval s_time; //start time of the process
	gettimeofday(&s_time,NULL);
	int startTime=(s_time.tv_sec * 1000) + (s_time.tv_usec * 1000);

	if ((pid = fork()) < 0) { // forking fails
		fprintf(stderr, "Fork error\n");
		exit(1);
	}
	else if (pid == 0) {     // child process
		if (execvp(argv[0], argv) < 0) {
			fprintf(stderr,"Execvp error\n");
			exit(1);
		}
	}
	else { // parent process
		wait(0); // wait for the child to finish
		printStatistics(startTime);
	}
}

void  main(int argc, char* argv[])
{
	int i;
	char **newargv=(char**)malloc(32 * sizeof(char*));// if the user supplies the commands along with the execution of ./doit
	if(argc>1){
		for(i=1;i<argc;i++){ //copy the commands to be executed in a new array
			newargv[i-1]=argv[i];
		}
		newargv[argc-1]='\0'; // make it null terminated
		if(strcmp(newargv[0],"exit")==0){
			exit(0);
		}
		else{
			execute(newargv);
		}
	}
	else{// if the user doesn't supply commands, prompts the shell

		char prompt[128]="==>";
		while(1){


			char* line;
			char* token;
			line=(char*)malloc(128*sizeof(char*));

			printf("%s",prompt);
			fgets(line,128,stdin);
			int j=0;
			token=strtok(line," ");
			while(token!=NULL){
				char* pos;
				if((pos=strchr(token,'\n'))!=NULL){ // remove the end of line character because of using enter
					*pos='\0';
				}
				newargv[j]=token;

				token=strtok(NULL, " ");
				j++;
			}
			newargv[j]='\0';
			if((strcmp(newargv[0],"exit")==0) || (j>32)){ // if the command is exit or the number of args is greater than 32
				exit(0);
			}
			else if((strcmp(newargv[0],"cd")==0) && (strcmp(newargv[1],"NULL")!=0)){ // if the command is cd
				if(chdir(newargv[1])<0){
					printf("Error: No directory exists.\n");
				}
			}
			else if((strcmp(newargv[0],"set")==0) && (strcmp(newargv[1],"prompt")==0) && (strcmp(newargv[2],"=")==0) && (strcmp(newargv[3],"NULL")!=0)){ // if the command is set prompt =
				strncpy(prompt,newargv[3],sizeof(newargv[3]));
			}
			else{
				execute(newargv);
			}
		}


	}


}

