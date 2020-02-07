#pragma once

MyImage metropolis_hastings(const MyImage& origin_image,
  const std::vector<MyImage::s_color>& x_possible, double beta,
  double div_temperature, size_t nb_iterations, bool special_init);

void init_parameters(MyImage& origin_image,
  const std::vector<MyImage::s_color>& x_possible);