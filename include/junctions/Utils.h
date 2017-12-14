
#ifndef JUNCTIONS_UTILS_H_
#define JUNCTIONS_UTILS_H_

namespace ju {

template <typename T>
// T: The type that we want an ID for.
struct IdForType {
  static size_t getId() {
    static_assert(sizeof(&getId) == sizeof(size_t), "size_t must be the same size as a pointer");
    return reinterpret_cast<size_t>(&getId);
  }
};

}  // namespace ju

#endif  // JUNCTIONS_UTILS_H_
