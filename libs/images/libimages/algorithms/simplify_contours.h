#pragma once

#include <libbase/point2.h>

#include <vector>

std::vector<point2i> simplifyContour(const std::vector<point2i> &contour, size_t targetVertexSize);

std::vector<std::vector<point2i>> splitContourByCorners(const std::vector<point2i> &contour, const std::vector<point2i> &corners);
