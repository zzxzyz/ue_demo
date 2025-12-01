//
//  monitor_usage.h
//  idbg_demo
//
//  Created by 郑俊明 on 2022/1/15.
//

#ifndef monitor_usage_hpp
#define monitor_usage_hpp

#include <memory>
#include <string>

class MonitorUsage {
public:
  using Ptr = std::shared_ptr<MonitorUsage>;
  virtual ~MonitorUsage() = default;
  
  virtual void Start() = 0;
  
  virtual void Stop() = 0;
};

MonitorUsage::Ptr CreateMonitorUsage();

#endif /* monitor_usage_hpp */
