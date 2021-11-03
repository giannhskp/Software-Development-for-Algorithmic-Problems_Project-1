#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "Vector/vector.h"
#include "./hashTable/hashTable.h"
#include "./hashTable/hashTableList/hashTableList.h"
#include "Hypercube/hypercube.h"
#include "./parsing/parsingCluster.h"
#include "./Clustering/clustering.h"
int d;
// int k;
int new_dimension;
int m;
int probes;
int w;
int numOfVecs;
int hashTableSize;
int complete;
int k_LSH;

void printOptions(){
  printf("_________________Options____________________\n\n");
	printf("1. /repeat <new_input_file>\n\n");
	printf("2. /exit\n\n");
	printf("_____________________________________\n\n");
}



int main(int argc, char *argv[]) {
  srand(time(NULL));
  int option;
  complete=0;
  char str[200];
  char inputFile[200];
  int inputflag=0;
  char confFile[200];
  int confflag=0;
  char outputFile[200];
  int outputflag=0;
  char method[200];
  int methodflag=0;


  while((option = getopt(argc, argv, "i:c:o:m:")) != -1){
     switch(option){
        case 'i':
        inputflag++;
        strcpy(inputFile,optarg);
        printf("Given input File : %s\n", inputFile);
        break;

        case 'c':
        printf("%d\n",optind);
        if(strcmp(argv[optind-1],"-complete")==0){
          printf("Complete option ON\n");
          complete=1;
        }
        else{
          confflag++;
          strcpy(confFile,optarg);
          printf("Given configuration File : %s\n", confFile);
        }
        break;

        case 'm':
        methodflag++;
        strcpy(method,optarg);
        printf("Method: %s\n", method);
        break;

        case 'o':
        outputflag++;
        strcpy(outputFile,optarg);
        printf("Given output File : %s\n", outputFile);
        break;

        case ':':
         printf("option needs a value\n");
         break;
        default: /* '?' */
          fprintf(stderr, "Usage: %s –i <input file> –c <configuration file> -o <output file> -complete <optional> -m <method: Classic OR LSH or Hypercube>\n",argv[0]);
          exit(EXIT_FAILURE);
     }
  }


  if(!inputflag){
    printf(">Input file name: ");
    fflush(stdin); // clear stdin buffer
    if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
      perror("Error reading string with fgets\n");
      exit(1);
    }
    sscanf(str,"%s\n",inputFile);
    printf("Given input File : %s\n", inputFile);
  }
  if(!confflag){
    printf(">Conf file name: ");
    fflush(stdin); // clear stdin buffer
    if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
      perror("Error reading string with fgets\n");
      exit(1);
    }
    sscanf(str,"%s\n",confFile);
    printf("Given query File : %s\n", confFile);
  }
  if(!outputflag){
    printf(">Output file name: ");
    fflush(stdin); // clear stdin buffer
    if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
      perror("Error reading string with fgets\n");
      exit(1);
    }
    sscanf(str,"%s\n",outputFile);
    printf("Given output File : %s\n", outputFile);
  }
  if(!methodflag){
    printf(">Method's name: ");
    fflush(stdin); // clear stdin buffer
    if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
      perror("Error reading string with fgets\n");
      exit(1);
    }
    sscanf(str,"%s\n",method);
    printf("Given method's name: %s\n", method);
  }

  FILE* fptr;
  List list;
  while(1){
    d = findDim(inputFile);
    printf("DIMENSION = %d\n",d);
    int numOfClusters=5,l=3,mHyper=10,probes=2;
    new_dimension=3;
    k_LSH=4;
    w=8;
    readConfFile(confFile,&numOfClusters,&l,&mHyper,&probes);
    list = initializeList();
    readFile(inputFile,&list,&numOfVecs);

    fptr = fopen(outputFile, "w");
    if(fptr == NULL){
      /* File not created hence exit */
      printf("Unable to create file.\n");
      exit(EXIT_FAILURE);
    }

    clustering(list,fptr,method,numOfClusters,l,mHyper,probes);


    printOptions(); // just printing the commands options for the user


    char command[200];

      printf("\n");
      printf(">Enter a command: ");
      fflush(stdin); // clear stdin buffer
      if (fgets(str, sizeof(char)*200, stdin) == NULL) { // read a command
        perror("Error reading string with fgets\n");
        exit(1);
      }
      else if(strstr(str, "/repeat") != NULL) {
        sscanf(str,"%s %s\n",command,inputFile);
        printf("FILE: %s\n",inputFile);
        if(strcmp(method,"LSH")==0 || strcmp(method,"HyperCube")==0)
          listDelete(list,0);
        else
          listDelete(list,1);
        fclose(fptr);
        continue;
      }
      else if(strcmp(str,"/exit\n")==0){
        break;
      }
      else{
        printf("\n\n  --- Wrong command ! Please, try again. ---  \n\n");
        printOptions(); // just printing the commands options for the user
        continue;
      }

  }

  if(strcmp(method,"LSH")==0 || strcmp(method,"HyperCube")==0)
    listDelete(list,0);
  else
    listDelete(list,1);

  fclose(fptr);
  return 0;
}
