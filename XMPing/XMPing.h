///
///  XMPing.h
///  XMPing
///
///  Created by mifit on 15/10/30.
///  Copyright © 2015年 mifit. All rights reserved.
///

#import <Foundation/Foundation.h>
#if TARGET_OS_EMBEDDED || TARGET_IPHONE_SIMULATOR
#import <CFNetwork/CFNetwork.h>
#else
#import <CoreServices/CoreServices.h>
#endif

#include <AssertMacros.h>
#pragma mark  - XMPing

@protocol XMPingDelegate;

@interface XMPing : NSObject{
    NSString *              _hostName;
    NSData *                _hostAddress;
    CFHostRef               _host;
    CFSocketRef             _socket;
    
    uint16_t                _identifier;                            // host byte order
    uint16_t                _nextSequenceNumber;
}

@property (nonatomic, assign, readwrite) id<XMPingDelegate> delegate;

@property (nonatomic, copy,   readonly)  NSString *             hostName;
@property (nonatomic, copy,   readonly)  NSData *               hostAddress;
@property (nonatomic, assign, readonly)  uint16_t               identifier;
@property (nonatomic, assign, readonly)  uint16_t               nextSequenceNumber;

+ (XMPing *)simplePingWithHostName:(NSString *)hostName;        // chooses first IPv4 address
+ (XMPing *)simplePingWithHostAddress:(NSData *)hostAddress;    // contains (struct sockaddr)

/// Starts the pinger object pinging.  You should call this after
/// you've setup the delegate and any ping parameters.
- (void)start;

/// Sends an actual ping.  Pass nil for data to use a standard 56 byte payload (resulting in a
/// standard 64 byte ping).  Otherwise pass a non-nil value and it will be appended to the
/// ICMP header.
///
/// Do not try to send a ping before you receive the -simplePing:didStartWithAddress: delegate
/// callback.
- (void)sendPingWithData:(NSData *)data;

/// Stops the pinger object.  You should call this when you're done
/// pinging.
- (void)stop;

/// Given a valid IP packet contains an ICMP , returns the address of the ICMP header that
/// follows the IP header.  This doesn't do any significant validation of the packet.
+ (const struct ICMPHeader *)icmpInPacket:(NSData *)packet;

@end




#pragma mark  delegate
@protocol XMPingDelegate <NSObject>
@optional

/// Called after the XMPing has successfully started up.  After this callback, you
/// can start sending pings via -sendPingWithData:
- (void)XMPing:(XMPing *)pinger didStartWithAddress:(NSData *)address;

/// If this is called, the XMPing object has failed.  By the time this callback is
/// called, the object has stopped (that is, you don't need to call -stop yourself).
- (void)XMPing:(XMPing *)pinger didFailWithError:(NSError *)error;


/// IMPORTANT: On the send side the packet does not include an IP header.
/// On the receive side, it does.  In that case, use +[XMPing icmpInPacket:]
/// to find the ICMP header within the packet.

/// Called whenever the XMPing object has successfully sent a ping packet.
- (void)XMPing:(XMPing *)pinger didSendPacket:(NSData *)packet;

/// Called whenever the XMPing object tries and fails to send a ping packet.
- (void)XMPing:(XMPing *)pinger didFailToSendPacket:(NSData *)packet error:(NSError *)error;

/// Called whenever the XMPing object receives an ICMP packet that looks like
/// a response to one of our pings (that is, has a valid ICMP checksum, has
/// an identifier that matches our identifier, and has a sequence number in
/// the range of sequence numbers that we've sent out).
- (void)XMPing:(XMPing *)pinger didReceivePingResponsePacket:(NSData *)packet;

/// Called whenever the XMPing object receives an ICMP packet that does not
/// look like a response to one of our pings.
- (void)XMPing:(XMPing *)pinger didReceiveUnexpectedPacket:(NSData *)packet;
@end

#pragma mark - * IP and ICMP On-The-Wire Format
/// The following declarations specify the structure of ping packets on the wire.

/// IP header structure:
struct IPHeader {
    uint8_t     versionAndHeaderLength;
    uint8_t     differentiatedServices;
    uint16_t    totalLength;
    uint16_t    identification;
    uint16_t    flagsAndFragmentOffset;
    uint8_t     timeToLive;
    uint8_t     protocol;
    uint16_t    headerChecksum;
    uint8_t     sourceAddress[4];
    uint8_t     destinationAddress[4];
    /// options...
    /// data...
};

typedef struct IPHeader IPHeader;

check_compile_time(sizeof(IPHeader) == 20);
check_compile_time(offsetof(IPHeader, versionAndHeaderLength) == 0);
check_compile_time(offsetof(IPHeader, differentiatedServices) == 1);
check_compile_time(offsetof(IPHeader, totalLength) == 2);
check_compile_time(offsetof(IPHeader, identification) == 4);
check_compile_time(offsetof(IPHeader, flagsAndFragmentOffset) == 6);
check_compile_time(offsetof(IPHeader, timeToLive) == 8);
check_compile_time(offsetof(IPHeader, protocol) == 9);
check_compile_time(offsetof(IPHeader, headerChecksum) == 10);
check_compile_time(offsetof(IPHeader, sourceAddress) == 12);
check_compile_time(offsetof(IPHeader, destinationAddress) == 16);

/// ICMP type and code combinations:
enum {
    kICMPTypeEchoReply   = 0,           /// code is always 0
    kICMPTypeEchoRequest = 8            /// code is always 0
};

/// ICMP header structure:
struct ICMPHeader {
    uint8_t     type;
    uint8_t     code;
    uint16_t    checksum;
    uint16_t    identifier;
    uint16_t    sequenceNumber;
    /// data...
};
typedef struct ICMPHeader ICMPHeader;

check_compile_time(sizeof(ICMPHeader) == 8);
check_compile_time(offsetof(ICMPHeader, type) == 0);
check_compile_time(offsetof(ICMPHeader, code) == 1);
check_compile_time(offsetof(ICMPHeader, checksum) == 2);
check_compile_time(offsetof(ICMPHeader, identifier) == 4);
check_compile_time(offsetof(ICMPHeader, sequenceNumber) == 6);