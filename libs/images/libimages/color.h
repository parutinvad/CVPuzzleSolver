#pragma once

#include <cstdint>
#include <cstddef>
#include <source_location>
#include <tuple>
#include <vector>
#include <variant>

template <typename T> class Color final {
public:
    using value_type = T;

    Color();
    explicit Color(T gray);
    Color(T r, T g, T b);

    int channels() const noexcept;
    std::tuple<int> size() const noexcept;

    T* data() noexcept;
    const T* data() const noexcept;
    std::vector<T> toVector() const;

    void fill(const T& v);

    T& operator()(int c, std::source_location loc = std::source_location::current());
    const T& operator()(int c, std::source_location loc = std::source_location::current()) const;

    bool operator==(const Color& other) const noexcept;
    bool operator!=(const Color& other) const noexcept { return !(*this == other); }

private:
    int c_ = 0;
    std::vector<T> data_;

    void init(int channels);
    void check_bounds(int c, std::source_location loc) const;
};

extern template class Color<std::uint8_t>;
extern template class Color<float>;

using color8u = Color<std::uint8_t>;
using color32f = Color<float>;
