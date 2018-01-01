#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//#define PATH "/home/saharsh/UTAustin/December/DistributedSystems/"
#define PATHSIZE 128

int main(int argc, char const *argv[])
{
	int num_proc = 6;
	int pid = (int) getpid();

    const char *dirPath = getenv("DistributedSystems");
    printf("dirPath is %s\n", dirPath);

    char const *testBin = argv[1];
    char binPath[PATHSIZE];
    memset(binPath, '\0', sizeof(binPath));
    strcat(binPath, dirPath);
    strcat(binPath, "demo");
    strcat(binPath, "/");
    strcat(binPath, testBin);
    strcat(binPath, "/");
    strcat(binPath, testBin);
    printf("Binary path is %s\n", binPath);

    char stdfile[PATHSIZE];
    memset(stdfile, '\0', sizeof(stdfile));
    strcat(stdfile, dirPath);
    strcat(stdfile, "demo");
    strcat(stdfile, "/");
    strcat(stdfile, testBin);
    strcat(stdfile, "/");
    strcat(stdfile, "proc");
    printf("Logs stored at %s\n", stdfile);

	for(int proc_iter = 0; proc_iter < num_proc; proc_iter++){
		pid = fork();

		if(pid==0){

			printf("Forked into child process %d\n", (int) getpid());
			char *args[4];

			char num_proc_str[2];
			sprintf(num_proc_str, "%d", num_proc);

			char proc_iter_str[2];
			sprintf(proc_iter_str, "%d", proc_iter);

			//args[0] = "/home/saharsh/UTAustin/December/DS/leadertest";        
            args[0] = binPath;
			args[1] = num_proc_str;
			args[2] = proc_iter_str;
    		args[3] = NULL;

    		char file[256];
    		//strcat(file, "/home/saharsh/UTAustin/December/DS/proc");
            strcat(file, stdfile);
    		strcat(file, proc_iter_str);
    		strcat(file, ".out");
    		printf("%s\n", file);
    		int fd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    		printf("FD opened is %d on process %d\n", fd, (int) getpid());

    		for(auto& x: args){
    			printf("Process %d::%s\n", (int) getpid(), x);
    		}

    		dup2(fd, 1);   // make stdout go to file
    		dup2(fd, 2);   // make stderr go to file - you may choose to not do this
                   			// or perhaps send stderr to another file

    		close(fd);     // fd no longer needed - the dup'ed handles are sufficient

    		setbuf(stdout, NULL);

    		execv( args[0], args );
    		while(1){}
		}
	}
	return 0;
}