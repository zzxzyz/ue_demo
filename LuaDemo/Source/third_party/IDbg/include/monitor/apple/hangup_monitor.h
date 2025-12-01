//
//  Created by mjzheng on 2022/7/28.
//  Copyright © 2021 mjzheng. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 add comment
 */
@interface HangupMonitor : NSObject
- (instancetype)init;
- (void)start;
- (void)stop;
- (void)applicationDidEnterBackground;
- (void)applicationDidBecomeActive;
- (void)suspend;
- (void)resume;
- (void)endLaunch;
- (void)flush;
@end

NS_ASSUME_NONNULL_END

