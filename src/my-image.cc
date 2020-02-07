#include "my-image.hh"

#include <iostream>

MyImage::MyImage(const std::string& filename)
{
  image_.load(filename.c_str());

  if ((image_.getColorType() != FIC_RGBALPHA
      && image_.getColorType() != FIC_RGB)
    || image_.getBitsPerPixel() > 32)
  {
    std::cout << "Converting to 32 bits..." << std::endl;

    if (!image_.convertTo32Bits())
    {
      std::cerr << "Type: Can't convert to 32 bits"
                << std::endl;
      return;
    }

    if ((image_.getColorType() != FIC_RGBALPHA
        && image_.getColorType() != FIC_RGB)
      || image_.getBitsPerPixel() > 32)
    {
      std::cerr << "ColorType: Can't handle this \
        type of image: it MUST be RGB or RGBA" << std::endl;
      return;
    }
  }

  init();
}

MyImage::MyImage(fipImage image)
{
  image_ = image;

  init();
}

void MyImage::init()
{
  width_ = image_.getWidth();
  height_ = image_.getHeight();

  auto pixels = image_.accessPixels();

  datas_ = std::vector<s_color>(width_ * height_);

  size_t x = 1;
  size_t mul = image_.getColorType() == FIC_RGB ? 3 : 4;
  for (size_t i = 0; i < datas_.size(); i++)
  {
    size_t w = i % width_;
    size_t h = i / width_;

    if ((i + 1) % width_ == 0)
      x += 1;

    if (mul == 4)
      datas_[i].a = &pixels[((height_ - 1 - h) * width_ + w) * mul + 3];
    datas_[i].r = &pixels[((height_ - 1 - h) * width_ + w) * mul + 2];
    datas_[i].g = &pixels[((height_ - 1 - h) * width_ + w) * mul + 1];
    datas_[i].b = &pixels[((height_ - 1 - h) * width_ + w) * mul + 0];
  }
}

void MyImage::write(const std::string& filename)
{
  image_.save(filename.c_str());
}

fipImage MyImage::get_fipImage() const
{
  return image_;
}

size_t MyImage::get_width() const
{
  return width_;
}

size_t MyImage::get_height() const
{
  return height_;
}

MyImage::s_color& MyImage::operator[](size_t i)
{
  return datas_[i];
}

MyImage::s_color MyImage::operator[](size_t i) const
{
  return datas_[i];
}

std::vector<MyImage::s_color>& MyImage::get_datas()
{
  return datas_;
}

std::vector<MyImage::s_color> MyImage::get_datas() const
{
  return datas_;
}