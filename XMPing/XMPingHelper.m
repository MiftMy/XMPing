//
//  XMPingHelper.m
//  XMPing
//
//  Created by mifit on 15/10/30.
//  Copyright © 2015年 mifit. All rights reserved.
//

#import "XMPingHelper.h"
#import "XMPing.h"
@interface XMPingHelper()<XMPingDelegate>
@property(nonatomic,retain) XMPing* simplePing;
@property(nonatomic,retain) id target;
@property(nonatomic,assign) SEL sel;
- (id)initWithAddress:(NSString*)address target:(id)_target sel:(SEL)_sel;
- (void)go;
@end

@implementation XMPingHelper
+ (void)ping:(NSString*)address target:(id)target sel:(SEL)sel {
    // The helper retains itself through the timeout function
    [[[XMPingHelper alloc] initWithAddress:address target:target sel:sel] go];
}

- (void)dealloc {
    _simplePing = nil;
    _target = nil;
}

- (id)initWithAddress:(NSString*)address target:(id)target sel:(SEL)sel {
    if (self = [self init]) {
        self.simplePing = [XMPing simplePingWithHostName:address];
        self.simplePing.delegate = self;
        _target = target;
        _sel = sel;
    }
    return self;
}

- (void)go {
    [self.simplePing start];
    [self performSelector:@selector(endTime) withObject:nil afterDelay:2]; // This timeout is what retains the ping helper
}

#pragma mark - Finishing and timing out

// Called on success or failure to clean up
- (void)killPing {
    [self.simplePing stop];
    // In case, higher up the call stack, this got called by the simpleping object itself
    self.simplePing = nil;
}

- (void)successPing {
    [self killPing];
    [_target performSelector:_sel withObject:[NSNumber numberWithBool:YES]];
}

- (void)failPing:(NSString*)reason {
    [self killPing];
    [_target performSelector:_sel withObject:[NSNumber numberWithBool:NO]];
}

// Called 1s after ping start, to check if it timed out
- (void)endTime {
    if (self.simplePing) { // If it hasn't already been killed, then it's timed out
        [self failPing:@"timeout"];
    }
}

#pragma mark - Pinger delegate

// When the pinger starts, send the ping immediately
- (void)XMPing:(XMPing *)pinger didStartWithAddress:(NSData *)address {
    [self.simplePing sendPingWithData:nil];
}

- (void)XMPing:(XMPing *)pinger didFailWithError:(NSError *)error {
    [self failPing:@"didFailWithError"];
}

- (void)XMPing:(XMPing *)pinger didFailToSendPacket:(NSData *)packet error:(NSError *)error {
    // Eg they're not connected to any network
    [self failPing:@"didFailToSendPacket"];
}

- (void)XMPing:(XMPing *)pinger didReceivePingResponsePacket:(NSData *)packet {
    [self successPing];
}
@end
