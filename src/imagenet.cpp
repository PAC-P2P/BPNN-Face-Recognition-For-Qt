#include "imagenet.h"

//extern void exit();
//extern int img_getpixel();

/* 避免使用0和1作为目标值的原因是sigmoid单元对于有限权值不能产生这样的输出,
   如果我们企图用训练网络来准确匹配目标值0和1，梯度下降将会迫使权值无限增长。
   值0.1和0.9是sigmoid函数在有限权值情况下可以完成的。
*/


/*** This is the target output encoding for a network with one output unit.
     It scans the image name, and if it's an image of me (js) then
     it sets the target unit to HIGH; otherwise it sets it to LOW.
     Remember, units are indexed starting at 1, so target unit 1
     is the one to change....  ***/
/***
    目标输出编码
***/

int load_target(IMAGE *img, BPNN *net, std::map<std::string, int> *map_userId)
{
  int imgNum, scale;
  char userid[40], photo[40];
  userid[0] = photo[0] = '\0';

  int userNum = (int)map_userId->size();

  /*** scan in the image features ***/
  sscanf(NAME(img), "%[^_]_%d_%d.%[^_]", userid, &imgNum, &scale, photo);

  //对 net->target 数组中每个元素初始化为 TARGET_LOW
  for(int i = 1; i <= userNum; ++i)
  {
      net->target[i] = TARGET_LOW;
  }

  std::map<std::string, int>::iterator map_iter;
  for(map_iter = map_userId->begin(); map_iter != map_userId->end(); ++map_iter)
  {
      if (!strcmp(userid, (map_iter->first).c_str()))
      {
          net->target[map_iter->second] = TARGET_HIGH;
      }
  }

  return 0;
}


/***********************************************************************/
/********* You shouldn't need to change any of the code below.   *******/
/***********************************************************************/

int load_input_with_image(IMAGE *img, BPNN *net)
{
  double *units;
  int nr, nc, imgsize, i, j, k;

  nr = ROWS(img);
  nc = COLS(img);
  imgsize = nr * nc;;
  if (imgsize != net->input_n) {
    printf("LOAD_INPUT_WITH_IMAGE: This image has %d pixels,\n", imgsize);
    printf("   but your net has %d input units.  I give up.\n", net->input_n);
    //exit (-1);
    return (-1);
  }

  units = net->input_units;
  k = 1;
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++) {
      units[k] = ((double) img_getpixel(img, i, j)) / 255.0;
      k++;
    }
  }
  return 0;
}
