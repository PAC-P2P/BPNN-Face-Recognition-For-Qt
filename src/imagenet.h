#ifndef IMAGENET_H
#define IMAGENET_H

#define TARGET_HIGH 0.9
#define TARGET_LOW 0.1

//#include <stdio.h>
#include <iostream>
//#include <string.h>
#include <string>
#include <map>
#include "pgmimage.h"
#include "backprop.h"

int load_target(IMAGE *, BPNN *, std::map<std::string, int> *);
int load_input_with_image(IMAGE *, BPNN *);

#endif // IMAGENET_H
