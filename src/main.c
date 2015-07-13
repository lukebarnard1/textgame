#include <main.h>

// To compile, make build

#define bool char
#define true 1
#define false 0
#define and &&

#ifdef DEBUG_ON
	bool debug = true;
#else
	bool debug = false;
#endif

#define DEBUG if(debug)


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

char * findNext(char * s, char c) {
	while (*s != c && *s != 0) s++;
	return s;
}

char * firstWordEq(char * s, char * first) {
	while (*s == *first && *s != 0) {
		s++;
		first++;
		if (*first == 0) return s;
	}
	return 0;
}

int parseQuantityDigit(char d) {
	int intDigit = d - 48;
	if (intDigit > 0 && intDigit < 10) {
		return intDigit;
	} else {
		return 0;
	}
}

int parseQuantity(char * strquantity) {
	int total = 0;
	int power = 0;
	char * cursor = strquantity;
	while (*cursor++) power++;
	while (power > 0 && *strquantity) {
		total += parseQuantityDigit(*strquantity) * pow(10, power - 1);
		power--;
		strquantity++;
	}
	return total;
}

char * parseAdd(char * cursor) {
	cursor++;

	char * itemName = cursor;

	char *endOfItemName = findNext(cursor, ' ');
	*endOfItemName = 0;

	cursor = endOfItemName + 1;

	char * properties = firstWordEq(cursor, "with");

	if (properties) {
		properties++;

		int remaining = 16;
		struct Entity * newItem = initInstance(remaining ,itemName); // Maximum 16 properties

		while (*properties and remaining > 0) {
			cursor = properties;
			char * propName = cursor;

			char *endOfPropName = findNext(cursor, ' ');
			*endOfPropName = 0;

			cursor = endOfPropName + 1;

			char * propValue = cursor;

			char *endOfPropValue = findNext(cursor, ' ');
			*endOfPropValue = 0;

			struct Entity * prop = initInteger(parseQuantity(propValue), propName);

			addVarToInstance(newItem, prop);

			cursor = endOfPropValue + 1;
			properties = cursor;

			usleep(100000);

			remaining--;
		}

		printEntity(newItem, 0);
	}
	return cursor;
}

char * parseRule(char * rule_text, struct Sentence * activating_sentence) {
	char * cursor = firstWordEq(rule_text, "add");
	if (cursor) return parseAdd(cursor);

	DEBUG if (*cursor != 0) printf("WARNING: Rule not parsed correctly. Remaining: %s\n", cursor);
	return cursor;
}

// Initialise a sentence without a decision
void initSentence(struct Sentence * s, char * text, struct Tempo * tempo) {
	s->text = text;
	s->tempo = tempo;
	s->decision = 0;

	char * cursor = findNext(text, '[');

	while (*cursor == '[') {
		// Start parsing a rule

		char * endOfRule = findNext(cursor, ']');
		*cursor = 0;
		*endOfRule = 0;

		char * rule = cursor + 1;
		printf("Rule: %s\n", rule);

		cursor = parseRule(rule, s);
	}
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

int readSentence(struct Sentence * s, int current) {
	if (s == 0){
		DEBUG printf("%s\n", "END OF THE STORY");
		return -1;
	}

	s = s + current;

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

			DEBUG printSentence(sentences + sentenceIndex);

			sentenceIndex = optionsIndex;
		} else {
			initSentence(sentences + sentenceIndex, lineCursor[sentenceIndex], 0);
			numSentences++;

			DEBUG printSentence(sentences + sentenceIndex);

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
	
	lines[numLines] = 0;
	readLinesFlat(&lines, numLines);

	free(lines);
	free(fileBuffer);
}

int main(int argc, char ** argv) {
	if (argc > 1) {
		readTGF(argv[1]);
	} else {
		printf("%s (%d)\n","Please provide a file to play", argc);
	}
}