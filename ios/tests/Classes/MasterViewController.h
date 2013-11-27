// MasterViewController.h
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@class DetailViewController;

@interface MasterViewController : UITableViewController

@property (nonatomic, strong) DetailViewController *detailViewController;

- (void)clearCachedData;

@end
