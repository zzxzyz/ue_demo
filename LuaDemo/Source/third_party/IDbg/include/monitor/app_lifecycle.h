//
//  Created by mjzheng on 2021/7/22.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_MONITOR_APP_STATE_H_
#define IDBG_MONITOR_APP_STATE_H_

#include <functional>
#include <string>
#include <map>
#include <vector>
#include "base/singleton.h"
#include "core/thread_cpu_info.h"
#include "thread/cross_handler_thread.h"
#include "thread/cross_timer.h"

namespace IDbg {

class AppLifecycleObserver{
public:
  using Ptr = std::shared_ptr<AppLifecycleObserver>;
  virtual ~AppLifecycleObserver() = default;
  
  /*
   app enter background
   */
  virtual void ApplicationDidEnterBackground() = 0;
  
  /*
   app become active
   */
  virtual void ApplicationDidBecomeActive() = 0;
  
  /*
   app suspend by system
   */
  virtual void Suspend() = 0;
  
  /*
   app resume by system
   */
  virtual void Resume() = 0;
  
  /*
   app launch finish
   */
  virtual void EndLaunch() = 0;
};

using AppLifecycleObserverList = std::vector<AppLifecycleObserver*>;

class AppLifecycle : public Singleton<AppLifecycle>{
  friend class Singleton<AppLifecycle>;
public:
  void AddObserver(AppLifecycleObserver* delegate);
  void RemoveObserver(AppLifecycleObserver* delegate);
  
  void ApplicationDidEnterBackground();
  void ApplicationDidBecomeActive();
  
  void Suspend();
  void Resume();
  
  void EndLaunch();
  
private:
  AppLifecycleObserverList observers_;
};

}  // namespace IDbg

#endif  // IDBG_MONITOR_APP_STATE_H_
