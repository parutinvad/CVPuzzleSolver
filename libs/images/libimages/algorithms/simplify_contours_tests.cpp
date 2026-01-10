#include "simplify_contours.h"

#include <gtest/gtest.h>

#include <libimages/tests_utils.h>
#include <libbase/configure_working_directory.h>
#include <libimages/debug_io.h>
#include <libimages/image.h>

#include <algorithm>
#include <vector>

namespace {

static std::vector<point2i> makeRectContour(point2i from, point2i to_exclusive) {
    // Rectangle perimeter in clockwise order, without repeating start at end.
    const int left = from.x;
    const int top = from.y;
    const int right = to_exclusive.x - 1;
    const int bottom = to_exclusive.y - 1;

    const int w = right - left + 1;
    const int h = bottom - top + 1;

    std::vector<point2i> c;
    c.reserve(static_cast<size_t>(2 * w + 2 * h - 4));

    // top edge (left -> right)
    for (int x = left; x <= right; ++x) c.push_back({x, top});
    // right edge (top+1 -> bottom)
    for (int y = top + 1; y <= bottom; ++y) c.push_back({right, y});
    // bottom edge (right-1 -> left)
    for (int x = right - 1; x >= left; --x) c.push_back({x, bottom});
    // left edge (bottom-1 -> top+1)
    for (int y = bottom - 1; y >= top + 1; --y) c.push_back({left, y});

    return c;
}

static void rotateToMinYX(std::vector<point2i>& pts) {
    if (pts.empty()) return;
    int best = 0;
    for (int i = 1; i < static_cast<int>(pts.size()); ++i) {
        if (pts[i].y < pts[best].y || (pts[i].y == pts[best].y && pts[i].x < pts[best].x)) best = i;
    }
    std::rotate(pts.begin(), pts.begin() + best, pts.end());
}

static image8u visualize(const std::vector<point2i>& contour,
                         const std::vector<point2i>& corners,
                         int w, int h)
{
    image8u img(w, h, 1);
    img.fill(0);

    for (const auto& p : contour) {
        if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h) img(p.y, p.x) = 120;
    }
    for (const auto& p : corners) {
        if (p.x >= 0 && p.x < w && p.y >= 0 && p.y < h) img(p.y, p.x) = 255;
    }
    return img;
}

} // namespace

TEST(simplify_contours, simplifyContour_rectangle_to_4_corners) {
    configureWorkingDirectory();

    const point2i from{2, 3};
    const point2i to{7, 8}; // exclusive => w=5,h=5 => perimeter 16

    auto contour = makeRectContour(from, to);
    ASSERT_EQ(contour.size(), 16u);

    auto simplified = simplifyContour(contour, 4);
    ASSERT_EQ(simplified.size(), 4u);

    std::vector<point2i> expected = {
        {from.x, from.y},           // top-left
        {to.x - 1, from.y},         // top-right
        {to.x - 1, to.y - 1},       // bottom-right
        {from.x, to.y - 1},         // bottom-left
    };

    rotateToMinYX(simplified);
    rotateToMinYX(expected);

    debug_io::dump_image(getUnitCaseDebugDir() + "00_contour.jpg", visualize(contour, {}, 12, 12));
    debug_io::dump_image(getUnitCaseDebugDir() + "01_simplified_4.jpg", visualize(contour, simplified, 12, 12));

    EXPECT_EQ(simplified, expected);
}

TEST(simplify_contours, simplifyContour_target_ge_size_returns_same) {
    configureWorkingDirectory();

    auto contour = makeRectContour(point2i{1, 1}, point2i{6, 6}); // 16 points
    auto simplified = simplifyContour(contour, contour.size());
    EXPECT_EQ(simplified, contour);

    debug_io::dump_image(getUnitCaseDebugDir() + "00_contour.jpg", visualize(contour, {}, 10, 10));
}

TEST(simplify_contours, splitContourByCorners_rectangle_sides) {
    configureWorkingDirectory();

    const point2i from{2, 3};
    const point2i to{7, 8}; // exclusive

    auto contour = makeRectContour(from, to);

    std::vector<point2i> corners = {
        {from.x, from.y},
        {to.x - 1, from.y},
        {to.x - 1, to.y - 1},
        {from.x, to.y - 1},
    };

    auto parts = splitContourByCorners(contour, corners);
    ASSERT_EQ(parts.size(), 4u);

    // Each side should be 5 pixels long (inclusive corners) for 5x5 bbox.
    for (const auto& p : parts) {
        EXPECT_EQ(p.size(), 5u);
        // consecutive 8-neighbors
        for (size_t i = 0; i + 1 < p.size(); ++i) {
            const int dx = std::abs(p[i].x - p[i + 1].x);
            const int dy = std::abs(p[i].y - p[i + 1].y);
            EXPECT_LE(dx, 1);
            EXPECT_LE(dy, 1);
            EXPECT_TRUE(dx != 0 || dy != 0);
        }
    }

    debug_io::dump_image(getUnitCaseDebugDir() + "00_contour.jpg", visualize(contour, corners, 12, 12));
    // Visualize each side with different intensities (just to see split)
    {
        image8u img(12, 12, 1);
        img.fill(0);
        unsigned char v = 60;
        for (const auto& part : parts) {
            for (const auto& p : part) img(p.y, p.x) = v;
            v = static_cast<unsigned char>(std::min(255, v + 50));
        }
        for (const auto& c : corners) img(c.y, c.x) = 255;
        debug_io::dump_image(getUnitCaseDebugDir() + "01_parts.jpg", img);
    }
}
