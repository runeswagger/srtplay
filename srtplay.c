/*srtplay V 0.1
  display subtitles
  by Timothy Loiselle
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

//use this to limit the max size of input file
#define ISREASONABLE(X) (X <= 10000000)

struct srtent {
	int id; //the line number--redundant
	char * line;
	time_t duration; //how long the subtitle is active for
	time_t start; //when it goes up on screen
};

time_t human_readable_to_timestamp(char* hrtime){
	//00:00:58,159
	//turn above into a timestamp
	//for adding to the starting time to get trigger time
	//since resulting timestamp is relative to UTC
	time_t r = 0; //Epoch
	int hr,min,sec,ms;
	sscanf((const char*) hrtime, "%2i:%2i:%2i,%3i", &hr, &min, &sec, &ms);
	r += sec;
	r += min*60;
	r += hr*3600;
	return r;
}

char* timestamp_to_human_readable(time_t t){
	//this function keeps 12 bytes handy for it's return value
	//you cannot free the pointer returned and you must copy the buffer elsewhere to
	//retain the contents of the conversion beyond the next call to this function
	static char hrtime[12] = {};
	int hr,min,sec,ms;
	hr = t/3600;
	t -= hr*3600;
	min = t/60;
	t -= min*60;
	sec = t;
	ms = 0; //time_t doesn't keep down to the millisecond
	sprintf(hrtime, "%02i:%02i:%02i,%03i", hr, min, sec, ms);
	return hrtime;
}

inline int s_newline_null(char *b, int size){
	//preform s/\n/\0/g
	int r = 0; //return value
	while(--size) if(b[size] == '\n') {b[size] = 0; r++;}
	return r;
}

inline int s_null_newline(char *b, int size){
	int r = 0;
	while(--size) if(b[size] == 0) {b[size] = '\n'; r++;}
	return r;
}

int parse_srt_file(struct srtent** s, char *srtfile, int size){
	//index all of the dialog in the file
#define GET_NEXT_TOKEN(X) while(X[i++]&&i<size);if(i>=size)break;
//damned windows line endings
#define GET_NEXT_ENTRY(X) while((X[i] || X[i+2]) && i < size)i++;i+=3;if(i>=size)break;

#define ADD_SRTENT(X) X=realloc(X,sizeof(struct srtent)*(nsubs+1));
#define LOG(X) printf(X" = %s\n",&srtfile[i]);

	struct srtent* subtitle = *s; //i want a ** as arg because realloc enables subtitle to migrate
	int nsubs = 0, i = 0, p = 0; //the p is a placeholder for calculating the size of the dialog string

	//I've replaced all newlines with nulls because strings are null delimited in the C world
	while(i < size){
		//must add struct to list
		ADD_SRTENT(subtitle);
		switch(0){ //for future addition of token qualification
			//token is id
			case 0:
				subtitle[nsubs].id = atoi(srtfile + i);
				GET_NEXT_TOKEN(srtfile);
			//token is timing
			case 1:
				//00:00:58,159 --> 00:01:00,759
				p = i; //my nulling screws up token parsing, so i save offset and parse before nulling
				GET_NEXT_TOKEN(srtfile);
				srtfile[p+12] = 0; //make it null
				subtitle[nsubs].start = human_readable_to_timestamp(srtfile + p);
				subtitle[nsubs].duration = human_readable_to_timestamp(srtfile + p + 17) - subtitle[nsubs].start;

			//token is dialog
			case 2: //i'll have scattered nulls for multiline dialog FIXME
				subtitle[nsubs].line = srtfile + i;
				p = i;
				GET_NEXT_ENTRY(srtfile);
				s_null_newline(srtfile + p, i - p - 1); //merge multiline subtitles
				printf("%s", subtitle[nsubs].line);
				//change i - p to i - p - 1 if you don't like '\0\0' to become '\n\0'... me, I like it
				break;
			//token is invalid
			default: printf("Fell through to ERROR!!!\n"); //capitalization and !!! for extra importance
		}
		nsubs++;
	}

	*s = subtitle; //update the pointer so that main() is aware of any realloc booboos

	return nsubs;
}

int main(int argc, char *argv[]){
	//read int SRT subtitles and render them to a terminal
	(int)argc;
	char clearchar[] = { 0x1b, 0x5b, 0x48, 0x1b, 0x5b, 0x32, 0x4a }; //clears a unix terminal and homes the cursor
	char * srtfile; //the files contents for lexing
	struct srtent *subtitle = NULL; //an array of subtitles
	int nsubs = 1, id = 0; //the number of subs... for iteration

	//some initialization
	struct stat f;
	FILE* input = NULL;
	input = fopen(argv[1], "r"); //dumb argument parsing FIXME
	fstat(fileno(input), &f); //stat the file

	if(ISREASONABLE(f.st_size))
		srtfile = malloc(f.st_size);
	else
		return 1;

	fread(srtfile, 1, f.st_size, input); //fill the buffer
	fclose(input); //close the file

	//prepreping the input
	s_newline_null(srtfile, f.st_size);
	nsubs = parse_srt_file(&subtitle, srtfile, f.st_size);

	while(id < nsubs){ //disabled
		printf("%s\n", clearchar); //clear the screen
		printf("%s\n", subtitle[id].line);
		sleep(subtitle[id++].duration);
		//the loop sleeps for strent[current].duration and then goes back to polling;
	}

	return 1;
}
