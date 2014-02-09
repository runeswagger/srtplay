/* Elapsed
   A simple time elapsed counter
   by Timothy Loiselle
   July 9, 2013
  */

#include <stdio.h>
#include <time.h> //accurate time elapsed counting

int main(int argc, char *argv[]){
	//simple counter
	time_t start;
	time_t now;
	struct tm* timeptr; //blah
	char clearchar[] = { 0x1b, 0x5b, 0x48, 0x1b, 0x5b, 0x32, 0x4a }; //got this from setterm -clear

	now = time(NULL); //get current time
	start = now; //initialize starttime
	while(1){
	now = time(NULL);
	printf("%s", clearchar);
	printf("%lu UTC = %s\n", now-start, asctime(localtime(&now)));
	sleep(1);
	}
	return 1;
}
