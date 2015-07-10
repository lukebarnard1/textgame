typedef enum {NULL_entity, INTEGER, STRING, INSTANCE} EntityType;

struct Entity{
	EntityType type;
	void * thing;
	char * name;
};

struct Entity * initEntity(void * thing, char * name, EntityType type);

void printEntity(struct Entity * e, int indent);

void binitEntity(struct Entity * e);


// Basic entities

struct Entity * initInteger(int i, char * name);
void printInteger(struct Entity * e);

struct Entity * initInstance(int number_of_variables, char * name);
void addVarToInstance(struct Entity * e, struct Entity * var);
void printInstance(struct Entity * e, int indent);
void binitInstance(struct Entity * e);