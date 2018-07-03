#include <Foundation/Foundation.h>

const char* ios_get_data_path()
{
    // Documents
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *docPath = [paths objectAtIndex:0];
    return [docPath UTF8String];
}

void list_path();

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
//    list_path();

}

void list_path()
{
    NSString* resources = [ [ NSBundle mainBundle ] resourcePath ];
    NSString* path = [resources stringByAppendingString:@"/data/sound/sse"];
    NSLog(@"list:%@",path);
    NSURL *folderURL = [NSURL fileURLWithPath:path];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError __autoreleasing *error = nil;
    NSArray *folderContents = [fileManager contentsOfDirectoryAtURL:folderURL includingPropertiesForKeys:nil options:0 error:&error];
    NSLog(@"返回文件夹内容的URL:%@",folderContents);
}

void init_locale()
{
    //init local
    NSString* resources = [ [ NSBundle mainBundle ] resourcePath ];
    int success = setenv("PATH_LOCALE", [resources cStringUsingEncoding:1], 1);
    setlocale(LC_CTYPE,"UTF-8");
    NSLog(@"setenv success,%@:%d",resources,success);
}
