#ifndef PGMIMAGE_H
#define PGMIMAGE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>

typedef struct {
  char *name;
  int rows, cols;
  int *data;
} IMAGE;

typedef struct {
  int n;
  IMAGE **list;
} IMAGELIST;

/*** User accessible macros ***/

#define ROWS(img)  ((img)->rows)
#define COLS(img)  ((img)->cols)
#define NAME(img)   ((img)->name)

/*** User accessible functions ***/

IMAGE *img_open(char *);
IMAGE *img_creat(char *, int, int);
void img_setpixel(IMAGE *, int, int, int);
int img_getpixel(IMAGE *, int, int);
int img_write(IMAGE *, char *);
void img_free(IMAGE *);

void imgl_load_images_from_textfile(IMAGELIST *, char *);
void imgl_load_images_from_textfile(IMAGELIST *, char *, std::map<std::string, int> *);

IMAGELIST *imgl_alloc();
void imgl_add(IMAGELIST *, IMAGE *);
void imgl_free(IMAGELIST *);
std::map<std::string, int> * imgl_get_map_userId(char *, std::map<std::string, int> *);

#endif // PGMIMAGE_H
