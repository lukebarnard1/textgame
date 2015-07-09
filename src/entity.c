
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

