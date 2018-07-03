#include <Foundation/Foundation.h>

const char* ios_get_data_path()
{
    // Documents
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *docPath = [paths objectAtIndex:0];
    return [docPath UTF8String];
}

void path_info()
{
	NSLog(@"%@",NSHomeDirectory());
	// MyApp.app
    NSString* cur_path = [[NSBundle mainBundle] bundlePath];
    NSLog(@"bundlePath=%@",cur_path);
//    NSFileManager *fileManager = [NSFileManager defaultManager];
//    NSString* data_path = cur_path + @"/data/startup.tjs";

//    
//    BOOL ret = [fileManager fileExistsAtPath:data_path];
//    NSLog(@"is exist?%@:::%i",data_path,ret);
//    // tmp
//    NSLog(@"%@",NSTemporaryDirectory());
//
    // Documents
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docPath = [paths objectAtIndex:0];
    NSLog(@"Documenets=%@",docPath);

//    // Library
//    paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
//    NSString *libPath = [paths objectAtIndex:0];
//    NSLog(@"%@",libPath);

}

void init_locale()
{
    //init local
    NSString* resources = [ [ NSBundle mainBundle ] resourcePath ];
    int success = setenv("PATH_LOCALE", [resources cStringUsingEncoding:1], 1);
    setlocale(LC_CTYPE,"UTF-8");
    NSLog(@"setenv success,%@:%d",resources,success);
}
