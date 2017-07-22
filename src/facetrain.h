#ifndef FACETRAIN_H
#define FACETRAIN_H

#include "backprop.h"
#include "pgmimage.h"
#include "imagenet.h"

//typedef int bool;
#define false 0
#define true 1

// 评估表现
int evaluate_performance(BPNN *, double *);

/*** Computes the performance of a net on the images in the imagelist. ***/
/*** Prints out the percentage correct on the image set, and the
     average error between the target and the output units for the set. ***/
int performance_on_imagelist(BPNN *, IMAGELIST *, int);

// 评估图片集的匹配情况
std::string result_on_imagelist(BPNN *, IMAGELIST *, int, std::map<std::string, int> *);

int backprop_face(IMAGELIST *, IMAGELIST *, IMAGELIST *, int, int, char *, int, int);

void facetrain(int);

#endif // FACETRAIN_H
