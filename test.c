#include <stdio.h>
//#define GET_NEXT_ENTRY(X) while(i++<size)if(X[i] == 0)if(X[i+1] == 0)break;i++;i++;if(i>=size)return 1;
#define GET_NEXT_ENTRY(X) while((X[i]+X[++i])&&i<size);i++;if(i>=size)return 1;
#define PRINT printf("%i\t%s\n",i, &string[i]);
int main(){
	//try to perfect finding "\0\0"
	char string[] = { 't','e','x','t',0,0,'t','e','x','t','2',0,'t','e','x','t','3', 0,'b','a','d',0,0,'e','n','d',0 };
	int i = 0, size = sizeof(string);
	PRINT
	GET_NEXT_ENTRY(string);
	PRINT
	GET_NEXT_ENTRY(string);
	PRINT
	return i;
}
