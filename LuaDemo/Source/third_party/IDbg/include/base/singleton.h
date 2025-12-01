//
//  Created by mjzheng on 2021/4/13.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_BASE_SINGLETON_H_
#define IDBG_BASE_SINGLETON_H_

#include "build_config.h"

namespace IDbg {

class NonCopyable {
 protected:
  virtual ~NonCopyable() = default;
  NonCopyable() = default;
 private:
  NonCopyable(const NonCopyable& other) = delete;
  NonCopyable(NonCopyable&& other) = delete;
  NonCopyable& operator=(const NonCopyable& other) = delete;
  NonCopyable& operator=(NonCopyable&& other) = delete;
};

template <typename T>
class  Singleton : public NonCopyable {
 public:
  static T* Instance();
 protected:
  virtual ~Singleton() = default;
  Singleton() = default;
};

template <typename T>
T* Singleton<T>::Instance() {
  static T instance;
  return &instance;
}

}  // namespace IDbg

#endif  // IDBG_BASE_SINGLETON_H_
