#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAXBUF 256
#define MAXNAM 260

/* Get a string from the user.
 */
char *get_string(const char *prompt)
{
    static char str[MAXBUF];
    int c, i;

    // Prompt user for input and get input.
    memset(str, 0, MAXBUF);
    printf("%s", prompt);
    for(i = 0; i < MAXBUF && (c = getc(stdin)) != EOF && c != '\n'; ) {
        if(i > 0 && c == '\b')
            str[--i] = '\0';
        if(iscntrl(c))
            continue;
        str[i++] = c;
    }
    str[i] = '\0';

    // Make sure that the character count isn't above the buffer size.
    if(i >= MAXBUF) {
        printf("Only %d max characters are allowed in the buffer.\n", MAXBUF);
        return NULL;
    }

    return str;
}
int main(int argc, char **argv)
{
	int i;
	char fname[MAXNAM];
	FILE *fp;
	char *tmp;
	char c;

	do {
		tmp = get_string("What is the filename [Makefile]? ");
	} while(tmp == NULL);
	if(tmp != NULL) {
		strncpy(fname, tmp, sizeof(fname)-1);
	}

	if((fp = fopen(fname, "w")) == NULL) {
		printf("Error: Cannot open file for writing.\n");
		return -1;
	}

	do {
		tmp = get_string("What compiler? ");
	} while(tmp == NULL);
	fprintf(fp, "CC = %s\n", tmp);

	do {
		tmp = get_string("What C flags? ");
	} while(tmp == NULL);
	fprintf(fp, "CFLAGS = %s\n", tmp);

	do {
		tmp = get_string("What linker flags? ");
	} while(tmp == NULL);
	fprintf(fp, "LDFLAGS = %s\n\nDESTDIR ?= \nPREFIX ?= /usr\n\n", tmp);

	i = 0;
	do {
		do {
			tmp = get_string("What is the program name? ");
		} while(tmp == NULL);
		fprintf(fp, "TARGET%u = %s\nOBJECT%u = $(SOURCE%u:%%.c=%%.c.o)\nSOURCE%u =", i, tmp, i, i, i);

		do {
			do {
				tmp = get_string("Compilation unit name? ");
			} while(tmp == NULL);
			if((int)strlen(tmp) < (MAXNAM - 3)) {
				strncat(tmp, ".c", 3);
				fprintf(fp, " %s", tmp);
			}

			printf("Add another compilation unit (Y/N)? ");
			c = getc(stdin);
			getc(stdin);
		} while(c == 'y' || c == 'Y');
		fprintf(fp, "\n");

		printf("Do you want another program (Y/N)? ");
		c = getc(stdin);
		getc(stdin);

		++i;
	} while(c == 'y' || c == 'Y');

	fprintf(fp, "\nall:");
	for(int k = 0; k < i; k++) {
		fprintf(fp, " $(TARGET%u)", k);
	}
	fprintf(fp, "\n\nclean:\n\trm -f *.o\n\ndistclean: clean\n\trm -f");
	for(int k = 0; k < i; k++) {
		fprintf(fp, " $(TARGET%u)", k);
	}

	fprintf(fp, "\n\ndist: distclean\n\ttar cvf ../$(DIRNAME)-latest.txz ../$(DIRNAME)\n\n");
	fprintf(fp, "install:");
	for(int k = 0; k < i; k++) {
		fprintf(fp, " $(TARGET%u)", k);
	}
	fprintf(fp, "\n\tcp");
	for(int k = 0; k < i; k++) {
		fprintf(fp, " $(TARGET%u)", k);
	}
	fprintf(fp, " $(DESTDIR)$(PREFIX)/bin\n\nuninstall:\n\trm -f");
	for(int k = 0; k < i; k++) {
		fprintf(fp, " $(TARGET%u)", k);
	}
	for(int k = 0; k < i; k++) {
		fprintf(fp, "\n\n$(TARGET%u): $(OBJECT%u)\n\t$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)\n\n", k, k);
	}
	fprintf(fp, "%%.c.o: %%.c\n\t$(CC) $(CFLAGS) -c -o $@ $<\n\n");

	printf("File '%s' was written.\n", fname);
	fclose(fp);

	return 0;
}
