#include "color.h"

#include <libbase/runtime_assert.h>

#include <algorithm>
#include <string>

template <typename T>
Color<T>::Color() {
    init(1);
    data_[0] = T(0);
}

template <typename T>
void Color<T>::init(int channels) {
    rassert(channels == 1 || channels == 3, "Invalid color channels count", channels);
    c_ = channels;
    data_.assign(static_cast<std::size_t>(c_), T(0));
}

template <typename T>
Color<T>::Color(T gray) {
    init(1);
    data_[0] = gray;
}

template <typename T>
Color<T>::Color(T r, T g, T b) {
    init(3);
    data_[0] = r;
    data_[1] = g;
    data_[2] = b;
}

template <typename T>
int Color<T>::channels() const noexcept {
    return c_;
}

template <typename T>
std::tuple<int> Color<T>::size() const noexcept {
    return {c_};
}

template <typename T>
T* Color<T>::data() noexcept {
    return data_.data();
}

template <typename T>
const T* Color<T>::data() const noexcept {
    return data_.data();
}

template <typename T>
std::vector<T> Color<T>::toVector() const {
    return data_;
}

template <typename T>
void Color<T>::fill(const T& v) {
    std::fill(data_.begin(), data_.end(), v);
}

template <typename T>
void Color<T>::check_bounds(int c, std::source_location loc) const {
    rassert(c >= 0 && c < c_, 3812459123,
            "Color channel out of bounds:", "c=" + std::to_string(c) + "/channels=" + std::to_string(c_),
            format_code_location(loc));
}

template <typename T>
T& Color<T>::operator()(int c, std::source_location loc) {
    check_bounds(c, loc);
    return data_[static_cast<std::size_t>(c)];
}

template <typename T>
const T& Color<T>::operator()(int c, std::source_location loc) const {
    check_bounds(c, loc);
    return data_[static_cast<std::size_t>(c)];
}

template <typename T>
bool Color<T>::operator==(const Color<T>& other) const noexcept {
    if (c_ != other.c_) return false;
    return data_ == other.data_;
}

// Explicit instantiations
template class Color<std::uint8_t>;
template class Color<float>;
