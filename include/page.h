
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

struct ViewPage {
	char * page;
	int size; // Bytes in page
	int filled; // Bytes appended to page
	void * prev; //ViewPage
	void * next; //ViewPage
};

struct ViewPage * initPage(int size);
void binitPage(struct ViewPage * vp);
struct ViewPage * appendText(struct ViewPage * vp, char * format, ... );
