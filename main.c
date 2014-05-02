

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
	printf("%s\n", "ADD OPTION");
	struct Option * cursor = &s->options, * prev;
	prev = cursor;
	while (cursor != 0) {prev = cursor; cursor = cursor->next;printf("%p\n", cursor);}// Get to the end the options
	prev->next = o;
	printf("!!!!\n");
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
		printf("%d\n", cursor->line);
		while(cursor != 0){
			printf("\t\tOption %d:\t%s\n", cursor->line, cursor->command);
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
			return cursor->line; // Return this choice
		}
		cursor = cursor->next;
	}
	return 0; // Tried every option and didn't find one equal
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

// void readSentence(struct Sentence * s) {
// 	if (s == 0){
// 		DEBUG printf("%s\n", "END OF THE STORY");
// 		return;
// 	}

// 	struct Tempo typical;
// 	typical.delay = 100;
// 	typical.jitter = 1;

// 	if (s->tempo == 0) {
// 		s->tempo = &typical;
// 	}

// 	char * cursor = s->text, * wordStart;
// 	int jitter = 0;
// 	wordStart = cursor;

// 	while (*cursor != 0) {
// 		while (*cursor != ' ' && *cursor != 0) {
// 			cursor++;
// 			//Move forward to first space
// 		}
// 		//Print between wordStart and cursor
// 		while (wordStart < cursor) {
// 			putchar(*wordStart);
// 			wordStart++;
// 		}
// 		fflush(stdout);
// 		if (*cursor) {cursor++;} else {break;}
		
// 		double d = ((double)rand()/(double)RAND_MAX);
// 		jitter = s->tempo->jitter * d * 1000;

// 		usleep(s->tempo->delay * 1000 + jitter);
// 	}

// 	putchar('\n');
// 	if (s->decision) {
// 		struct Option * cursor = s->options.next;
// 		int i = 1;
// 		while(cursor != 0){
// 			printf("\t%d: %s", i, cursor->command);
// 			cursor = cursor->next;
// 			i++;
// 		}
// 		putchar('\n');

// 		int max = 10; //Max size of commands is here
// 		char * command = malloc(sizeof(char) * 10);
// 		struct Sentence * choice = 0;

// 		// Loop until a correct command is typed in
// 		do{
// 			printf("%s\n", "What is your decision?");
// 			fgets(command, max, stdin);

// 			trimNewline(command);

// 		} while ((choice = selectChoice(&s->options ,command)) == 0);
// 		printf("You chose %s...\n\n", command);
		
// 		free(command);
// 		readSentence(choice);
// 	} else {
// 		readSentence(s->options.sentence);
// 	}
// }

// // Read an array of strings as the lines of a TGF file and return start sentence
// // For this block at least
// struct Sentence * readLines(char *** lines, int offset, int indent) {
// 	printf("readlines\n");
// 	// lineCursor is now at the start of  ** lines
// 	char ** lineCursor = *lines + offset;
// 	DEBUG for (int i = 0; i < indent; i++)printf("\t");
// 	DEBUG printf("%s '%s'\n", "READING",lineCursor[0] + indent);

// 	struct Sentence * s = malloc(sizeof(struct Sentence));

// 	if (lineCursor[0][indent] == '>') {
// 		int jump;
// 		sscanf(lineCursor[0] + indent, ">%d", &jump);
// 		int i = 0;
// 		while (lineCursor[i] && lineCursor[i][indent - 1] == '\t') {
// 			i++;
// 		}
// 		return readLines(lines, offset + i, indent - 1);
// 	}
// 	if (lineCursor[1]) {
// 		if (lineCursor[1][indent] == '\t') {
// 			initDecision(s, lineCursor[0] + indent, 0);// For now, 0 is default tempo
// 			int i = 1;
// 			while(lineCursor[i] && lineCursor[i][indent + 1] != '|') {
// 				DEBUG printf("Option %d: %s\n", i, lineCursor[i]);
// 				struct Option * o = malloc(sizeof(struct Option));

// 				sscanf(lineCursor[i], "%d", &o->link);
// 				// DEBUG printf("link %d\n", o->link);

// 				o->command = lineCursor[i] + 3 + indent;
// 				addOption(s, o);
// 				i++;
// 			}
// 			//No more options, now they need to be populated with their link sentences

// 			char ** decisionLinks = lineCursor + 1;

// 			struct Option * option = s->options.next;
// 			while(option != 0) {
// 				for (int i = 0; i < indent; i++)printf("\t");
// 				DEBUG printf("Option with link %d:\n", option->link);
// 				int linkFound = 0;
// 				while (decisionLinks[i] && decisionLinks[i][indent] == '\t') {
// 					sscanf(decisionLinks[i], "%d", &linkFound);

// 					if (linkFound == option->link) {
// 						DEBUG for (int i = 0; i < indent; i++)printf("\t");
// 						DEBUG printf("Link found %d\n", linkFound);
// 						option->sentence = readLines(lines,  offset + i + 1, indent + 1);
// 						break;
// 					}
// 					i++;
// 				}
// 				option = option->next;
// 			}

// 		} else {
// 			DEBUG for (int i = 0; i < indent; i++)printf("\t");
// 			DEBUG printf("%s\n", "NEXT LINE");
// 			struct Sentence * nextSentence = readLines(lines, offset + 1, indent);

// 			DEBUG printf(" %p\n", nextSentence);
// 			initSentence(s, lineCursor[0] + indent, 0, nextSentence);
// 		}
// 	} else { // End of Lines
// 		initSentence(s, lineCursor[0] + indent, 0, 0);
// 		return s;
// 	}
// 	return s;
// }

void readLinesFlat(char *** lines, int numLines) {
	// Sentences will just be stored in a massive array as they are in lines

	char ** lineCursor = *lines;

	struct Sentence sentences[numLines];

	int sentenceIndex = 0;

	while (sentenceIndex < numLines) {
		if (lineCursor[sentenceIndex + 1][0] == '\t') { // Indent on the next line == decision
			initDecision(sentences + sentenceIndex, lineCursor[sentenceIndex], 0);
			printf("Decision\n");
			sentenceIndex++;
			while (lineCursor[sentenceIndex][0] == '\t') {
				struct Option * o = malloc(sizeof(struct Option));
				o->next = 0;
				printf("%p\n",&o->command);
				//Option format:
				sscanf("\t%d", lineCursor[sentenceIndex], &o->line);

				char * space = &lineCursor[sentenceIndex][1];
				while (*(space++) != ' ');

				o->command = space;

				addOption(sentences + sentenceIndex, o);
				printf("Option added\n");
				sentenceIndex++;
			}
			printSentence(sentences + sentenceIndex);
		} else {
			initSentence(sentences + sentenceIndex, lineCursor[0], 0);
			printSentence(sentences + sentenceIndex);
			printf("%s\n","next");
			sentenceIndex++;
		}
	}
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

	readTGF("test1.tgf");

}