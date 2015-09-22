
#include <page.h>

struct ViewPage * initPage(int size) {
	struct ViewPage * vp = (struct ViewPage*)malloc(sizeof(struct ViewPage));
	vp->page = (char*)malloc(sizeof(char) * size);
	vp->size = size; // Bytes in page
	vp->filled = 0; // Bytes appended to page
	vp->prev = 0; //ViewPage
	vp->next = 0; //ViewPage
	return vp;
}

void binitPage(struct ViewPage * vp){
	if (!vp)return;
	free(vp->page);
	binitPage(vp->next);
	free(vp);
}

struct ViewPage * appendText(struct ViewPage * vp, char * format, ... ) {
	// Appends text to this view page, adds another page if necessary
	// and returns a pointer to the view page whose page was added to.
	if (!vp)return vp;

	char text[1024];

	va_list arguments;

	va_start(arguments, format);
	vsprintf(text, format, arguments);
	va_end(arguments);

	if(vp->next) {
		return appendText(vp->next, text);
	}

	int addedChars = strlen(text);
	int addedBytes = addedChars * sizeof(char);

	if (vp->filled + addedBytes > vp->size - 1) { // New page needed
		// Copy the last part of the page
		strcpy(vp->page + vp->filled, text);

		*(vp->page + vp->size - 1) = 0;

		struct ViewPage * np = (struct ViewPage*)malloc(sizeof(struct ViewPage));

		np->size = vp->size;
		np->filled = 0;
		np->page = (char*)malloc(sizeof(char) * np->size);
		np->prev = vp;// Back reference
		np->next = 0; // Null pointer

		vp->next = np;

		np = appendText(vp->next, text + vp->size - vp->filled - 1);

		vp->filled = vp->size;
		return np;
	} else { // Just append new text, nothing extra needed
		strcpy(vp->page + vp->filled, text);
		vp->filled += addedBytes;
		return vp;
	}
}
