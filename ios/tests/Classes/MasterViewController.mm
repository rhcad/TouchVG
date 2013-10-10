// MasterViewController.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "MasterViewController.h"
#import "DetailViewController.h"

@interface MasterViewController () {
    NSMutableArray *_titles;
}
@end

void getTestViewTitles(NSMutableArray *arr);
UIViewController *createTestView(NSUInteger index, CGRect frame);

@implementation MasterViewController

@synthesize detailViewController = _detailController;

- (id)init
{
    self = [super init];
    if (self) {
        self.title = @"Demos";
        if (!ISPHONE) {
            self.clearsSelectionOnViewWillAppear = NO;
            self.contentSizeForViewInPopover = CGSizeMake(320.0, 600.0);
        }
    }
    return self;
}
							
- (void)dealloc
{
    [_titles release];
    [super dealloc];
}

- (void)setNavigationButtons
{
    if (!self.navigationItem.leftBarButtonItem && _detailController) {
        UIBarButtonItem *editButton = [[UIBarButtonItem alloc]
                                       initWithBarButtonSystemItem:UIBarButtonSystemItemEdit
                                       target:_detailController
                                       action:@selector(editDetailPage:)];
        self.navigationItem.leftBarButtonItem = editButton;
        [editButton release];
    }
    
    if (!self.navigationItem.rightBarButtonItem && _detailController) {
        UIBarButtonItem *saveButton = [[UIBarButtonItem alloc]
                                       initWithBarButtonSystemItem:UIBarButtonSystemItemSave
                                       target:_detailController
                                       action:@selector(saveDetailPage:)];
        self.navigationItem.rightBarButtonItem = saveButton;
        [saveButton release];
    }
}

- (void)viewDidLoad
{
    _titles = [[NSMutableArray alloc] init];
    getTestViewTitles(_titles);
    
    [self setNavigationButtons];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return YES;
}

#pragma mark - Table View

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return _titles.count;
}

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                       reuseIdentifier:CellIdentifier] autorelease];
        if (ISPHONE) {
            cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
        }
    }
    cell.textLabel.text = [_titles objectAtIndex:indexPath.row];
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    UIViewController *controller;
    
    if (ISPHONE) {
	    if (!_detailController) {
	        _detailController = [[DetailViewController alloc] init];
            [self setNavigationButtons];
	    }
	    controller = createTestView(indexPath.row, _detailController.view.bounds);
        [self.navigationController pushViewController:_detailController animated:YES];
    }
    else {
        controller = createTestView(indexPath.row, _detailController.view.bounds);
    }
    
    _detailController.content = controller;
    [controller release];
}

- (void)clearCachedData
{
    [_detailController clearCachedData];
}

@end
