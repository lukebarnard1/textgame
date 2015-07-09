struct Entity{
	void * thing;
	char * name;
};


void initEntity(struct Entity * e, void * thing, char * name);

void printEntity(struct Entity * e);

void binitEntity(struct Entity * e);


// Basic entities

void initInteger(struct Entity * e, int i, char * name);
void printInteger(struct Entity * e);
