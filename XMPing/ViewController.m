//
//  ViewController.m
//  XMPing
//
//  Created by mifit on 15/10/30.
//  Copyright © 2015年 mifit. All rights reserved.
//

#import "ViewController.h"
#import "XMPingHelper.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    NSLog(@"24235");
    int f = [self reverse:24235];
    NSLog(@"%d",f);
    int dd = reverseF(24235);
     NSLog(@"%d",dd);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (IBAction)pingToIP:(id)sender {
    [XMPingHelper ping:self.ip.text target:self sel:@selector(pingResult:)];
}

- (void)pingResult:(NSNumber *)result{
    if ([result boolValue]){
        NSLog(@"ssss");
    } else {
        NSLog(@"ffff");
    }
    
}
-  (int) reverse:(int) number
{
    int tem = number;
    int result = 0;
    while ( tem > 0) {
        result *= 10;
        result += tem % 10;
        tem /= 10;
    }
    return result;
}

int reverseF(int number)
{
    int number_new = 0;
    int size_of_number = 0;
    int temp = number;
    
    while (temp){
        size_of_number++;
        temp /= 10;
    };
    for (int i = 1,j = size_of_number; i <= size_of_number, j >= 1; i++, j--)
    {
        double f = pow((double) 10,i-1);
        double s = (number / (int)pow((double)10,j-1));
        number_new += (int)s * f;
        number -= (int)pow((double)10,j-1) * (number / (int)pow((double)10,j-1));
    }
    return number_new;
}
@end
