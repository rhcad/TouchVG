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

- (void)setNavigationButtons
{
    if (!self.navigationItem.leftBarButtonItem && _detailViewController) {
        UIBarButtonItem *editButton = [[UIBarButtonItem alloc]
                                       initWithBarButtonSystemItem:UIBarButtonSystemItemEdit
                                       target:_detailViewController
                                       action:@selector(editDetailPage:)];
        self.navigationItem.leftBarButtonItem = editButton;
    }
    
    if (!self.navigationItem.rightBarButtonItem && _detailViewController) {
        UIBarButtonItem *saveButton = [[UIBarButtonItem alloc]
                                       initWithBarButtonSystemItem:UIBarButtonSystemItemSave
                                       target:_detailViewController
                                       action:@selector(saveDetailPage:)];
        self.navigationItem.rightBarButtonItem = saveButton;
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
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                      reuseIdentifier:CellIdentifier];
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
        if (!_detailViewController) {
            _detailViewController = [[DetailViewController alloc] init];
            [self setNavigationButtons];
        }
        controller = createTestView(indexPath.row, _detailViewController.view.bounds);
        [self.navigationController pushViewController:_detailViewController animated:YES];
    }
    else {
        controller = createTestView(indexPath.row, _detailViewController.view.bounds);
    }
    
    _detailViewController.content = controller;
}

- (void)clearCachedData
{
    [_detailViewController clearCachedData];
}

@end
