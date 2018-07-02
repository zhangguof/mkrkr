#include <Foundation/Foundation.h>

void path_info()
{
	NSLog(@"%@",NSHomeDirectory());
	// MyApp.app
    NSString* cur_path = [[NSBundle mainBundle] bundlePath];
    NSLog(@"bundlePath=%@",cur_path);
    NSFileManager *fileManager = [NSFileManager defaultManager];
//    NSString* data_path = cur_path + @"/data/startup.tjs";
    NSString* data_path = [cur_path stringByAppendingString:@"/DATA/startup.tjs"];
    
    BOOL ret = [fileManager fileExistsAtPath:data_path];
    NSLog(@"is exist?%@:::%i",data_path,ret);
//    // tmp
//    NSLog(@"%@",NSTemporaryDirectory());
//
//    // Documents
//    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    NSString *docPath = [paths objectAtIndex:0];
//    NSLog(@"%@",docPath);
//
//    // Library
//    paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
//    NSString *libPath = [paths objectAtIndex:0];
//    NSLog(@"%@",libPath);
}
