
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <entity.h>

#define bool char
#define true 1
#define false 0
#define and &&

#ifdef DEBUG_ON
	bool moo = true;
#else
	bool moo = false;
#endif

#define DEBUG if(moo)
#define NOT_DEBUG if(!moo)

struct Entity * initEntity(void * thing, char * name, EntityType type) {
	struct Entity * e = malloc(sizeof(struct Entity));
	e->type = type;
	e->thing = thing;
	e->name = (char *)malloc(sizeof(name) * sizeof(char));
	strcpy(e->name, name);
	return e;
}

void printEntity(struct Entity * e, int indent) {
	for(int i = 0; i < indent; i++)printf(" ");
	switch(e->type) {
		case INTEGER: printInteger(e);return;
		case STRING: printf("%s : %s\n", e->name, (char*)e->thing);return;
		case INSTANCE: printInstance(e, indent + 1);return;
		case ENTITY: printf("%s refers to:\n", e->name);printEntity(e->thing, indent + 1);return;

		case NULL_entity: printf("%s : %p (null type)\n", e->name, e->thing);
	}
	printf("%s : %p (unknown type)\n", e->name, e->thing);
}

void binitEntity(struct Entity * e) {
	DEBUG printf("Bining %s\n", e->name);
	if (e->type == INSTANCE) binitInstance(e);
	if (e->type == ENTITY) {
		binitEntity(e->thing);
	} else {
		free(e->thing);
	}
	free(e->name);
	free(e);
}

// Basic entities

struct Entity * initInteger(int i, char * name) {
	int * thing_pointer = malloc(sizeof(int));
	*thing_pointer = i;

	return initEntity(thing_pointer, name, INTEGER);
}

void changeInteger(struct Entity * e, int amount) {
	(*(int*)e->thing) += amount;
	printf("%s now %d\n", e->name, (*(int*)e->thing));
}

void printInteger(struct Entity * e) {
	printf("%s : %d\n", e->name, *(int*)e->thing);
}

struct Entity * initString(char * value, char * name) {
	char * thing_pointer = malloc(sizeof(value) * sizeof(char));
	strcpy(thing_pointer, value);

	return initEntity(thing_pointer, name, STRING);
}

struct Entity * initInstance(int number_of_variables, char * name) {
	number_of_variables += 1;

	struct Entity ** variables = malloc(sizeof(struct Entity *) * number_of_variables);

	for (int i = 0; i < number_of_variables; ++i) {
		struct Entity * var = malloc(sizeof(struct Entity));
		var->thing = 0;
		var->name = 0;
		variables[i] = var;
	}

	variables[0] = initInteger(number_of_variables, "n_variables");

	return initEntity(variables, name, INSTANCE);
}

struct Entity * getInstanceVariableByIndex(struct Entity * e, int i) {
	return ((struct Entity **)e->thing)[i];
}

void setInstanceVariableByIndex(struct Entity * e, struct Entity * var, int i) {
	((struct Entity **)e->thing)[i] = var;
}

struct Entity * getInstanceVariableByName(struct Entity * e, char * name) {
	int i = getInstanceVariableIndex(e, name);
	if (i >= 0) {
		return getInstanceVariableByIndex(e, i);
	} else {
		return 0;
	}
}

int getInstanceNumberOfVariables(struct Entity * e) {
	return *(int*)((struct Entity **)e->thing)[0]->thing;
}

int getInstanceVariableIndex(struct Entity * e, char * name) {
	int n = getInstanceNumberOfVariables(e);
	for (int i = 0; i < n; ++i) {
		struct Entity * var = getInstanceVariableByIndex(e, i);
		if (var->thing != 0 && strcmp(var->name, name) == 0) {
			return i;
		}
	}
	// DEBUG printf("WARNING: Could not find '%s' in '%s'\n", name, e->name);
	return -1;
}

char isPropertiesSubset(struct Entity * a, struct Entity * b) {
	// Does 'b' contain a set of Entities equal to a subset of 'a'
	if (a && b && a->type == INSTANCE && b->type == INSTANCE) {
		return 1;
	} else {
		DEBUG printf("WARNING: isPropertiesSubset(%p, %p) failed\n", a, b);
		return 0;
	}
}


char isEntitySimilar(struct Entity * a,  struct Entity * b) {
	return strcmp(a->name, b->name) == 0 && a->type == b->type;
}

char isEntityEqual(struct Entity * a, struct Entity * b) {
	if (isEntitySimilar(a, b)) {
		if (a->type == INSTANCE) {
			//Do the instances have the same number of variables?
			int n = getInstanceNumberOfVariables(a);

			char haveEqualNumberOfVariables = n == getInstanceNumberOfVariables(b);

			if (!haveEqualNumberOfVariables) return false;

			//For all var in a, is var in b?
			for (int i = 0; i < n; ++i) {
				struct Entity * var = getInstanceVariableByIndex(a, i);
				if (var && var->thing && !searchInstanceForRequirement(b, var)) {
					return false;
				}
			}
			return true;

		} else if (a->type == STRING) {
			// DEBUG printf("comparing %s with %s\n", (char*)a->thing, (char*)b->thing);
			return strcmp((char*)a->thing, (char*)b->thing) == 0;
		} else if (a->type == INTEGER) {
			return *(int*)a->thing == *(int*)b->thing;
		}
	}
	return 0;
}

char hasInstanceVariables(struct Entity * a) {
	int n = *(int*)((struct Entity **)a->thing)[0]->thing;
	for (int i = 1; i < n; ++i) {
		if (getInstanceVariableByIndex(a, i)->thing) return 1;
	}
	return 0;
}

struct Entity * searchInstanceForRequirement(struct Entity * haystack, struct Entity * needle) {
	// DEBUG printf("Looking for %s in %s\n", haystack->name, needle->name);
	int n = *(int*)((struct Entity **)haystack->thing)[0]->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * var = getInstanceVariableByIndex(haystack, i);

		if (var->thing != 0 && isEntityEqual(var, needle)) {
			return var;
		}
	}
	// DEBUG printf("WARNING: Could not find '%s' in '%s'\n", needle->name, haystack->name);
	return 0;
}

void printInstance(struct Entity * e, int indent) {
	printf("%s:\n", e->name);
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (a && a->thing) {
			printEntity(a, indent + 1);
		}
	}
	for(int i = 0; i < indent/2; i++)printf(" ");
	printf(":%s\n", e->name);
}

int firstAvailableIndex(struct Entity * e) {
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (!a || !a->thing) {
			return i;
		}
	}
	return -1;
}

void addVarToInstance(struct Entity * e, struct Entity * var) {
	int i = firstAvailableIndex(e);
	if (i != -1) {
		setInstanceVariableByIndex(e, var, i);
	} else {
		DEBUG printf("WARNING: addVarToInstance failed - no available slots\n");
	}
}

void removeVarFromInstance(struct Entity * e, char * name) {
	int i = getInstanceVariableIndex(e, name);
	struct Entity * var = getInstanceVariableByIndex(e, i);
	binitEntity(var);

	// Set the pointer to null at i
	setInstanceVariableByIndex(e, 0, i);
}

void binitInstance(struct Entity * e) {
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (a->thing) {
			binitEntity(a);
		}
	}
}