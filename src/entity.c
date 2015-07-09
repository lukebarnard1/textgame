
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
