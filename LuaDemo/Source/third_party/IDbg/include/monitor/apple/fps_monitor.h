//
//  WeMeetQAPM.h
//  WeMeet
//
//  Created by mjzheng on 2023/04/17.
//  Copyright © 2023 tencent. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

int GetFPS();

@interface FPSMonitor : NSObject
- (void)start;
- (void)stop;
- (void)statisticsFPS:(double)vsync_time Index:(int)frameIndex;
- (int)getFPS;
@end

NS_ASSUME_NONNULL_END
