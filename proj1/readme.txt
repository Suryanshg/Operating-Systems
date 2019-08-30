
This is a program that takes another command as an argument and executes that command. For instance, executing:
% ./doit wc foo.txt
would invoke the wc “word count” command with an argument of foo.txt, which will output the number of lines, words and bytes in the file “foo.txt” (which is assumed to exist in the current directory).  
Note the wc command will work on any file containing text. After execution of the specified command has completed, doit should display statistics that showsome of system resources the command used. In particular,doit should print:
1. the amount of CPU time used (both user and system time) (in milliseconds)
2. the elapsed “wall-clock” time for the command to execute (in milliseconds)
3. the number of times the process was preempted involuntarily (e.g. time slice expired,preemption by higher priority process)
4. the number of times the process gave up the CPU voluntarily (e.g. waiting for a resource)
5. the number of major page faults, which require disk I/O, and
6. the number of minor page faults, which could be satisfied without disk I/O

Shell Mode: It continuously prompts the user for an input if there are no arguments provided along with ./doit. It handles three basic commands:
1. exit
2. cd dir
3. set prompt = newprompt:

It will exit if more than 32 arguments are provided in a line of command.  
