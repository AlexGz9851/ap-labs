#include <stdio.h>
int findNonRepeating(char* s){
	int lApp[256]; 
	for(int j=0;j<256;j++){
		lApp[j] = -1;
	}
	int first=0;
	int firstOfMax=0;
	int maxLength=0;

	int i=0;
	while(s[i]!= '\0'){
		if(lApp[s[i]]>= first){
			if((i-first+1)>maxLength){
				firstOfMax = first;
				maxLength = i - first;
			}
			first++;
		}else{
			lApp[s[i]] = i;
			i++;
		}
	}
	if(i-first>maxLength){
		maxLength = i - first;
	}
	return maxLength;
}


int main(int argv , char** args){
	for(int i=1;i<argv;i++){
		printf("%d\n",findNonRepeating(args[i]));
	}
}
