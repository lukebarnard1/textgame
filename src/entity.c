
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <entity.h>

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
		case NULL_entity: printf("%s : %p (null type)\n", e->name, e->thing);
	}
	printf("%s : %p (unknown type)\n", e->name, e->thing);
}

void binitEntity(struct Entity * e) {
	printf("Bining %s\n", e->name);
	if(e->type == INSTANCE) binitInstance(e);
	free(e->thing);
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
	return getInstanceVariableByIndex(e, getInstanceVariableIndex(e, name));
}

int getInstanceVariableIndex(struct Entity * e, char * name) {
	int n = *(int*)((struct Entity **)e->thing)[0]->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * var = getInstanceVariableByIndex(e, i);

		if (strcmp(var->name, name) == 0) {
			return i;
		}
	}
	return -1;
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

void addVarToInstance(struct Entity * e, struct Entity * var) {
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (!a->thing) {
			setInstanceVariableByIndex(e, var, i);
			return;
		}
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