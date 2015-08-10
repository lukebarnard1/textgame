typedef enum {NULL_entity, INTEGER, STRING, INSTANCE, ENTITY} EntityType;

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
void changeInteger(struct Entity * e, int amount);
void printInteger(struct Entity * e);

struct Entity * initString(char * value, char * name);

struct Entity * initInstance(int number_of_variables, char * name);
void addVarToInstance(struct Entity * e, struct Entity * var);
int firstAvailableIndex(struct Entity * e);
void removeVarFromInstance(struct Entity * e, char * name);
int getInstanceVariableIndex(struct Entity * e, char * name);
struct Entity * searchInstanceForRequirement(struct Entity * haystack, struct Entity * needle);
struct Entity * getInstanceVariableByName(struct Entity * e, char * name);
char hasInstanceVariables(struct Entity * a);
void printInstance(struct Entity * e, int indent);
void binitInstance(struct Entity * e);