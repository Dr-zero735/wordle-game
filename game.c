#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<assert.h>
#include <errno.h>

#define green   		1
#define red     		0
#define yellow  		2
#define max     		5508
#define valOK   		0
#define valBAD  		1
#define valNoSuchWord	2
#define ClrGreen		"\033[0;32m"
#define ClrRed 			"\033[0;31m"
#define ClrYellow		"\033[0;33m"
#define ClrStop 		"\033[0m"

typedef char ValResult;

bool win;
typedef char Result;
bool continuation;
static char words[max][5];
int rounds;
bool corrects[5];

ValResult validator(char*);
char* readline(void);
void prompt(char*);
void gameloop(char*);
char *randomword(int);
void seed(void);
int readfile(char*);
bool isin(char, char*);
void print_result(Result*,char*,char*);
Result *cw(char*,char*);
Result cc(char, char* ,int);
int main(int ,char**);

char *randomword(int m){
	static char ret[8];
	int x= rand() % m;
	ret[0]=words[x][0];
	ret[1]=words[x][1];
	ret[2]=words[x][2];
	ret[3]=words[x][3];
	ret[4]=words[x][4];
	ret[5]=0;
	return ret;
}

bool isin(char guess ,char* word){
	bool result=false;
	int size= strlen(word);
	for(int i=0;i<size;i++){
	if(word[i]==guess){
		result =true;
		break;
		}
	}
	return result;
	}

int readfile(char *filename){
	char buf[8];
	int i;
	FILE *fd;
	fd=fopen(filename,"r");
	if(!fd){
	perror("fopen");
	return -1;
	}
	i=0;
	memset(buf,0,8);
	while(fgets(buf,7,fd)){
	int size=strlen(buf);
	if(size<1){
	memset(buf,0,8);
	continue;
	}
	size--;
	buf[size]=0;
	if(size != 5){
	memset(buf,0,8);
	continue;
	}
	words[i][0]=buf[0];
	words[i][1]=buf[1];
	words[i][2]=buf[2];
	words[i][3]=buf[3];
	words[i][4]=buf[4];
	memset(buf,0,8);
	i++;
	if(max<=i)
	    break;
	}
	fclose(fd);
	return i;
}


void print_result(Result* res,char* correct, char* guess){
	for(int i=0;i<5;i++){
	switch(res[i]){
		case green:
			printf("%s%c%s",ClrGreen,guess[i],ClrStop);
			break;

		case red:
			printf("%s%c%s",ClrRed,guess[i],ClrStop);
			break;

		case yellow:
			printf("%s%c%s",ClrYellow,guess[i],ClrStop);
			break;

		default:
			printf("unkown \n");
			break;
	}
	}
	printf("\n");
}


Result *cw(char *word,char *guess){
	static Result res[5];
	int i;
	for(i=0;i<5;i++){
		res[i]=cc(guess[i],word,i);
	}
	return res;
}

Result cc(char guess,char* word ,int idx){
	char correct = word[idx];
	if(correct ==guess){
		corrects[idx]=true;
		return green;
	}else if(isin(guess, word)){
		return yellow;
	}
	return red;
	}

void seed(){
	int x;
	x=getpid();
	srand(x);
	return;
}

char* readline(){
	static char buf[10];
	int size;
	memset(buf,0,8);
	fgets(buf,7,stdin);
	size=strlen(buf);
	assert(size>0);
	size--;
	buf[size]=0;
	return buf;
}

void gameloop(char* correct){ 
	char *input;
	Result *res;
	ValResult valres;
	int i,c;
	prompt(correct);
	input=readline();
	valres=validator(input);
	switch(valres){
		case valBAD:
			printf("%s\n","bad input");
			return ;
		case valNoSuchWord:
			printf("%s\n","no such word --only 5 letters english words");
			rounds++;
			return ;
		default:
			break;
	}
	res=cw(input,correct);
	for(i=c=0;i<5;i++){
		if(corrects[i]==true){
			c++;
		}
	}
	print_result(res,correct,input);
	if(c==5){
		 win=true;
		continuation=false;
		return ;
	}
	rounds++;
	if(rounds>4){
		 win=false;
		continuation=false;
		return;
	}
	return ;
}

ValResult validator(char* word){
	int i,n;
	bool ret;
	bool strcmp_(char *str1,char *str2){
		int s;
		s=0;
		for(int i=0;i<5;i++){
			if(str1[i]==str2[i])
				s++;			
		}
		return (s ==5)?
		true:
		false;
	};
	n=strlen(word);
	if(n!=5)
		return valBAD;

	ret =false;
	for(i=0;i<max;i++)
		if(strcmp_(words[i],word)){
			ret=true;
			break;
		}
	if(ret){
		return valOK;
	}else{
		return valNoSuchWord;
	}	
}

void prompt(char* correctword){
	int i;
	for(i=0;i<5;i++){
	switch(corrects[i]){
		case false:
			printf("-");
			break;

		case true:
			printf("%c",correctword[i]);
			break;
	}
	}
	printf("\n\n%d>",(5-rounds));
	fflush(stdout);
	return ;
}

int main(int argc,char* argv[]){

	rounds=0;
	int n= readfile("list.txt");
	char *p;
	corrects[0]=false;
	corrects[1]=false;
	corrects[2]=false;
	corrects[3]=false;
	corrects[4]=false;

	seed();

	assert(!(n<0));
	p=randomword(n);
	printf("-----\n");
	fflush(stdout);

	win=false;
	continuation=true;

	printf("********* Welcome to THE GAME *********\n");

	printf("Enter your guess:\n");
	while(continuation){
		gameloop(p);
	}

	printf("The correct word was: '%s' \n",p);
	if(win){
		printf("congratulations ,you won\n");
	}else{
		printf("you lost ,game over\n");
	}
	return 0;
}
