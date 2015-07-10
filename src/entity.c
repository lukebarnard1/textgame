
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <entity.h>

void initEntity(struct Entity * e, void * thing, char * name) {
	e->thing = thing;
	e->name = (char *)malloc(sizeof(name) * sizeof(char));
	strcpy(e->name, name);
}

void printEntity(struct Entity * e) {
	printf("%s : %p\n", e->name, e->thing);
}

void binitEntity(struct Entity * e) {
	printf("Bining %s\n", e->name);
	free(e->thing);
	free(e->name);
}

// Basic entities

void initInteger(struct Entity * e, int i, char * name) {
	int * thing_pointer = malloc(sizeof(int));
	*thing_pointer = i;

	initEntity(e, thing_pointer, name);
}

void printInteger(struct Entity * e) {
	printf("%s : %d\n", e->name, *(int*)e->thing);
}


void initInstance(struct Entity * e, int number_of_variables,char * name) {
	number_of_variables += 1;

	struct Entity ** variables = malloc(sizeof(struct Entity *) * number_of_variables);

	for (int i = 0; i < number_of_variables; ++i) {
		struct Entity * var = malloc(sizeof(struct Entity));
		var->thing = 0;
		var->name = 0;
		variables[i] = var;
	}

	initInteger(variables[0], number_of_variables, "n_variables");

	initEntity(e, variables, name);
}

struct Entity * getInstanceVariableByIndex(struct Entity * e, int i) {
	return ((struct Entity **)e->thing)[i];
}

void setInstanceVariableByIndex(struct Entity * e, struct Entity * var, int i) {
	((struct Entity **)e->thing)[i] = var;
}

struct Entity * getInstanceVariableByName(struct Entity * e, char * name) {
	int n = *(int*)((struct Entity **)e->thing)[0]->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * var = getInstanceVariableByIndex(e, i);

		if (strcmp(var->name, name) == 0) {
			return var;
		}
	}
	return 0;
}

void printInstance(struct Entity * e) {
	printf("%s:\n", e->name);
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (a->thing) {
			printf("\t%s : %d\n", a->name, *(int*)a->thing);
		}
	}
}

void addVarToInstance(struct Entity * e, struct Entity * var) {
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (!a->thing) {
			printf("Adding %s\n", var->name);
			setInstanceVariableByIndex(e, var, i + 1);
			return;
		}
	}
}

void binitInstance(struct Entity * e) {
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (a->thing) {
			binitEntity(a);
		}
	}
	binitEntity(e);
}