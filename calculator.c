#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLEN 1000

char operators[100];
int fds[100][2];
int operatorCount=0;
int numPipes=0;

void child(int index) {
	//CODE HERE!
	    close(0);
        if(!index) {
            dup(fds[0][0]);
        }
        else {
            dup(fds[2*index][0]);
        }
        close(3);

        dup(fds[index*2+1][0]);
        close(1);
        dup(fds[index*2+2][1]);
       
	    //optional cleanup same to mysum
        for(int i = 0;i < 2*operatorCount;i++)
        {
            close(fds[i][0]);
            close(fds[i][1]);
        }

        //execl and check for operators
		if(operators[index] == '+'){
            execl("add","Addition", "-v", NULL);
        	fprintf(stderr, "I hope you do not see me!\n");
		}   
        else if(operators[index] == '-'){
        	execl("subtract", "Subtract", "-v", NULL);
        	fprintf(stderr, "I hope you do not see me!\n");
		}
        else if(operators[index] ==  '*'){
        	execl("multiply", "Multiply", "-v", NULL);
        	fprintf(stderr, "I hope you do not see me!\n");
		}
        else{
        	execl("divide", "Divide", "-v", NULL);
            fprintf(stderr, "I hope you do not see me!\n");
        }
        exit(1);
}

int main(int argc, char *argv[]) {
	char line[MAXLEN], *temp;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	FILE *dataFile = fopen(argv[1], "r");
	//read the first line - it contains the configuration
	fgets(line, MAXLEN, dataFile); 

	// sample content for line: a + b - c
	strtok(line, " \n"); //skip the symbol representing variable/parameter
	while (temp = strtok(NULL, " \n")) {
		operators[operatorCount] = temp[0];
		//printf("operator: %c\n", operators[operatorCount]);
		operatorCount++;
		strtok(NULL, " \n"); //skip the symbol representing variable/parameter
	}

	//create the necessary # of pipes
	numPipes = operatorCount * 2 + 1;

	//CODE HERE!
	//create that many pipes using for loop
	for (int i = 0 ; i < numPipes; i++){
		pipe(fds[i]);
	}
	//create children using another for loop
	for (int j =0 ; j < operatorCount; j++) {
		if(!fork()){
			child(j);
		}
	}
	//continue to read the input data file: while (fscanf(dataFile, "%d", &value)  > 0)
	//if one int can be read, go ahead and read the remaining data for that line.
	int numbers;
	while(fscanf(dataFile,"%d",&numbers)>0){
		write(fds[0][1], &numbers, sizeof(int));
		for(int k = 0; k < operatorCount ; k++) {
		   fscanf(dataFile, "%d", &numbers);
           write(fds[k*2+1][1], &numbers, sizeof(int));
		}
		read(fds[operatorCount*2][0], &numbers, sizeof(int));
        printf("%d\n", numbers);
	}
	fclose(dataFile);
}