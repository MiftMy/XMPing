//
//  XMPingHelper.h
//  XMPing
//
//  Created by mifit on 15/10/30.
//  Copyright © 2015年 mifit. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface XMPingHelper : NSObject
/**
 *  @parm address ip地址
 *  @parm target  对象
 *  @parm sel     ping结果方法
 */
+ (void)ping:(NSString*)address target:(id)target sel:(SEL)sel;
@end
