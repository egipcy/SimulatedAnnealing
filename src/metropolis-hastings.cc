#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "my-image.hh"
#include "random-generator.hh"

BYTE abs_diff(BYTE a, BYTE b)
{
  return a > b ? a - b : b - a;
}

size_t color_diff(const MyImage::s_color& c1,
                  const MyImage::s_color& c2)
{
  size_t r = abs_diff(*c1.r, *c2.r);
  size_t g = abs_diff(*c1.g, *c2.g);
  size_t b = abs_diff(*c1.b, *c2.b);
  return r + g + b;
}

bool color_equal(const MyImage::s_color& c1,
                 const MyImage::s_color& c2)
{
  return *c1.r == *c2.r && *c1.g == *c2.g && *c1.b == *c2.b;
}

size_t get_random(size_t max_value)
{
  return RandomGenerator::get_instance().get_random(max_value);
}

double get_random()
{
  return (double)std::rand() / RAND_MAX;
}

MyImage::s_color get_random_x_new(
  const std::vector<MyImage::s_color>& x_possible,
  const MyImage::s_color& x)
{
  size_t i = 0;
  for (auto x_poss: x_possible)
    if (color_equal(x, x_poss))  // x in x_possible
    {
      size_t random = get_random(x_possible.size() - 2);

      size_t i;
      for (i = 0; i <= random; i++)
        if (color_equal(x_possible[i], x))
          return x_possible[random + 1];

      return x_possible[i - 1];
    }

  return x_possible[get_random(x_possible.size() - 1)];
}

std::vector<MyImage::s_color> get_neighborhood(size_t pixel_pos,
                                               const MyImage& image)
{
  std::vector<MyImage::s_color> ret;

  size_t x = pixel_pos % image.get_width();
  size_t y = pixel_pos / image.get_width();

  if (x > 0)
  {
    if (y > 0)
      ret.push_back(image[pixel_pos - 1 - image.get_width()]);
    ret.push_back(image[pixel_pos - 1]);
    if (y < image.get_height() - 1)
      ret.push_back(image[pixel_pos - 1 + image.get_width()]);
  }
  if (y > 0)
    ret.push_back(image[pixel_pos - image.get_width()]);
  if (y < image.get_height() - 1)
    ret.push_back(image[pixel_pos + image.get_width()]);
  if (x < image.get_width() - 1)
  {
    if (y > 0)
      ret.push_back(image[pixel_pos + 1 - image.get_width()]);
    ret.push_back(image[pixel_pos + 1]);
    if (y < image.get_height() - 1)
      ret.push_back(image[pixel_pos + 1 + image.get_width()]);
  }

  return ret;
}

double get_energy(const MyImage::s_color& y,
                  const MyImage::s_color& x,
                  const std::vector<MyImage::s_color>& x_neighbors,
                  double beta)
{
  double pixel_energy = color_diff(x, y) / 3.0 / 255.0;

  double neighbors_energy = 0;
  for (auto x_neighbor: x_neighbors)
    neighbors_energy += color_diff(x, x_neighbor);

  neighbors_energy /= 3.0 * 255.0 * x_neighbors.size();

  return pixel_energy + beta * neighbors_energy;
}

double get_total_energy(const MyImage& origin_image,
                        const MyImage& current_image,
                        double beta)
{
  double ret = 0;
  for (size_t i = 0; i < current_image.get_datas().size(); i++)
    ret += get_energy(origin_image[i],
                      current_image[i],
                      get_neighborhood(i, current_image),
                      beta);

  return ret;
}

double get_ratio(const MyImage::s_color& y,
                 const MyImage::s_color& x,
                 const MyImage::s_color& x_new,
                 const std::vector<MyImage::s_color>& x_neighbors,
                 double beta,
                 double temperature)
{
  auto energy_old = get_energy(y, x, x_neighbors, beta);
  auto energy_new = get_energy(y, x_new, x_neighbors, beta);

  return std::exp((energy_old - energy_new) * temperature);
}

void init_parameters(MyImage& origin_image,
  const std::vector<MyImage::s_color>& x_possible)
{
  for (auto& p: origin_image.get_datas())
  {
    auto best_x = x_possible[0];
    for (size_t i = 1; i < x_possible.size(); i++)
      if (color_diff(x_possible[i], p) < color_diff(best_x, p))
        best_x = x_possible[i];

    *p.r = *best_x.r;
    *p.g = *best_x.g;
    *p.b = *best_x.b;
  }
}

MyImage metropolis_hastings(const MyImage& origin_image,
  const std::vector<MyImage::s_color>& x_possible, double beta,
  double div_temperature, size_t nb_iterations, bool special_init)
{
  MyImage image(origin_image.get_fipImage());

  if (special_init)
    init_parameters(image, x_possible);

  std::vector<bool> pixel_changed(image.get_datas().size(), false);

  size_t not_changed = 0;
  for (size_t i = 1; i <= nb_iterations; i++)
  {
    size_t pixel_pos = get_random(image.get_datas().size() - 1);

    auto y = origin_image[pixel_pos];

    auto x = image[pixel_pos];
    auto x_neighbors = get_neighborhood(pixel_pos, image);

    auto x_new = get_random_x_new(x_possible, x); // x_new != x

    double alpha = get_ratio(y, x, x_new, x_neighbors, beta,
                             i / div_temperature);

    if (alpha >= 1.0 || get_random() < alpha)
    {
      *image[pixel_pos].r = *x_new.r;
      *image[pixel_pos].g = *x_new.g;
      *image[pixel_pos].b = *x_new.b;
      pixel_changed[pixel_pos] = true;
    }
    else
      not_changed++;
  }

  std::cout << (double) not_changed / nb_iterations * 100
            << "% unchanged"
            << std::endl;

  double rate_pixel_changed = 0;
  for (auto p: pixel_changed)
    if (p)
      rate_pixel_changed++;
  rate_pixel_changed /= pixel_changed.size();

  std::cout << rate_pixel_changed * 100
            << "% of pixels changed from "
            << pixel_changed.size()
            << " pixels"
            << std::endl;

  return image;
}