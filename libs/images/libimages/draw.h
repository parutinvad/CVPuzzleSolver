#pragma once

#include <vector>

#include <libbase/point2.h>
#include <libimages/image.h>

#include "color.h"

template <typename T>
void drawPoint(Image<T>& image, point2i pixel, Color<T> c, int size=1);

template <typename T>
void drawPoints(Image<T>& image, const std::vector<point2i>& pixels, Color<T> c);

extern template void drawPoint<std::uint8_t>(Image<std::uint8_t>& image, point2i pixel, Color<uint8_t> c, int size);
extern template void drawPoint<float>(Image<float>& image, point2i pixel, Color<float> c, int size);

extern template void drawPoints<std::uint8_t>(Image<std::uint8_t>& image, const std::vector<point2i>& pixels, Color<uint8_t> c);
extern template void drawPoints<float>(Image<float>& image, const std::vector<point2i>& pixels, Color<float> c);
