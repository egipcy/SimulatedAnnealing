#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "PGM.hh"
#include "PGM.cc"

#include "random_generator.hh"
#include "random_generator.cc"

size_t abs_diff(size_t a, size_t b)
{
  return a > b ? a - b : b - a;
}

size_t get_random(size_t max_value)
{
  return RandomGenerator::get_instance().get_random(max_value);
}

double get_random()
{
  return (double)std::rand() / RAND_MAX;
}

std::vector<size_t> get_neighborhood(size_t pixel_pos, const PGM& image)
{
  std::vector<size_t> ret;

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

double get_energy(size_t y, size_t x, std::vector<size_t> x_neighbors, double beta)
{
  double pixel_energy = abs_diff(x, y) / 255.0;

  double neighbors_energy = 0;
  for (auto x_neighbor: x_neighbors)
    neighbors_energy += abs_diff(x, x_neighbor);
  neighbors_energy /= 255.0 * x_neighbors.size();

  return pixel_energy + beta * neighbors_energy;
}

double get_total_energy(PGM origin_image, PGM current_image, double beta)
{
  double ret = 0;
  for (size_t i = 0; i < current_image.get_datas().size(); i++)
    ret += get_energy(origin_image.get_datas()[i], current_image.get_datas()[i],
                      get_neighborhood(i, current_image), beta);

  return ret;
}

double get_ratio(size_t y, size_t x, size_t x_new,
                 std::vector<size_t> x_neighbors, double beta,
                 double temperature)
{
  auto energy_old = get_energy(y, x, x_neighbors, beta);
  auto energy_new = get_energy(y, x_new, x_neighbors, beta);

  double moy_neighbors = 0;
  for (auto x_neighbor: x_neighbors)
    moy_neighbors += x_neighbor;
  moy_neighbors /= x_neighbors.size();

  /*
  std::cout << std::endl;
  std::cout << "moy neighbors: " << moy_neighbors << std::endl;
  std::cout << "old: " << y << "->" << x << " energy: " << energy_old << std::endl;
  std::cout << "new: " << y << "->" << x_new << " energy: " << energy_new << std::endl;
  */

  return std::exp((energy_old - energy_new) * temperature);
}

PGM init_parameters(PGM origin_image, const std::vector<size_t>& x_possible)
{
  PGM ret = origin_image;

  for (auto& p: ret.get_datas())
  {
    auto best_x = x_possible[0];
    for (size_t i = 1; i < x_possible.size(); i++)
      if (abs_diff(x_possible[i], p) < abs_diff(best_x, p))
        best_x = x_possible[i];

    p = best_x;
  }

  return ret;
}

size_t get_random_x_new(const std::vector<size_t>& x_possible, size_t x)
{
  for (auto x_poss: x_possible)
    if (x == x_poss)  // x in x_possible
    {
      size_t random = get_random(x_possible.size() - 2);

      size_t i;
      for (i = 0; i <= random; i++)
        if (x_possible[i] == x)
          return x_possible[random + 1];

      return x_possible[i - 1];
    }

  return x_possible[get_random(x_possible.size() - 1)];
}

PGM metropolis_hastings(PGM origin_image, const std::vector<size_t>& x_possible,
                        double beta, double div_temperature,
                        size_t nb_iterations, bool special_init)
{
  std::cout << "Total Energy=" << get_total_energy(origin_image, origin_image, beta) << std::endl;

  PGM image = origin_image;
  if (special_init)
    image = init_parameters(origin_image, x_possible);

  std::vector<bool> pixel_changed(image.get_datas().size(), false);

  size_t not_changed = 0;
  for (size_t i = 1; i <= nb_iterations; i++)
  {
    size_t pixel_pos = get_random(image.get_datas().size());

    auto y = origin_image.get_datas()[pixel_pos];

    auto x = image.get_datas()[pixel_pos];
    auto x_neighbors = get_neighborhood(pixel_pos, image);

    auto x_new = get_random_x_new(x_possible, x); // x_new != x

    double alpha = get_ratio(y, x, x_new, x_neighbors, beta, i / div_temperature);

    //std::cout << "proba=" << acceptance_ratio << std::endl;

    if (alpha >= 1.0 || get_random() < alpha)
    {
      image.get_datas()[pixel_pos] = x_new;
      pixel_changed[pixel_pos] = true;
    }
    else
      not_changed++;
  }

  std::cout << (double) not_changed / nb_iterations * 100 << "% unchanged" << std::endl;

  double rate_pixel_changed = 0;
  for (auto p: pixel_changed)
    if (p)
      rate_pixel_changed++;
  rate_pixel_changed /= pixel_changed.size();

  std::cout << rate_pixel_changed * 100 << "% of pixels changed from " << pixel_changed.size() << " pixels" << std::endl;

  return image;
}

int main()
{
  std::string name = "house";
  auto image = PGM(name + ".pgm");

  init_parameters(image, {0, 255}).write(name + ".0.pgm");

  double beta = 1;
  double div_temperature = 100000;
  bool special_init = false;

  auto im = metropolis_hastings(image, {0, 255}, beta, div_temperature, 1000000, special_init);

  std::ostringstream beta_str;
  beta_str << beta;
  beta_str << std::setprecision(2);

  std::ostringstream div_temperature_str;
  div_temperature_str << div_temperature;
  div_temperature_str << std::setprecision(0);

  im.write(name + ".beta=" + beta_str.str() + ".temp=" + div_temperature_str.str() + (special_init ? ".si" : "") + ".pgm");

  return 0;
}

