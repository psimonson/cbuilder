/*
 * cbuilder.c - Source for a simple C source/header builder.
 *
 * Author: Philip R. Simonson
 * Date  : 07/07/2021
 *
 ***************************************************************************
 */

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
/* Make the comment header for a C source/header file.
 */
void make_header(FILE *fp, const char *name)
{
    struct tm *tm;
    time_t now;
    char desc[MAXBUF];
    char author[MAXBUF];
    int i = 2;

    // Get current date and time.
    now = time(NULL);
    tm = localtime(&now);

    // Get all user input.
    while(i > 0) {
        char *tmp;

        // Get input from user and ignore lines to big.
        do {
            char buf[64];
            snprintf(buf, sizeof(buf), "%s is? ",
                i == 2 ? "Description" : "Author");
            tmp = get_string(buf);
        } while(tmp == NULL);

        // Copy the data
        if(i == 2)
            strncpy(desc, tmp, sizeof(desc));
        else
            strncpy(author, tmp, sizeof(author));

        i--;
    }

    // Write the comment header to the file.
    fprintf(fp, "/*\n * %s - %s.\n", name, desc);
    fprintf(fp, " *\n * Author: %s\n * Date:   %.2d/%.2d/%.4d\n *\n", author,
        tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900);
    fprintf(fp, " */\n\n");
}
/* Make includes from user.
 */
void make_includes(FILE *fp)
{
    char *tmp;
    char c;

    printf("Do you want any includes (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
        int i = 0;
        do {
            if(i == 0)
                printf("Do you want any standard includes (Y/N)? ");
            else
                printf("Do you want any user includes (Y/N)? ");

            c = getc(stdin);
            getc(stdin);
            if(c == 'y' || c == 'Y') {
                do {
                    do {
                        tmp = get_string("What do you want included? ");
                    } while(tmp == NULL);

                    if(i == 0)
                        fprintf(fp, "#include <%s.h>\n", tmp);
                    else
                        fprintf(fp, "#include \"%s.h\"\n", tmp);

                    printf("Do you want another library include (Y/N)? ");
                    c = getc(stdin);
                    getc(stdin);
                } while(c == 'y' || c == 'Y');
            }
            ++i;
        } while(i < 2);
        fprintf(fp, "\n");
    }
}
/* Make defines from user.
 */
void make_defines(FILE *fp)
{
    char *tmp;
    char c;

    printf("Do you want any defines (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
	do {
       		do {
			tmp = get_string("Define what? ");
           	} while(tmp == NULL);
		fprintf(fp, "#define %s ", tmp);

                do {
                	tmp = get_string("Define data? ");
                } while(tmp == NULL);
		fprintf(fp, "%s\n", tmp);

                printf("Do you want another define (Y/N)? ");
                c = getc(stdin);
                getc(stdin);
	} while(c == 'y' || c == 'Y');
	fprintf(fp, "\n");
    }
}
/* Make a function definition.
 */
void make_functions(FILE *fp)
{
    char *tmp;
    char c;

    printf("Would you like any functions (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
        do {
            char funcdef[MAXBUF];
            char buf[32];

            do {
                tmp = get_string("What is the function definition?\n");
            } while(tmp == NULL);
            strncpy(funcdef, tmp, sizeof(funcdef));

            if(sscanf(funcdef, "%31s", buf) != 1) {
                fprintf(stderr, "Warning: Cannot get function return type.\n");
            }
            else {
                printf("Would you like to add a comment (Y/N)? ");
                c = getc(stdin);
                getc(stdin);

                if(c == 'y' || c == 'Y') {
                    do {
                        tmp = get_string("Enter a comment:\n");
                    } while(tmp == NULL);
                    fprintf(fp, "/* %s.\n */\n", tmp);
                }

                fprintf(fp, "%s\n{\n%s}\n", funcdef,
                    !strcmp(buf, "void") ? "" : "\treturn 0;\n");
            }

            printf("Would you like another function (Y/N)? ");
            c = getc(stdin);
            getc(stdin);
        } while(c == 'y' || c == 'Y');
    }
}
/* Make a function prototype.
 */
void make_prototypes(FILE *fp)
{
    char *tmp;
    char c;

    printf("Would you like any prototypes (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
        do {
            do {
                tmp = get_string("Define a function prototype:\n");
            } while(tmp == NULL);
            fprintf(fp, "%s;\n", tmp);

            printf("Do you want to add another (Y/N)? ");
            c = getc(stdin);
            getc(stdin);
        } while(c == 'y' || c == 'Y');
        fprintf(fp, "\n");
    }
}
/* Make an enumeration.
 */
void make_enumerations(FILE *fp)
{
    char *tmp;
    char c;

    printf("Would you like any enumerations (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
        do {
            do {
                tmp = get_string("What is the enumeration name? ");
            } while(tmp == NULL);
            fprintf(fp, "enum %s {\n", tmp);

            do {
                do {
                    tmp = get_string("What to define? ");
                } while(tmp == NULL);
                fprintf(fp, "\t%s", tmp);

                printf("Do you want another define (Y/N)? ");
                c = getc(stdin);
                getc(stdin);

		if(c == 'y' || c == 'Y') {
			fprintf(fp, ",\n");
		}
            } while(c == 'y' || c == 'Y');
            fprintf(fp, "\n};\n");

            printf("Do you want another enumeration (Y/N)? ");
            c = getc(stdin);
            getc(stdin);
        } while(c == 'y' || c == 'Y');
        fprintf(fp, "\n");
    }
}
/* Make a structure.
 */
void make_structures(FILE *fp)
{
    char *tmp;
    char c;

    printf("Would you like any structures (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
        do {
            do {
                tmp = get_string("What is the structure name? ");
            } while(tmp == NULL);
            fprintf(fp, "struct %s {\n", tmp);

            do {
                do {
                    tmp = get_string("What variable to declare? ");
                } while(tmp == NULL);
                fprintf(fp, "\t%s;\n", tmp);

                printf("Do you want another variable (Y/N)? ");
                c = getc(stdin);
                getc(stdin);
            } while(c == 'y' || c == 'Y');
            fprintf(fp, "};\n");

            printf("Do you want another structure (Y/N)? ");
            c = getc(stdin);
            getc(stdin);
        } while(c == 'y' || c == 'Y');
        fprintf(fp, "\n");
    }
}
/* Make a external variable.
 */
void make_externals(FILE *fp)
{
    char *tmp;
    char c;

    printf("Would you like any external variables (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
        do {
            do {
                tmp = get_string("What is the external variable? ");
            } while(tmp == NULL);
            fprintf(fp, "%s;\n", tmp);

            printf("Do you want another external variable (Y/N)? ");
            c = getc(stdin);
            getc(stdin);
        } while(c == 'y' || c == 'Y');
        fprintf(fp, "\n");
    }
}
/* Make a typedef declaration.
 */
void make_typedefs(FILE *fp)
{
    char *tmp;
    char c;

    printf("Would you like any typedefs (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
        do {
            do {
                tmp = get_string("What is the typedef? ");
            } while(tmp == NULL);
            fprintf(fp, "typedef %s;\n", tmp);

            printf("Do you want another typedef (Y/N)? ");
            c = getc(stdin);
            getc(stdin);
        } while(c == 'y' || c == 'Y');
        fprintf(fp, "\n");
    }
}
/* Build a C source file.
 */
void build_source(const char *filename)
{
    char buf[MAXNAM];
    FILE *fp;

    // Build filename.
    strncpy(buf, filename, sizeof(buf));
    strncat(buf, ".c", 3);

    if((fp = fopen(buf, "wt")) == NULL) {
        perror("fopen");
        return;
    }

    // Build header for file.
    make_header(fp, filename);
    make_defines(fp);
    make_includes(fp);
    make_defines(fp);
    make_enumerations(fp);
    make_structures(fp);
    make_typedefs(fp);
    make_externals(fp);
    make_prototypes(fp);
    make_functions(fp);

    // Cleanup
    fclose(fp);
}
/* Build a C header file.
 */
void build_header(const char *filename)
{
    char buf[MAXNAM];
    FILE *fp;

    // Build filename.
    strncpy(buf, filename, sizeof(buf));
    strncat(buf, ".h", 3);

    if((fp = fopen(buf, "wt")) == NULL) {
        perror("fopen");
        return;
    }

    // Build header for file.
    make_header(fp, filename);
    make_defines(fp);
    make_includes(fp);
    make_defines(fp);
    make_enumerations(fp);
    make_structures(fp);
    make_typedefs(fp);
    make_externals(fp);
    make_prototypes(fp);
    make_functions(fp);

    // Cleanup
    fclose(fp);
}
/* Entry point for program.
 */
int main(void)
{
    char filename[MAXNAM];
    char *tmp;
    char c;

    // Get filename from user.
    do {
        tmp = get_string("Enter filename (without extension):\n");
    } while(tmp == NULL);

    // Copy file name into buffer.
    strncpy(filename, tmp, sizeof(filename));

    printf("Do you want to make a source file (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
	    build_source(filename);
    }

    printf("Do you want to make a header file (Y/N)? ");
    c = getc(stdin);
    getc(stdin);

    if(c == 'y' || c == 'Y') {
    	build_header(filename);
    }
}
