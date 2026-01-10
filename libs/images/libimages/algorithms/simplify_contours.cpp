#include "simplify_contours.h"

#include <libbase/runtime_assert.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <queue>
#include <tuple>
#include <vector>

namespace {

double dist2_point_to_line(point2i p, point2i a, point2i b) {
    const long long vx = static_cast<long long>(b.x) - a.x;
    const long long vy = static_cast<long long>(b.y) - a.y;
    const long long wx = static_cast<long long>(p.x) - a.x;
    const long long wy = static_cast<long long>(p.y) - a.y;

    const long long vv = vx * vx + vy * vy;
    if (vv == 0) {
        return static_cast<double>(wx * wx + wy * wy);
    }

    const long long cross = vx * wy - vy * wx;
    const double cross2 = static_cast<double>(cross) * static_cast<double>(cross);
    return cross2 / static_cast<double>(vv);
}

struct HeapItem {
    double cost = 0.0;
    int idx = -1;
    std::size_t ver = 0;

    bool operator>(const HeapItem& o) const noexcept {
        if (cost != o.cost) return cost > o.cost;
        return idx > o.idx;
    }
};

} // namespace

std::vector<point2i> simplifyContour(const std::vector<point2i> &contour, size_t targetVertexSize) {
    const int n = static_cast<int>(contour.size());
    if (targetVertexSize == 0 || contour.empty()) return {};
    if (static_cast<size_t>(n) <= targetVertexSize) return contour;

    std::vector<int> prev(n), next(n);
    std::vector<bool> alive(n, true);
    std::vector<std::size_t> version(n, 0);

    for (int i = 0; i < n; ++i) {
        prev[i] = (i - 1 + n) % n;
        next[i] = (i + 1) % n;
    }

    auto compute_cost = [&](int i) -> double {
        if (!alive[i]) return std::numeric_limits<double>::infinity();
        const int a = prev[i];
        const int b = next[i];
        if (!alive[a] || !alive[b]) return std::numeric_limits<double>::infinity();
        return dist2_point_to_line(contour[i], contour[a], contour[b]);
    };

    std::priority_queue<HeapItem, std::vector<HeapItem>, std::greater<HeapItem>> pq;
    for (int i = 0; i < n; ++i) {
        pq.push(HeapItem{compute_cost(i), i, version[i]});
    }

    int aliveCount = n;

    while (aliveCount > static_cast<int>(targetVertexSize)) {
        rassert(!pq.empty(), 71238123);

        HeapItem it = pq.top();
        pq.pop();

        const int i = it.idx;
        if (i < 0 || i >= n) continue;
        if (!alive[i]) continue;
        if (it.ver != version[i]) continue;

        const int a = prev[i];
        const int b = next[i];

        // Remove i from cyclic linked list
        alive[i] = false;
        --aliveCount;

        next[a] = b;
        prev[b] = a;

        // Update neighbors' costs
        version[a] += 1;
        version[b] += 1;
        pq.push(HeapItem{compute_cost(a), a, version[a]});
        pq.push(HeapItem{compute_cost(b), b, version[b]});
    }

    // Collect remaining vertices in contour order starting from the smallest original index still alive.
    int start = -1;
    for (int i = 0; i < n; ++i) {
        if (alive[i]) {
            start = i;
            break;
        }
    }
    rassert(start >= 0, 71238124);

    std::vector<point2i> out;
    out.reserve(targetVertexSize);

    int cur = start;
    do {
        out.push_back(contour[cur]);
        cur = next[cur];
    } while (cur != start && static_cast<int>(out.size()) <= aliveCount + 1);

    return out;
}

std::vector<std::vector<point2i>> splitContourByCorners(
    const std::vector<point2i> &contour,
    const std::vector<point2i> &corners)
{
    if (contour.empty()) return {};
    if (corners.empty()) return {contour};

    const int n = static_cast<int>(contour.size());

    std::vector<int> cornerIdx;
    cornerIdx.reserve(corners.size());

    for (const auto& c : corners) {
        int idx = -1;
        for (int i = 0; i < n; ++i) {
            if (contour[i] == c) { idx = i; break; }
        }
        rassert(idx >= 0, 918273650);
        cornerIdx.push_back(idx);
    }

    std::sort(cornerIdx.begin(), cornerIdx.end());
    cornerIdx.erase(std::unique(cornerIdx.begin(), cornerIdx.end()), cornerIdx.end());

    rassert(cornerIdx.size() >= 2, 918273651);

    const int m = static_cast<int>(cornerIdx.size());
    std::vector<std::vector<point2i>> parts;
    parts.reserve(static_cast<size_t>(m));

    for (int k = 0; k < m; ++k) {
        const int i = cornerIdx[k];
        const int j = cornerIdx[(k + 1) % m];

        std::vector<point2i> part;
        part.reserve(static_cast<size_t>(n / m + 4));

        part.push_back(contour[i]);

        if (i < j) {
            for (int t = i + 1; t <= j; ++t) part.push_back(contour[t]);
        } else {
            for (int t = i + 1; t < n; ++t) part.push_back(contour[t]);
            for (int t = 0; t <= j; ++t) part.push_back(contour[t]);
        }

        parts.push_back(std::move(part));
    }

    return parts;
}
