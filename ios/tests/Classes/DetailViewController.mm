// DetailViewController.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "DetailViewController.h"
#import "TestView.h"

@interface DetailViewController ()
@property (nonatomic, WEAK) UIPopoverController *masterPopoverController;
@end

@implementation DetailViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor colorWithPatternImage:
                                 [UIImage imageNamed:@"TouchVG.bundle/translucent.png"]];
    
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, 
                                                          NSUserDomainMask, YES) objectAtIndex:0];
    NSLog(@"Document directory: %@", path);
    
    UIBarButtonItem *saveButton = [[UIBarButtonItem alloc] initWithTitle:@"Save" style:UIBarButtonItemStylePlain target:self action:@selector(saveDetailPage:)];          
    self.navigationItem.rightBarButtonItem = saveButton;
}

- (void)viewDidUnload
{
    if ([_content.view respondsToSelector:@selector(tearDown)]) {
        [_content.view performSelector:@selector(tearDown)];
    }
    [super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES;
}

- (id)init
{
    self = [super init];
    if (self) {
        self.title = @"Detail";
    }
    return self;
}

- (void)setContent:(UIViewController *)c
{
    if (_content) {
        [_content.view removeFromSuperview];
        _content.view = nil;
        [_content RELEASE];
    }
    _content = [c RETAIN];
    if (_content) {
        UIView *view = _content.view;
        CGRect barframe = self.navigationController.navigationBar.frame;
        CGFloat y = barframe.origin.y + barframe.size.height;
        view.frame = CGRectMake(0, y, self.view.bounds.size.width,
                                self.view.bounds.size.height - y);
        [self.view addSubview:view];
        view.autoresizingMask = 0xFF;
        [view setNeedsDisplay];
    }
    self.title = _content ? _content.title : @"Detail";
}

- (void)clearCachedData
{
    static int bkmode = 0;
    
    bkmode = ++bkmode % 3;
    switch (bkmode) {
        case 0:
            self.view.backgroundColor = [UIColor colorWithPatternImage:[UIImage imageNamed:@"TouchVG.bundle/translucent.png"]];
            break;
        case 1:
            self.view.backgroundColor = [UIColor whiteColor];
            break;
        case 2:
            self.view.backgroundColor = [UIColor darkGrayColor];
            break;
    }

    [self.navigationItem setLeftBarButtonItem:nil animated:YES];
    self.masterPopoverController = nil;
    if ([_content.view respondsToSelector:@selector(clearCachedData)]) {
        [_content.view performSelector:@selector(clearCachedData)];
    }
}

- (void)editDetailPage:(id)sender
{
    [self.masterPopoverController dismissPopoverAnimated:YES];
    
    if ([_content.view respondsToSelector:@selector(edit)]) {
        [_content.view performSelector:@selector(edit)];
    }
    else if ([_content.view.subviews count] > 0) {
        UIView *sview = [_content.view.subviews objectAtIndex:0];
        if ([sview respondsToSelector:@selector(edit)]) {
            [sview performSelector:@selector(edit)];
        }
    }
}

- (void)saveViewPng:(id)obj
{
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                          NSUserDomainMask, YES) objectAtIndex:0];
    static int order = 0;
    NSString *filename = [NSString stringWithFormat:@"%@/page%d.png", path, order++ % 10];
    
    if ([obj performSelector:@selector(exportPNG:) withObject:filename]) {
        NSString *msg = [NSString stringWithFormat:@"%@", 
                         [filename substringFromIndex:[filename length] - 19]];
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Save" message:msg
                                                       delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
    }
}

- (void)saveDetailPage:(id)sender
{
    [self.masterPopoverController dismissPopoverAnimated:YES];
    
    if ([_content.view respondsToSelector:@selector(save)]) {
        [_content.view performSelector:@selector(save)];
    }
    else if ([_content.view respondsToSelector:@selector(exportPNG:)]) {
        [self saveViewPng:_content.view];
    }
    else if ([_content.view.subviews count] > 0) {
        UIView *sview = [_content.view.subviews objectAtIndex:0];
        if ([sview respondsToSelector:@selector(save)]) {
            [sview performSelector:@selector(save)];
        }
        else if ([sview respondsToSelector:@selector(exportPNG:)]) {
            [self saveViewPng:sview];
        }
    }
}

#pragma mark - Split view

- (void)splitViewController:(UISplitViewController *)splitController willHideViewController:(UIViewController *)viewController withBarButtonItem:(UIBarButtonItem *)barButtonItem forPopoverController:(UIPopoverController *)popoverController
{
    barButtonItem.title = @"Demos";
    [self.navigationItem setLeftBarButtonItem:barButtonItem animated:YES];
    self.masterPopoverController = popoverController;
}

- (void)splitViewController:(UISplitViewController *)splitController willShowViewController:(UIViewController *)viewController invalidatingBarButtonItem:(UIBarButtonItem *)barButtonItem
{
    // Called when the view is shown again in the split view, invalidating the button and popover controller.
    [self.navigationItem setLeftBarButtonItem:nil animated:YES];
    self.masterPopoverController = nil;
}

@end
