/* Text_spliter
 * Authors: lion2486 && sudavar && xe3r
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFER 128*1024-1	/* 1 MB buffer */
#define DEFAULT 10			/* Default file size */
#define MAX_FILE_SIZE 256	/* Max file size 256MB == 268435456 Bytes */

void next_name(char *, int *, int, char *, char *, char []);
int close(FILE *, FILE *, char *, FILE *, char *, int, char *);
int last(char *);
void itos(int, char *);

int main(int argc, char *argv[])
{
	FILE *src, *targ;
	int  file_c = 1, file_size = 0, name_size, force = 0;
	char buf[BUFFER], ext[5], *spl_name, *credits, *backup, *backup2, str[10];
	double size = 0, tempsize;
	long src_size;
  
	credits =
		"\t--------------------------------------\n"
		"\t             Credits:                 \n"
		"\tWritten by Lion2486 , Sudavar and Xe3r\n"
		"\t--------------------------------------\n"
		"\t~~~~~~~~~~~~~~ DI @ UOA ~~~~~~~~~~~~~~\n\n\nPress enter to continue...";
	/* Checks if the option force is on.
	 * Force makes the program close a new file exactly when it finds
	 * that the size of the new file exceeds the allowed.
	 * By default I made the new file stop when it had exceed the
	 * size and it ends at a preselected character(or more)(now it is'\n')
	 * so it wont cut a sentence until it is finished.
	 */

	if(argc > 2)
		if(!(strcmp(argv[argc - 1] ,"-f"))){
		  argc--;
		  force = 1;
		}

	if(argc > 3 || argc == 1){
		fprintf(stderr ,
		  "Usage: %s [file_name] [size_of_splits(in MB)][options]\nor with default split size %dMB %s [text_name]\n"
		  "Program shuts down\tPress any key...", argv[0], DEFAULT, argv[0]);
		getchar();
		return EXIT_FAILURE;
	}

	if(argc == 3)
		if( (atof(argv[2]) > MAX_FILE_SIZE || atof(argv[2]) <= 0)){
		  fprintf(stderr,
				  "\aEXIT_FAILURE at output file size!\nMaximum file size %dMB\nProgram shut's down unsuccessfully\n", MAX_FILE_SIZE );
		  return EXIT_FAILURE;
		}

	if(argc == 2){
	  printf("\nOutput files are set to %d MB\n", DEFAULT);
	  size = DEFAULT;
	}
	else{
	  printf("\nOutput files are set to %.1f MB\n", atof(argv[2]));
	  size = atof(argv[2]);
	}


	/* Estimating input file's size */
	if(!(src = fopen(argv[1],"rb")))
		return close(NULL, NULL, NULL, stderr, "\aEXIT_FAILURE in input file opening!", 0, credits);
	else{
		fseek(src, 0, SEEK_END);
		src_size = ftell(src);
		fseek( src, 0, SEEK_SET);
		tempsize = (double)src_size/(1024*1024);
		printf("%s size is %.2f MB\n", argv[1], tempsize);
	}


	/* Checking whether split size is valid or not */
	if((int)src_size <= (int)size*1024*1024)
		return close(NULL, NULL, NULL, stderr,"\aInvalid split size data!", 0, credits);
	size *= 1024*1024;
	src_size /= 1024/1024;


	/*
	 * In variable char *spl_name we keep name of current output file,
	 * in other words argv[1]."_splitX.txt", where X number of split file
	 */
	name_size = strlen(argv[1]);
	if(  !(spl_name = malloc((name_size + 16) * sizeof(char)))
	  || !(backup = malloc((name_size + 16) * sizeof(char)))
	  || !(backup2 = malloc((name_size + 16) * sizeof(char))) )
	return close(src, NULL, NULL, stderr, "\aProblem with memory allocation", 0, credits);

	/* Creating name of first output file and keeping a backup of input file name */
	strcpy(backup2 , argv[1]);
	memmove(ext, argv[1]+name_size-4, 4);
	ext[4] = '\0';
	(argv[1])[name_size-4] = '\0';

	/* Creating name of split files */
	spl_name = strcat(argv[1], "_split");
	strcpy(backup, spl_name);
	itos(file_c, str);
	strcat(spl_name, str);
	strcat(spl_name, ext);

	/* Opening first output file */
	if(!(targ = fopen(spl_name, "wb")))
		return close(targ, src, spl_name, stderr, "\aEXIT_FAILURE in file opening!", 0, credits);
	printf("\tProcessing file: %s\n", spl_name);


	/* Reading line and saving in buf */
	while(fgets(buf, BUFFER, src) != NULL){
		/*
		 * While file size not reached && the last char is a '\n'
		 * or the force option is on(description above)
		 */
		if((file_size + strlen(buf) + 1 >= size) && (last(buf) || force)){
			if(fclose(targ) == EOF)
				return close(src, NULL, spl_name, stderr, "\aEXIT_FAILURE in file closing!", 0 , credits);

			/*
			 * Creating new name for next output file,
			 * opening this new output file and checking if successful
			 */
			next_name(spl_name, &file_c, name_size, str, backup, ext);
			if(!(targ = fopen(spl_name, "wb")))
				return close(src , NULL , spl_name , stderr ,"\aEXIT_FAILURE in file opening!", 0 , credits);
			printf("\tProcessing file: %s\n", spl_name);

			/* Writing first line in new output file and increasing file size */
			fprintf(targ, "%s", buf);
			file_size = strlen(buf)+1;
		}
		else{
			/* Printing line -- Same as fputs(buf, targ); and increasing file size */
			fprintf(targ, "%s", buf);
			file_size += strlen(buf)+1;
		}
	}

	/* Program ending */
	fflush(targ);
	printf("Text splitter split %s to %d files\n\n", backup2, file_c);
	if(fclose(src) == EOF)
		return close(targ, NULL, spl_name, stderr, "\aEXIT_FAILURE in file closing!", 0, credits);
	if(fclose(targ) == EOF)
		return close(src, NULL, spl_name, stderr, "\aEXIT_FAILURE in file closing!", 0, credits);
	return close(NULL, NULL, NULL, stdout, "\n\nSuccessful splitting ... DONE!", 1, credits);
}


void next_name(char *spl_name, int *file_c, int name_size, char *str, char* backup, char ext[])
{
	(*file_c)++;
	strcpy(spl_name, backup);
	itos(*file_c, str);
	strcat(spl_name, str);
	strcat(spl_name, ext);
	return;
}


int close(FILE *close1, FILE *close2, char *table, FILE *fp, char *msg, int k , char *credits)
{
	if(close1 != NULL)
		fclose(close1);
	if(close2 != NULL)
		fclose(close2);
	if(table != NULL)
		free(table);
	fprintf(fp,
			"%s \n%s \n%s", msg,
			( k == 1 ) ? "Program shuts down\n" : "Program shuts down unsuccessfully",
			( k == 1 ) ? credits : "Press enter to continue..." );
	getchar();
	if(!k)
		return EXIT_FAILURE;
	return 0;
}

/*
 * Tests if the last letter is proper so the sentence is not cut of between 2 files.
 */
int last(char *buf)
{
	if(buf[strlen(buf)-1] == '\n')
		return 1;
	else
		return 0;
}


void itos(int number ,char *str)
{
	sprintf(str, "%d", number);
}
