//
//  Created by mjzheng on 2021/7/22.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_BASE_ENUM_UTIL_H_
#define IDBG_BASE_ENUM_UTIL_H_

#include <set>

namespace IDbg {

template <typename T>
constexpr auto enum_cast(T t) noexcept {
  return static_cast<std::underlying_type_t<T>>(t);
}

template <typename T>
constexpr std::enable_if_t<std::is_enum<T>::value, T> operator|(const T lhs, const T rhs) noexcept {
  return T(enum_cast(lhs) | enum_cast(rhs));
}

template <typename T>
constexpr std::enable_if_t<std::is_enum<T>::value, T&> operator|=(T* lhs, const T rhs) noexcept {
  lhs = T(enum_cast(lhs) | enum_cast(rhs));
  return lhs;
}

template <typename T>
constexpr std::enable_if_t<std::is_enum<T>::value, T> operator&(const T lhs, const T rhs) noexcept {
  return T(enum_cast(lhs) & enum_cast(rhs));
}

template <typename T>
constexpr std::enable_if_t<std::is_enum<T>::value, T&> operator&=(T& lhs, const T rhs) noexcept { // NOLINT [build/c++11]
  lhs = T(enum_cast(lhs) & enum_cast(rhs));
  return lhs;
}

template <typename T>
constexpr std::enable_if_t<std::is_enum<T>::value, T> operator^(const T lhs, const T rhs) noexcept {
  return T(enum_cast(lhs) ^ enum_cast(rhs));
}

template <typename T>
constexpr std::enable_if_t<std::is_enum<T>::value, T&> operator^=(T& lhs, const T rhs) noexcept { // NOLINT [build/c++11]
  lhs = T(enum_cast(lhs) ^ enum_cast(rhs));
  return lhs;
}

template <typename T>
constexpr std::enable_if_t<std::is_enum<T>::value, T&> operator~(const T op) noexcept {
  return T(~enum_cast(op));
}

// c++14
//template <typename T>
//decltype(auto) move_def(T&& param) {
//  using ReturnType = typename std::remove_reference<T>::type&&;
//  return static_cast<ReturnType>(param);
//}
//
//template <typename T>
//T&& forward_def(std::remove_reference_t<T>& param) {
//  return static_cast<T&&>(param);
//}
//
//std::multiset<std::string> names;
//template <typename T>
//void logAndAdd(T&& name) {
//  names.emplace(std::forward<T>(name));
//}
//
//void logAndAdd(int idx) {
//  names.emplace(std::to_string(idx));
//}
//
//template <typename T>
//void logAndAddTag(T&& name) {
//  logAndAddImpl(std::forward<T>(name), std::is_integral<typename std::remove_reference<T>::type>());
//}
//
//template <typename T>
//void logAndAddImpl(T&& name,  std::false_type) {
//  names.emplace(std::forward<T>(name));
//}
//
//template <typename T>
//void logAndAddImpl(int idx,  std::true_type) {
//  names.emplace(std::to_string(idx));
//}
//
//class Person {
//public:
//  template <typename T, typename = std::enable_if_t<
//  !std::is_base_of<Person,std::decay_t<T>>::value
//  &&
//  !std::is_integral<std::remove_reference_t<T>>::value
//  >>
//  explicit Person(T&& n);
//};

}  // namespace IDbg

#endif  // IDBG_BASE_ENUM_UTIL_H_
