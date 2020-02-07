#pragma once

#include <FreeImagePlus.h>
#include <string>
#include <vector>

class MyImage
{
public:
  struct s_color
  {
    BYTE* r;
    BYTE* g;
    BYTE* b;
    BYTE* a;

  };

  MyImage(const std::string& filename);
  MyImage(fipImage image);
  void init();
  void write(const std::string& filename);

  fipImage get_fipImage() const;

  size_t get_width() const;
  size_t get_height() const;

  s_color& operator[](size_t i);
  s_color operator[](size_t i) const;

  std::vector<s_color>& get_datas();
  std::vector<s_color> get_datas() const;

private:
  fipImage image_;

  size_t width_;
  size_t height_;

  std::vector<s_color> datas_;
};