/*
*	Kenny Chen
*	2-1-17
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h> // for open
#include "blowfish.h"

#define VERSION 10.0


void printHelp(int helpOpt){
	fprintf(stderr, "%s\n", "USEAGE: cipher [-devh] [-p PASSWD] infile outfile");
	if(helpOpt == 1){
		fprintf(stderr, "%s\n", "Either the -d or -e options must be supplied exactly once");
	}
    exit(1);

}


void getFileNames(int argc, char **argv){
	int i;
	getopt(argc, argv, "devhp:");;
	for(i = 0; i < argc; i++)
		printf("%s\n", argv[i]);
}

int main(int argc, char **argv){

	int opterr = 0;			// opterr = 0 no error message, anything else error message
	if(opterr){	}			// used to solve "unused variable 'opterr'"

	int opt;
	int dCount = 0;			// used to keep count of encrypt or decrypt
	int eCount = 0;			// one or the other not both

	unsigned char * userPass = NULL;	// used to store the password
	int userpasslen = 0;

	char * infile = NULL;
	char * outfile = NULL;

	int stdi = 0;			// flag to check if infile in -
	int stdo = 0;			// flag to check if outfile is -
	int outexists = 1;		// flag to check if the out file exists assume exists
	int samefiles = 0;

	int pagesize = sysconf(_SC_PAGESIZE);
	//int pagesize = 10;

	char * inbuf = NULL;	// used for read file
	int infd = 0;			// used for open file
	int numbytesread = 0;

	int doubleprompt = 0;

	while((opt = getopt(argc, argv, "devhsp:")) != -1){
		switch(opt) {
			case 'h':
                /* The help menu was selected 
				* Should print usage to std error
				* exit with non zero (1)
                */
				printHelp(0);
				break;
			case 'v':
				printf("Version: %f\n", VERSION);
				break;
			case 'e':
				eCount++;
				break;
			case 'd':
				dCount++;
				break;
			case 's':
				doubleprompt = 1;
				break;
			case 'p':
				if(userPass == NULL){
	            	userpasslen = strlen(argv[optind-1]);
	            	userPass = malloc(userpasslen);
	            	if(userPass == NULL){
	            		perror("password error");
	            		exit(1);
	            	}
	            	memset(userPass, 0, userpasslen);
  					memcpy(userPass, argv[optind-1], userpasslen);
	            } else{
	            	printHelp(0);
	            }	            	
				break;	            
			default:
                /* A bad option was provided. */				
				printHelp(0);
		} 
	} // end of while

	//checks for proper e and d usage
	if(eCount + dCount != 1){
		printHelp(1);
	}

	// checks for only 2 files
	if((argc - optind) != 2){
		printHelp(0);
	} else{
		infile = argv[argc-2];
		outfile = argv[argc-1];
		//printf("in: %s\t out: %s\n", infile, outfile);
	}


	// if -p ARG was not provided getpassword
	if(userPass == NULL){

		char * tempvar = getpass("Enter Password: ");

		userpasslen = strlen(tempvar);
    	if(userpasslen < 1){
    		fprintf(stderr, "Password length cannot be 0\n");
    		exit(1);
    	}

		//ec
		#ifdef EXTRA_CREDIT
			if(doubleprompt){
				char * tempvardouble = malloc(userpasslen);
				memcpy(tempvardouble, tempvar, userpasslen);
				char * passver = getpass("Enter Password Again: ");	
				if(strcmp(tempvardouble, passver) != 0){
					fprintf(stderr, "%s\n", "Passwords do not match");
					free(tempvardouble);
					exit(1);
				}
				free(tempvardouble);

			}
		#else
		#endif
		
		//end of ec

    	
    	userPass = malloc(userpasslen);
    	//printf("\n\n%d\n\n", userpasslen);
		userPass = malloc(strlen(tempvar));
		if(userPass == NULL){
    		perror("password error");
    		exit(1);
    	}
    	memset(userPass, 0, strlen(tempvar));
		memcpy(userPass, tempvar, strlen(tempvar));
	}
	//printf("password is: %s\n", userPass);

    struct stat insb;
	struct stat outsb;


	if(strcmp(infile, "-") == 0){
		stdi = 1;
	} else if (stat(infile, &insb) == -1) {
	   perror(infile);
	   exit(EXIT_FAILURE);
	}
	
	if(strcmp(outfile, "-") == 0){
		stdo = 1;
	} else if (stat(outfile, &outsb) == -1) {
	   outexists = 0;
	   //printf("the file does't exist\n");
	} 

	if((long) insb.st_ino == (long) outsb.st_ino){		
		samefiles = 1;
	}

    //printf("%s I-node number:            %ld\n\n", infile, (long) insb.st_ino);   

    //printf("%s I-node number:            %ld\n", outfile, (long) outsb.st_ino);



	/*struct utsname name;
	if(uname(&name)) exit(-1);
		printf("Your computer's OS is %s@%s\n", name.nodename, name.release);*/

	//printf("The page size is %d\n", pagesize);



/////////// read in file
	inbuf = malloc(pagesize);
	if (inbuf == NULL) {
	// print err msg
		perror("inbuf");
		exit(1);
	}

	int num = -1;
	int count = 0;
	if(stdi){
		printf("Reading from std in, finish with ctrl+D\n");
		
		while((num = read(0, inbuf+count, 1)) > 0){

			numbytesread += num;
			//printf("%s\n", inbuf);
			

			//printf("%d\t%p\n", *(inbuf+count), inbuf+count);
			/*if((*(inbuf+count) == 65 || *(inbuf+count) == 66 || *(inbuf+count) == 67 || *(inbuf+count) == 68) && *(inbuf+count-1) == 91 && *(inbuf+count-2) == 27){
				// all arrows
				*(inbuf+count-2) = 0;
				*(inbuf+count-1) = 0;
				*(inbuf+count) = 0;
				count -= 2;
				continue;
			} else if((*(inbuf+count) == 126) && (*(inbuf+count-1) == 50 || *(inbuf+count-1) == 52 || *(inbuf+count-1) == 53 || *(inbuf+count-1) == 54) && *(inbuf+count-2) == 91 && *(inbuf+count-3) == 27){
				//HOME END
				*(inbuf+count-3) = 0;
				*(inbuf+count-2) = 0;
				*(inbuf+count-1) = 0;
				*(inbuf+count) = 0;
				count -= 3;
				continue;
			} else if(*(inbuf+count) == 8 || *(inbuf+count) == 6 || *(inbuf+count) == 20 || *(inbuf+count) == 5 || *(inbuf+count) == 9 || *(inbuf+count) == 1 || *(inbuf+count) == 15|| *(inbuf+count) == 16 || *(inbuf+count) == 25 || *(inbuf+count) == 7 || *(inbuf+count) == 11 || *(inbuf+count) == 12 || *(inbuf+count) == 2 ||  *(inbuf+count) == 14){
				// ctrl + o p y e t y i a f n b
 				*(inbuf+count) = 0;
				continue;
			}*/

			count++;
			//printf("%c  %d\n", *(inbuf + count-1),*(inbuf + count-1));

			if((inbuf = realloc(inbuf, (count+1)*1)) == 0){
	    		perror("error reading");
    		}
		}
	#ifdef __FreeBSD__

	#else 
	/* do something only linux needs */
	#endif

	} else {
		infd = open(infile, O_RDONLY);
		if(infd == -1){
			perror(infile);
			exit(1);
		}

		while((num = read(infd, inbuf+count*pagesize, pagesize)) > 0) {
    	//printf("%s", inbuf);
    		count++;
	    	if((inbuf = realloc(inbuf, (count+1)*pagesize+1)) == 0){
	    		perror("error reading");
	    	}

			numbytesread += num;
		}
		//printf("\n\n%s\n", inbuf);
		#ifdef __FreeBSD__

		#else 
		/* do something only linux needs */
		#endif

		close(infd);
	}



	// where the encryption is done
	/* define the output buffer */
	unsigned char * outbuf = malloc(sysconf(_SC_PAGESIZE));
	memset(outbuf, 0, sysconf(_SC_PAGESIZE));
	unsigned char * tempinbuf = malloc(strlen(inbuf)+sysconf(_SC_PAGESIZE));
	memcpy(tempinbuf, inbuf, strlen(inbuf)+sysconf(_SC_PAGESIZE));

	/* define a structure to hold the key */
	BF_KEY key;
	BF_set_key(&key, userpasslen, userPass);

	/* don't worry about these two: just define/use them */
	int n = 0;			/* internal blowfish variables */
	unsigned char * iv = malloc(8);		/* Initialization Vector */
	memset(iv, 0, 8);

	if(eCount){
  		BF_cfb64_encrypt(tempinbuf, outbuf, numbytesread, &key, iv, &n, BF_ENCRYPT);
	} else if(dCount){
		BF_cfb64_encrypt(tempinbuf, outbuf, numbytesread, &key, iv, &n, BF_DECRYPT);
	}


	/* fill the IV with zeros (or any other fixed data) */

	if(stdo){	// if i should print to std out		
			printf("%s\n", outbuf);
	} else{

		// write to file
		int fd = -1;
		if(outexists == 0){
			fd = open(outfile,  O_RDWR | O_CREAT | O_EXCL, 0666);	
		} else if(outexists == 1){
			fd = open(outfile,  O_RDWR | O_CREAT | O_TRUNC, 0666);
		}

		if(fd == -1){
			perror("failed to open");
		} else{	
			write(fd, outbuf, numbytesread);
		}

		close(fd);
	}
	
	

	if(num == -1){
		perror(infile);
		exit(1);
	}

	free(userPass);
	free(outbuf);
	free(iv);
	free(tempinbuf);
	free(inbuf);

	printf("done!\n");
	return 0;
}

