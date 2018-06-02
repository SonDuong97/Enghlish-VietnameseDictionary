#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

#define NAME_LENGTH  50
#define MEAN_LENGTH 10000

void separate_mean(char* mean){
  int i=0,j=1;
  while (mean[j]!='\0'){
    if (mean[j]=='\\' && mean[j+1]=='n'){
      mean[i++]='\n';
      j+=2;
    }
    else{
      if (i!=j)
	mean[i++]=mean[j++];
      else{
	i++;j++;
      }
    }
  }
  mean[i]='\0';
}

int main(int argc, char* argv[]){
  if (argc < 3){
    printf("Loi cu phap\n");
    exit(EXIT_FAILURE);
  }
  FILE* datafile;
  if ((datafile=fopen(argv[1],"r"))==NULL){
      printf("Loi mo file\n");
      exit(EXIT_FAILURE);
  }
  BTA *convertfile;
  convertfile=btcrt(argv[2], 0, FALSE);
  char name[NAME_LENGTH],mean[MEAN_LENGTH];
  while (fscanf(datafile,"%[^\t]",name)==1){
    fgets(mean,MEAN_LENGTH,datafile);
    separate_mean(mean);
    btins(convertfile,name,mean,strlen(mean)+1);
  } 
  btcls(convertfile);
  fclose(datafile);
  return 0;
}
