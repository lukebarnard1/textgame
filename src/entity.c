
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

struct Entity * getEntity(struct Entity * entity){
	return (struct Entity *)entity->thing;
} 

int getInteger(struct Entity * integer){
	return *(int*)integer->thing;
}

char * getString(struct Entity * string){
	return (char*)string->thing;
}

void printEntity(struct Entity * e, int indent) {
	for(int i = 0; i < indent; i++)printw(" ");
	switch(e->type) {
		case INTEGER: printInteger(e);return;
		case STRING: printw("%s : %s\n", e->name, getString(e));return;
		case INSTANCE: printInstance(e, indent + 1);return;
		case ENTITY: printw("%s refers to:\n", e->name);printEntity(getEntity(e), indent + 1);return;

		case NULL_entity: printw("%s : %p (null type)\n", e->name, e->thing);
	}
	printw("%s : %p (unknown type)\n", e->name, e->thing);
}

void binitEntity(struct Entity * e) {
	DEBUG printw("Bining %s\n", e->name);
	if (e->type == INSTANCE) binitInstance(e);
	if (e->type == ENTITY) {
		binitEntity(getEntity(e));
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
	printw("%s now %d\n", e->name, (*(int*)e->thing));
}

void printInteger(struct Entity * e) {
	printw("%s : %d\n", e->name, getInteger(e));
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
	// DEBUG printw("WARNING: Could not find '%s' in '%s'\n", name, e->name);
	return -1;
}

char isPropertiesSubset(struct Entity * a, struct Entity * b) {
	// Does 'b' contain a set of Entities equal to a subset of 'a'
	if (a && b && a->type == INSTANCE && b->type == INSTANCE) {
		return 1;
	} else {
		DEBUG printw("WARNING: isPropertiesSubset(%p, %p) failed\n", a, b);
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
			// DEBUG printw("comparing %s with %s\n", (char*)a->thing, (char*)b->thing);
			return strcmp((char*)a->thing, (char*)b->thing) == 0;
		} else if (a->type == INTEGER) {
			return *(int*)a->thing == *(int*)b->thing;
		}
	}
	return 0;
}

char hasInstanceVariables(struct Entity * a) {
	/**
	Returns true if an Instance has a variable that isn't null.
	**/
	int n = *(int*)((struct Entity **)a->thing)[0]->thing;
	for (int i = 1; i < n; ++i) {
		if (getInstanceVariableByIndex(a, i)->thing) return 1;
	}
	return 0;
}

struct Entity * searchInstanceForRequirement(struct Entity * haystack, struct Entity * needle) {
	/** Returns the entity equal to needle in haystack if
	it can be found. Equality is nested/deep. **/
	int n = *(int*)((struct Entity **)haystack->thing)[0]->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * var = getInstanceVariableByIndex(haystack, i);

		if (var->thing != 0 && isEntityEqual(var, needle)) {
			return var;
		}
	}
	return 0;
}

char canSetInstance(struct Entity * instance, struct Entity * instanceProps) {
	/** Returns true if all of the variables within instanceProps
	exist within instance once.**/
	int n = *(int*)((struct Entity **)instanceProps->thing)[0]->thing;

	for (int i = 1; i < n; ++i) {
		struct Entity * newValue = getInstanceVariableByIndex(instanceProps, i);

		if (newValue && newValue->thing) {
			struct Entity * oldValue = getInstanceVariableByName(instance, newValue->name);

			if (!oldValue) {
				return 0;
			}
		} else {
			break;
		}
	}
	return 1;
}

void setInstance(struct Entity * instance, struct Entity * instanceProps) {
	/** instance and instanceProps are both instances. It is assumed
	that instanceProps has a subset of variables that instance has **/
	int n = *(int*)getInstanceVariableByName(instanceProps, "n_variables")->thing;
	for (int i = 1; i < n; ++i) {
		struct Entity * newValue = getInstanceVariableByIndex(instanceProps, i);
		
		if (newValue && newValue->thing) {
			struct Entity * oldValue = getInstanceVariableByName(instance, newValue->name);

			if (oldValue) {
				free(oldValue->thing); // Destructive - prevent memory leak?

				oldValue->thing = newValue->thing;
			} else {
				// Couldn't find the variable to set - ignore (this should be prevented by canSetInstance)
			}
		} else {
			break; //No more variables to set in instanceProps
		}
	}
}

void printInstance(struct Entity * e, int indent) {
	printw("%s:\n", e->name);
	refresh();
	int n = *(int*)getInstanceVariableByName(e, "n_variables")->thing;
	for (int i = 0; i < n; ++i) {
		struct Entity * a = getInstanceVariableByIndex(e, i);
		if (a && a->thing) {
			printEntity(a, indent + 1);
			refresh();
		}
	}
	for(int i = 0; i < indent/2; i++)printw(" ");
	printw(":%s\n", e->name);
	refresh();
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
		DEBUG printw("WARNING: addVarToInstance failed - no available slots\n");
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