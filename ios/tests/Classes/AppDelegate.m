// AppDelegate.m
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "AppDelegate.h"
#import "MasterViewController.h"
#import "DetailViewController.h"

@interface AppDelegate () {
    UIWindow *_window;
    UINavigationController *_navigationController;
    UISplitViewController *_splitViewController;
    MasterViewController *_masterController;
}
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    _window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    if (ISPHONE) {
        _masterController = [[MasterViewController alloc] init];
        _navigationController = [[UINavigationController alloc] initWithRootViewController:_masterController];
        _window.rootViewController = _navigationController;
    }
    else {
        _masterController = [[MasterViewController alloc] init];
        _navigationController = [[UINavigationController alloc] initWithRootViewController:_masterController];
        
        DetailViewController *detailController = [[DetailViewController alloc] init];
        UINavigationController *detailNavigationController = [[UINavigationController alloc] initWithRootViewController:detailController];
        
        _masterController.detailViewController = detailController;
        
        _splitViewController = [[UISplitViewController alloc] init];
        _splitViewController.delegate = detailController;
        _splitViewController.viewControllers = [NSArray arrayWithObjects:_navigationController, detailNavigationController, nil];
        
        _window.rootViewController = _splitViewController;
    }
    [_window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    [_masterController clearCachedData];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    [_masterController clearCachedData];
}

@end
