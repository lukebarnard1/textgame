typedef enum {NULL_entity, INTEGER, STRING, INSTANCE} EntityType;

struct Entity{
	EntityType type;
	void * thing;
	char * name;
};



void initEntity(struct Entity * e, void * thing, char * name, EntityType type);

void printEntity(struct Entity * e, int indent);

void binitEntity(struct Entity * e);


// Basic entities

void initInteger(struct Entity * e, int i, char * name);
void printInteger(struct Entity * e);

void initInstance(struct Entity * e, int number_of_variables, char * name);
void addVarToInstance(struct Entity * e, struct Entity * var);
void printInstance(struct Entity * e, int indent);
void binitInstance(struct Entity * e);