#include "pgmimage.h"
#include "dir.h"

#define MAXLISTBUF 20000
// 数据目录
#define DATA "data"
#define IMAGESREC "/images/imagesRec/"

extern void *malloc();
extern void *realloc();
//extern char *strcpy();

char *img_basename(char *filename)
{
  char *p_new, *part;
  int len, dex;

  len = (int)strlen(filename);  dex = len - 1;
  while (dex > -1) {
    if (filename[dex] == '/') {
      break;
    } else {
      dex--;
    }
  }
  dex++;
  part = &(filename[dex]);
  len = (int)strlen(part);
  p_new = (char *) malloc ((unsigned) ((len + 1) * sizeof (char)));
  strcpy(p_new, part);
  return(p_new);
}


IMAGE *img_alloc()
{
  IMAGE *p_new;

  p_new = (IMAGE *) malloc (sizeof (IMAGE));
  if (p_new == NULL) {
    printf("IMGALLOC: Couldn't allocate image structure\n");
    return (NULL);
  }
  p_new->rows = 0;
  p_new->cols = 0;
  p_new->name = NULL;
  p_new->data = NULL;
  return (p_new);
}


void img_free(IMAGE *img)
{
  if (img->data) free ((char *) img->data);
  if (img->name) free ((char *) img->name);
  free ((char *) img);
}


void img_setpixel(IMAGE *img, int r, int c, int val)
{
  int nc;

  nc = img->cols;
  img->data[(r * nc) + c] = val;
}


int img_getpixel(IMAGE *img, int r, int c)
{
  int nc;

  nc = img->cols;
  return (img->data[(r * nc) + c]);
}


IMAGE *img_creat(char *name, int nr, int nc)
{
  int i, j;
  IMAGE *p_new;

  p_new = img_alloc();
  p_new->data = (int *) malloc ((unsigned) (nr * nc * sizeof(int)));
  p_new->name = img_basename(name);
  p_new->rows = nr;
  p_new->cols = nc;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
      img_setpixel(p_new, i, j, 0);
    }
  }
  return (p_new);
}


IMAGE *img_open(char *filename)
{
  IMAGE *p_new;
  FILE *pgm;
  char line[512], intbuf[100], ch;
  int type, nc, nr, maxval, i, j, k, found;

  p_new = img_alloc();
  if ((pgm = fopen(filename, "r")) == NULL) {
    printf("IMGOPEN: Couldn't open '%s'\n", filename);
    return(NULL);
  }

  p_new->name = img_basename(filename);

  /*** Scan pnm type information, expecting P5 ***/
  fgets(line, 511, pgm);
  sscanf(line, "P%d", &type);
  if (type != 5 && type != 2) {
    printf("IMGOPEN: Only handles pgm files (type P5 or P2)\n");
    fclose(pgm);
    return(NULL);
  }

  /*** Get dimensions of pgm ***/
  fgets(line, 511, pgm);
  sscanf(line, "%d %d", &nc, &nr);
  p_new->rows = nr;
  p_new->cols = nc;

  /*** Get maxval ***/
  fgets(line, 511, pgm);
  sscanf(line, "%d", &maxval);
  if (maxval > 255) {
    printf("IMGOPEN: Only handles pgm files of 8 bits or less\n");
    fclose(pgm);
    return(NULL);
  }

  p_new->data = (int *) malloc ((unsigned) (nr * nc * sizeof(int)));
  if (p_new->data == NULL) {
    printf("IMGOPEN: Couldn't allocate space for image data\n");
    fclose(pgm);
    return(NULL);
  }

  if (type == 5) {

    for (i = 0; i < nr; i++) {
      for (j = 0; j < nc; j++) {
        img_setpixel(p_new, i, j, fgetc(pgm));
      }
    }

  } else if (type == 2) {

    for (i = 0; i < nr; i++) {
      for (j = 0; j < nc; j++) {

        k = 0;  found = 0;
        while (!found) {
          ch = (char) fgetc(pgm);
          if (ch >= '0' && ch <= '9') {
            intbuf[k] = ch;  k++;
      } else {
            if (k != 0) {
              intbuf[k] = '\0';
              found = 1;
        }
      }
    }
        img_setpixel(p_new, i, j, atoi(intbuf));

      }
    }

  } else {
    printf("IMGOPEN: Fatal impossible error\n");
    fclose(pgm);
    return (NULL);
  }

  fclose(pgm);
  return (p_new);
}


int img_write(IMAGE *img, char *filename)
{
  int i, j, nr, nc, k, val;
  FILE *iop;

  nr = img->rows;  nc = img->cols;
  iop = fopen(filename, "w");
  fprintf(iop, "P2\n");
  fprintf(iop, "%d %d\n", nc, nr);
  fprintf(iop, "255\n");

  k = 1;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
      val = img_getpixel(img, i, j);
      if ((val < 0) || (val > 255)) {
        printf("IMG_WRITE: Found value %d at row %d col %d\n", val, i, j);
        printf("           Setting it to zero\n");
        val = 0;
      }
      if (k % 10) {
        fprintf(iop, "%d ", val);
      } else {
        fprintf(iop, "%d\n", val);
      }
      k++;
    }
  }
  fprintf(iop, "\n");
  fclose(iop);
  return (1);
}


IMAGELIST *imgl_alloc()
{
  IMAGELIST *p_new;

  p_new = (IMAGELIST *) malloc (sizeof (IMAGELIST));
  if (p_new == NULL) {
    printf("IMGL_ALLOC: Couldn't allocate image list\n");
    return(NULL);
  }

  p_new->n = 0;
  p_new->list = NULL;

  return (p_new);
}


void imgl_add(IMAGELIST *il, IMAGE *img)
{
  int n;

  n = il->n;

  if (n == 0) {
    il->list = (IMAGE **) malloc ((unsigned) (sizeof (IMAGE *)));
  } else {
    il->list = (IMAGE **) realloc ((char *) il->list,
      (unsigned) ((n+1) * sizeof (IMAGE *)));
  }

  if (il->list == NULL) {
    printf("IMGL_ADD: Couldn't reallocate image list\n");
  }

  il->list[n] = img;
  il->n = n+1;
}


void imgl_free(IMAGELIST *il)
{
  free((char *) il->list);
  free((char *) il);
}


int imgl_munge_name(char *buf)
{
  int j;

  j = 0;
  while (buf[j] != '\n') j++;
  buf[j] = '\0';
  return 0;
}


void imgl_load_images_from_textfile(IMAGELIST *il, char *filename)
{
  IMAGE *iimg;
  FILE *fp;
  char buf[MAXLISTBUF];  // 这个字符数组长度一定要长，因为它接受的list文件流长度可能会很长

  if (filename[0] == '\0') {
    printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Invalid file '%s'\n", filename);
    return;
  }

  if ((fp = fopen(filename, "r")) == NULL) {
    printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Couldn't open '%s'\n", filename);
    return;
  }

  // Set the data path
  QDir dir = programRootDir();
  dir.cd(DATA);
  QString data_path = dir.absolutePath();

  std::string str_data_path = data_path.toStdString();

  while (fgets(buf, 19999, fp) != NULL) {

    // Set the ...data/images/xxx.png path
    std::string s_path = str_data_path + "/" + buf;
    char * c_path = (char *)s_path.c_str();

    imgl_munge_name(c_path);
    printf("Loading '%s'...\n", c_path);
    fflush(stdout);
    if ((iimg = img_open(c_path)) == 0)
    {
      printf("Couldn't open '%s'\n", c_path);
    }
    else
    {
      imgl_add(il, iimg);
      printf("done\n");
    }
    fflush(stdout);
  }

  fclose(fp);
}

void imgl_load_images_from_textfile(IMAGELIST *il, char *filename, std::map<std::string, int> *map_userId)
{
  IMAGE *iimg;
  FILE *fp;
  char buf[MAXLISTBUF];  // 这个字符数组长度一定要长，因为它接受的list文件流长度可能会很长

  if (filename[0] == '\0') {
    printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Invalid file '%s'\n", filename);
    return;
  }

  if ((fp = fopen(filename, "r")) == NULL) {
    printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Couldn't open '%s'\n", filename);
    return;
  }

  // Set the data path
  QDir dir = programRootDir();
  dir.cd(DATA);
  QString data_path = dir.absolutePath();

  std::string str_data_path = data_path.toStdString();

  int i_userNum = 0, imgNum = 0, scale = 0;
  char userid[40], imageFormat[10];

  std::pair< std::map< std::string,int >::iterator,bool > isInsert;

  while (fgets(buf, 19999, fp) != NULL) {

    sscanf(buf, "%*[^/]/%*[^/]/%[^_]_%d_%d.%[^_]", userid, &imgNum, &scale, imageFormat);

    if(map_userId->find(userid) == map_userId->end())
    {
        // 插入
        isInsert = map_userId->insert(std::pair<std::string, int>(userid, i_userNum + 1));
        if(isInsert.second)
        {
            // 插入成功
            i_userNum++;
        }
    }

    std::string s_path = str_data_path + "/" + buf;
    char * c_path = (char *)s_path.c_str();

    imgl_munge_name(c_path);
    printf("Loading '%s'...\n", c_path);
    fflush(stdout);
    if ((iimg = img_open(c_path)) == 0)
    {
      printf("Couldn't open '%s'\n", c_path);
    }
    else
    {
      imgl_add(il, iimg);
      printf("done\n");
    }
    fflush(stdout);
  }

  fclose(fp);
}

std::map<std::string, int> * imgl_get_map_userId(char *filename, std::map<std::string, int> *map_userId)
{
    FILE *fp;
    char buf[MAXLISTBUF];  // 这个字符数组长度一定要长，因为它接受的list文件流长度可能会很长

    if (filename[0] == '\0') {
      printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Invalid file '%s'\n", filename);
      return nullptr;
    }

    if ((fp = fopen(filename, "r")) == NULL) {
      printf("IMGL_LOAD_IMAGES_FROM_TEXTFILE: Couldn't open '%s'\n", filename);
      return nullptr;
    }

    int i_userNum = 0;
    char userid[40];

    std::pair< std::map< std::string,int >::iterator,bool > isInsert;

    while (fgets(buf, 19999, fp) != NULL) {

      sscanf(buf, "%*[^/]/%*[^/]/%[^_]_*", userid);

      if(map_userId->find(userid) == map_userId->end())
      {
          // 插入
          isInsert = map_userId->insert(std::pair<std::string, int>(userid, i_userNum + 1));
      }

      if(isInsert.second)
      {
          // 插入成功
          i_userNum++;
      }
    }
    fclose(fp);
    return map_userId;
}
