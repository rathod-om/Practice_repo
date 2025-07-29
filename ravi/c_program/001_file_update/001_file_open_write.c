#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/* Required Declaration */
#define MAX_SIZE 100

int main(){
	FILE *filp;
	char filename[] = "test.txt";
	char buffer[MAX_SIZE];

	filp = fopen(filename,"a+");
	if (filp == NULL){
		perror("Open failed\n");
		return -1;
	}

	if(fread(buffer,1,sizeof(buffer),filp) != 0){
		printf("content before update: %s\n",buffer);
	}
	char buff_read[MAX_SIZE] = "updated with this string\n";
	fwrite(buff_read,1,strlen(buff_read),filp);
	fclose(filp);
	printf("File updated successfully\n");
	return 0;
}
