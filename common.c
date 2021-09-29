#include "common.h"

void myerr(char *errmsg) {
    char *err = strdup("ERROR: ");

    err = (char *) realloc(err, strlen(err) + strlen(errmsg) + 2);
    strcat(err, errmsg);
    err[strlen(err)] = '\n';
    err[strlen(err) + 1] = '\0';
    fprintf(stderr, "%s", err);

    free(err);
    
    exit(EXIT_FAILURE);
}