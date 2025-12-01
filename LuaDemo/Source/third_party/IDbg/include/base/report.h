//
//  Created by mjzheng on 2021/7/22.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#ifndef IDBG_BASE_REPORT_H_
#define IDBG_BASE_REPORT_H_

#include <functional>
#include <map>
#include <string>
#include "build_config.h"
#include "singleton.h"

#define REPORT(event_name, event_properties) (*IDbg::Report::Instance())(event_name, event_properties)

namespace IDbg {

using ReportDelegate = std::function<void(const std::string& event_name, std::map<std::string, std::string>&& event_properties)>;

class IDBG_API Report : public Singleton<Report>{
  friend class Singleton<Report>;
 public:
  void SetDelegate(ReportDelegate&& p);
  Report& operator()(const std::string& event_name, std::map<std::string, std::string>&& event_properties);

 private:
  Report() = default;
  ~Report() = default;
 private:
  ReportDelegate delegate_ = nullptr;
};

}  // namespace IDbg

#endif  // IDBG_BASE_REPORT_H_
