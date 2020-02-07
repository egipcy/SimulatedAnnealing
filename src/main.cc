#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <FreeImagePlus.h>

#include "my-image.hh"
#include "metropolis-hastings.hh"

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    std::cerr << "Error: expected 3 arguments, got " << argc << std::endl;
    return 1;
  }

  std::string filename_input = argv[1];
  std::string filename_output = argv[2];

  MyImage image(filename_input);

  std::cout << image.get_width() << "x" << image.get_height() << std::endl;

  BYTE byte_0 = 0;
  BYTE byte_255 = 255;

  MyImage::s_color k = {&byte_0, &byte_0, &byte_0};
  MyImage::s_color b = {&byte_0, &byte_0, &byte_255};
  MyImage::s_color g = {&byte_0, &byte_255, &byte_0};
  MyImage::s_color c = {&byte_0, &byte_255, &byte_255};
  MyImage::s_color r = {&byte_255, &byte_0, &byte_0};
  MyImage::s_color m = {&byte_255, &byte_0, &byte_255};
  MyImage::s_color y = {&byte_255, &byte_255, &byte_0};
  MyImage::s_color w = {&byte_255, &byte_255, &byte_255};

  std::vector<MyImage::s_color> colors = {k, b, g, c, r, m, y, w};

  double beta = 10000;
  double div_temperature = 100000;
  bool special_init = true;
  size_t nb_iterations = 20000000;

  auto im = metropolis_hastings(image,
                                colors,
                                beta,
                                div_temperature,
                                nb_iterations,
                                special_init);

  im.write(filename_output);

  return 0;
}
