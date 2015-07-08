

// Game engine for having a text based decision making game
// Game format:
//  - 	Text is displayed to the user with variable speed
//  - 	The smallest unit of displayable text is a sentence (variable-lengthed string)
//		with time delays between words and sentences. But defining every delay between
//		every word would be crap, so maybe it should be random and slow or random and 
//		fast. So sentences have a speed and a degree of randomness as a percentage of 
//		their speed. Maybe they can also SKIP the sentence their currently reading.
//	-	Sentences can link to the next sentence to be read (but there is no option
//		on which sentence comes next)
//	-	Users must then make decisions based on the text they are reading.
//	-	Decisions will be made by simple one word commands that change based on the
//		question sentence (i.e. "There is a door in front of you, open or close?")
//			Response: open
//			...
//			The door is locked.
//	 	Choosing different options will change the sentence to be displayed next.
//	-	Some sentences will be displayed and require no action and are simply part of
//		a longer dialogue. Longer dialogues contain sentences and no decisions.

// To compile, use gcc main.c -o main

#define bool char
#define true 1
#define false 0

#ifdef DEBUG_ON
	bool debug = true;
#else
	bool debug = false;
#endif

#define DEBUG if(debug)

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct Tempo{
	int delay; // Delay in milliseconds on average between words
	int jitter; // The amount +/- to add randomly to the delay
};

struct Option{
	char * command;
	int line;
	struct Option * next;
};

struct Sentence{
	char * text;
	struct Tempo * tempo;

	bool decision; // = 1 if a decision must be made
	struct Option options;
};

// Initialise a sentence without a decision
void initSentence(struct Sentence * s, char * text, struct Tempo * tempo) {
	s->text = text;
	s->tempo = tempo;
	s->decision = 0;
}

// Initialise a sentence that will later have a decision to make
void initDecision(struct Sentence * s, char * text, struct Tempo * tempo) {
	s->text = text;
	s->tempo = tempo;

	s->decision = 1;

	struct Option o;// Null option 
	o.line = 0;
	o.next = 0;
	s->options = o; 
}

// Add a sentence option
void addOption(struct Sentence * s, struct Option * o) {
	struct Option * cursor = &s->options, * prev;
	prev = cursor;
	while (cursor != 0) {prev = cursor; cursor = cursor->next;}//printf("%p\n", cursor);}// Get to the end the options
	if(prev) prev->next = o; else printf("Failed to add option\n");
}

void printSentence(struct Sentence * s) {
	printf("Sentence:\n\ttext:'%s'\n", s->text);

	if (s->tempo) {
		printf("\ttempo:%d +/- %d\n", s->tempo->delay, s->tempo->jitter);
	}

	if (s->decision) {
		printf("\toptions:\n");

		struct Option * cursor = s->options.next;
		int i = 0;
		while(cursor != 0){
			printf("\t\tOption [to line:%d]:\t%s\n", cursor->line, cursor->command);
			cursor = cursor->next;
			i++;
		}
	}
}

int selectChoice(struct Option * o, char * command) {
	struct Option * cursor = o->next;
	while(cursor != 0) {
		char * input = command, * expected = cursor->command;

		DEBUG printf("Compare \t'%s' with \t'%s'\n", input, expected);
		// Cycle through both strings before they end
		while (*input != 0 && *expected != 0) {
			if (*input != *expected) {/*printf("'%c' '%c'\n", *input, *expected);*/ break;} // Stop if they aren't equal
			input++;
			expected++;
		}
		if (*input == 0 && *expected == 0) { // This means they are equal
			DEBUG printf("line: %d\n", cursor->line);
			return cursor->line; // Return this choice
		}
		cursor = cursor->next;
	}
	return -1; // Tried every option and didn't find one equal
}

// Put the new end character at the first newline
void trimNewline(char * s) {
	if (s) {
		while (*s != 0) {
			if (*s == '\n') {
				*s = 0;
				return;
			}
			s++;
		}
	}
}

void flush() {
	char ch;
	while ((ch=getchar()) != EOF && ch != '\n');
}

bool is_character_needing_escaping(char c) {
	return c == '(' || c ==')';
}

int unescaped_characters(char * potentialy_unescaped) {
	// How much memory is required for the new string?
	// The original size + the number of slashes added
	int count = 0;

	char * cursor = potentialy_unescaped;
	while (*cursor != 0) {
		if (is_character_needing_escaping(*cursor)) {
			count++;
		}
		cursor++;
	}

	printf("There are %d unescaped characters in %p\n", count, potentialy_unescaped );
	return count;
}

char * escape(char * unescaped) {
	printf("escape(%p)\n", unescaped);
	int finalSize = sizeof(unescaped) / sizeof(char);

	finalSize += unescaped_characters(unescaped);

	char * escaped = malloc(sizeof(char) * finalSize);

	char * cursor = unescaped; //Move cursor back to beginning
	char * previous = cursor;
	while (*cursor != 0) {
		printf("Cursor %p = %c, %p = %s\n", cursor, *cursor, escaped, escaped);
		sleep(1);
		if (is_character_needing_escaping(*cursor)) {
			int position = cursor - previous;
			printf("strcpy to %p\n", escaped + position);
			stpncpy(escaped + (previous - unescaped), previous, position - 1);
			escaped[previous - unescaped + position] = '\\';
			previous = cursor + 1;
		}
		cursor++;
	}

	return escaped;
}

int readSentence(struct Sentence * s, int current) {
	if (s == 0){
		DEBUG printf("%s\n", "END OF THE STORY");
		return -1;
	}

	s = s + current;

	// char command[512] = "say ";

	// if(s->text) {
	// 	if (unescaped_characters(s->text) != 0) {
	// 		char * escaped = escape(s->text);

	// 		strcpy(command + 4, escaped);

	// 		free(escaped);

	// 	} else {
	// 		strcpy(command + 4, s->text);
	// 	}

	// 	printf("Saying... (%s)\n", command);
	// 	if(fork()==0){system(command);exit(0);}
	// }

	struct Tempo typical;
	typical.delay = 1;
	typical.jitter = 3;

	if (s->tempo == 0) {
		s->tempo = &typical;
	}

	char * cursor = s->text, * wordStart;
	int jitter = 0;
	int letters = 0;
	wordStart = cursor;

	//Start a new paragraph - deep breath
	usleep(1000000);

	if (cursor)
	while (*cursor != 0) {
		letters = 0;
		while (*cursor != ' ' && *cursor != 0) {
			cursor++;
			letters++;
			//Move forward to first space
		}

		//Print between wordStart and cursor
		while (wordStart < cursor) {
			putchar(*wordStart);
			wordStart++;
		}

		fflush(stdout);

		if(wordStart[-1] == '.') usleep(1000000);
		if(wordStart[-1] == ',') usleep(500000);

		if (*cursor) {cursor++;} else {break;}
		
		double d = ((double)rand()/(double)RAND_MAX);
		jitter = ((double)s->tempo->jitter) * d * 1000;

		usleep(s->tempo->delay * 100000 + jitter);
	}

	putchar('\n');
	if (s->decision) {
		struct Option * cursor = s->options.next;
		int i = 1;
		while(cursor != 0){
			printf("\t%d: %s", i, cursor->command);
			cursor = cursor->next;
			i++;
		}
		putchar('\n');

		int max = 20; //Max size of commands is here
		char * command = malloc(sizeof(char) * max);
		int choice = 0;

		// Loop until a correct command is typed in
		do{
			DEBUG printf("%s\n", "What is your decision?");
			fgets(command, max, stdin);
			// flush();
			trimNewline(command);

		} while ((choice = selectChoice(&s->options, command)) == -1);
		printf("You chose %s...\n\n", command);
		
		free(command);
		return choice;
	} else {
		return current + 1;//Advance the index
	}
}

void readSentences(struct Sentence * sentences, int numLines) {
	int next = 0;
	while (next < numLines) {
		next = readSentence(sentences, next);
	}
}

void readLinesFlat(char *** lines, int numLines) {
	// Sentences will just be stored in a massive array as they are in lines

	char ** lineCursor = *lines;

	struct Sentence sentences[numLines];

	int sentenceIndex = 0;
	int numSentences = 0;
	// DEBUG printf("%s\n", );
	while (sentenceIndex < numLines) {
		if (lineCursor[sentenceIndex + 1] != 0 && lineCursor[sentenceIndex + 1][0] == '\t') { // Indent on the next line == decision
			initDecision(sentences + sentenceIndex, lineCursor[sentenceIndex], 0);
			numSentences++;
			DEBUG printf("Decision at %d \n", sentenceIndex);
			int optionsIndex = sentenceIndex + 1;
			while (lineCursor[optionsIndex] != 0 && lineCursor[optionsIndex][0] == '\t') {
				DEBUG printf("\tOption at %d: ", optionsIndex);
				struct Option * o = malloc(sizeof(struct Option));
				o->next = 0;

				char * space = &lineCursor[optionsIndex][1];
				while (*(space++) != ' ');

				*(space - 1) = 0; // The space after the number becomes 0

				sscanf(lineCursor[optionsIndex] + 1, "%d", &o->line);
				DEBUG printf("[line: %d]", o->line);

				o->line--;

				DEBUG printf("'%s'\n", space);

				o->command = space;
				addOption(sentences + sentenceIndex, o);
				DEBUG printf("Option added\n");
				optionsIndex++;
			}
			DEBUG printf("Out of while loop\n");
			// struct Option * cursor = sentences[sentenceIndex].options.next;
			// while(cursor != 0){
			// 	cursor->line -= optionsIndex;
			// 	cursor = cursor->next;
			// }
			DEBUG printSentence(sentences + sentenceIndex);
			sentenceIndex = optionsIndex;
		} else {
			initSentence(sentences + sentenceIndex, lineCursor[sentenceIndex], 0);
			numSentences++;
			DEBUG printSentence(sentences + sentenceIndex);
			DEBUG printf("%s\n","next");
			sentenceIndex++;
		}
	}
	printf("Now reading sentences...\n");
	readSentences(sentences, numLines);
}

void readTGF(char * fileName) {
	FILE * file = fopen(fileName,"r");

	fseek(file, 0L, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char * fileBuffer = malloc(sizeof(char) * size);
	char * cursor = fileBuffer;
	char * start = fileBuffer;

	fread(fileBuffer, sizeof(char), size, file);

	int numLines = 1;

	//Until we reach the end of the fileBuffer
	while (*cursor != 0) {
		if (*cursor == '\n') {
			numLines++;
		}
		cursor++;
	}

	char ** lines = malloc(sizeof(char *) * (numLines) + 1);// +1 is for 0 at the end

	int currentLine = 0;

	cursor = start;
	lines[currentLine++] = cursor;
	while (*cursor != 0) {
		if (*cursor == '\n') {
			*cursor = 0; // End each string
			cursor++;
			lines[currentLine++] = cursor;
			continue;
		}
		cursor++;
	}

	// Algorithm:
	//	 Determine if this is a decision or sentence
	//	 	If it is a decision, collect it's options
	//		Else connect it to the next sentence

	// initSentence(&initial, lines[0], &typical, &initialDecision);
	
	lines[numLines] = 0;
	readLinesFlat(&lines, numLines);

	free(lines);
	free(fileBuffer);
}

int main(int argc, char ** argv) {

	// //What I want to be able to do:

	// struct Sentence initialSentence, initialDecision;

	// struct Tempo typical;
	// typical.delay = 100;
	// typical.jitter = 1;

	// initSentence(&initialSentence, "This is my first sentence! I wonder what it looks like if use a really really long bit of prose and pass it through the same function... Better be quick as this is getting really low on battery!! :)\0", &typical, &initialDecision);
	// initDecision(&initialDecision, "This is my first decision!\0", &typical);

	// struct Option one = (struct Option){.command="back",.sentence=&initialSentence,.next=0};
	// addOption(&initialDecision, &one);

	// // readSentence(&initialSentence);

	if (argc > 1) {
		readTGF(argv[1]);
	} else {
		printf("%s (%d)\n","Please provide a file to play", argc);
	}

}