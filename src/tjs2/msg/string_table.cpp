﻿#include "string_table_resource.h"
#include <map>
#include <string>
#include <stdio.h>
std::map<int,std::wstring> string_tables;
void init_string_table()
{
//string_tables[ID]="xxxx";
printf("init_string_table!\n");
    string_tables[IDS_TJS_INTERNAL_ERROR] = L"发生了内部错误";
    string_tables[IDS_TJS_WARNING] = L"警告：";
    string_tables[IDS_TJS_WARN_EVAL_OPERATOR] = L"使用运算符时非全局变量放在后面。（请注意：当前运算符在 TJS 2.4.1 有作改动）";
    string_tables[IDS_TJS_NARROW_TO_WIDE_CONVERSION_ERROR] = L"不能将现有文字转换为 Unicode 文字。包含的有不能识别的文字在内。请确认 data 是否指定有误。";
    string_tables[IDS_TJS_VARIANT_CONVERT_ERROR] = L"不能转换变量类型 %1 到 %2";
    string_tables[IDS_TJS_VARIANT_CONVERT_ERROR_TO_OBJECT] = L"不能将 %1 转换为 Object 类型";
    string_tables[IDS_TJS_IDEXPECTED] = L"请指定 ID";
    string_tables[IDS_TJS_SUBSTITUTION_IN_BOOLEAN_CONTEXT] = L"请使用 = 运算符来对比。（使用 '(A=B)!=0' 来与 0 作对比）";
    string_tables[IDS_TJS_CANNOT_MODIFY_LHS] = L"进行了错误的表达式";
    string_tables[IDS_TJS_INSUFFICIENT_MEM] = L"内存不足";
    string_tables[IDS_TJS_CANNOT_GET_RESULT] = L"不能从当前表达式中获取到结果";
    string_tables[IDS_TJS_NULL_ACCESS] = L"发生了访问 null 对象";
    string_tables[IDS_TJS_MEMBER_NOT_FOUND] = L"未找到 Member '%1'";
    string_tables[IDS_TJS_MEMBER_NOT_FOUND_NO_NAME_GIVEN] = L"未指名 Member 未找到";
    string_tables[IDS_TJS_NOT_IMPLEMENTED] = L"调用了未实现的方法";
    string_tables[IDS_TJS_INVALID_PARAM] = L"参数不合法";
    string_tables[IDS_TJS_BAD_PARAM_COUNT] = L"参数个数有误";
    string_tables[IDS_TJS_INVALID_TYPE] = L"不是一个 function 或者 方法/属性 有误";
    string_tables[IDS_TJS_SPECIFY_DIC_OR_ARRAY] = L"请声明为 Dictionary 或者 Array 对象类型";
    string_tables[IDS_TJS_SPECIFY_ARRAY] = L"请声明为 Array 对象";
    string_tables[IDS_TJS_STRING_DEALLOC_ERROR] = L"不能释放 string 内存区域";
    string_tables[IDS_TJS_STRING_ALLOC_ERROR] = L"不能创建 string 内存区域";
    string_tables[IDS_TJS_MISPLACED_BREAK_CONTINUE] = L"不能写 'break' 或者 'continue' 到这里";
    string_tables[IDS_TJS_MISPLACED_CASE] = L"不能写 'case' 到这里";
    string_tables[IDS_TJS_MISPLACED_RETURN] = L"不能把 'return' 写到这里";
    string_tables[IDS_TJS_STRING_PARSE_ERROR] = L"string/regxp/octet 处理时发生了错误";
    string_tables[IDS_TJS_NUMBER_ERROR] = L"number 处理时发生了错误";
    string_tables[IDS_TJS_UNCLOSED_COMMENT] = L"注释未关闭";
    string_tables[IDS_TJS_INVALID_CHAR] = L"未知字符：'%1'";
    string_tables[IDS_TJS_EXPECTED] = L"异常发生：%1";
    string_tables[IDS_TJS_SYNTAX_ERROR] = L"语法有误 （%1）";
    string_tables[IDS_TJS_PPERROR] = L"条件表达式错误";
    string_tables[IDS_TJS_CANNOT_GET_SUPER] = L"父类不存在或者不能定位到父类";
    string_tables[IDS_TJS_INVALID_OPECODE] = L"VM 代码有误";
    string_tables[IDS_TJS_RANGE_ERROR] = L"错误：取值范围以外";
    string_tables[IDS_TJS_ACCESS_DENYED] = L"对 只读/只写 属性进行了错误操作";
    string_tables[IDS_TJS_NATIVE_CLASS_CRASH] = L"无效的 object context";
    string_tables[IDS_TJS_INVALID_OBJECT] = L"对象已无效";
    string_tables[IDS_TJS_CANNOT_OMIT] = L"不能在方法之外使用 '...'";
    string_tables[IDS_TJS_CANNOT_PARSE_DATE] = L"不正确的日期格式";
    string_tables[IDS_TJS_INVALID_VALUE_FOR_TIMESTAMP] = L"不正确的 日期/时间 值";
    string_tables[IDS_TJS_EXCEPTION_NOT_FOUND] = L"不能转换异常 因为 'Exception' 不存在";
    string_tables[IDS_TJS_INVALID_FORMAT_STRING] = L"不正确的字符串格式";
    string_tables[IDS_TJS_DIVIDE_BY_ZERO] = L"被 0 整除了";
    string_tables[IDS_TJS_NOT_RECONSTRUCTIVE_RANDOMIZE_DATA] = L"不能重新构造随机数列";
    string_tables[IDS_TJS_SYMBOL] = L"ID";
    string_tables[IDS_TJS_CALL_HISTORY_IS_FROM_OUT_OF_TJS2SCRIPT] = L"[TJS2 脚本之外]";
    string_tables[IDS_TJS_NOBJECTS_WAS_NOT_FREED] = L"共计 %1 个对象未释放";
    string_tables[IDS_TJS_OBJECT_CREATION_HISTORY_DELIMITER_CRLF] = L"\r\n                     ";
    string_tables[IDS_TJS_OBJECT_CREATION_HISTORY_DELIMITER] = L"\n                     ";
    string_tables[IDS_TJS_OBJECT_WAS_NOT_FREED_CRLF] = L"对象 %1 [%2] 未释放。对象创建在：%2";
    string_tables[IDS_TJS_OBJECT_WAS_NOT_FREED] = L"对象 %1 [%2] 未释放。对象创建在：%2";
    string_tables[IDS_TJS_GROUP_BY_OBJECT_TYPE_AND_HISTORY] = L"以对象类型和所在位置来分组";
    string_tables[IDS_TJS_GROUP_BY_OBJECT_TYPE] = L"以为对象类型来分组";
    string_tables[IDS_TJS_OBJECT_COUNTING_MESSAGE_GROUP_BY_OBJECT_TYPE_AND_HISTORY_CRLF] = L"%1 个： [%2]\r\n                     %3";
    string_tables[IDS_TJS_OBJECT_COUNTING_MESSAGE_GROUP_BY_OBJECT_TYPE_AND_HISTORY] = L"%1 个：[%2]\n                     %3";
    string_tables[IDS_TJS_OBJECT_COUNTING_MESSAGE_TJSGROUP_BY_OBJECT_TYPE] = L"%1 个： [%2]";
    string_tables[IDS_TJS_WARN_RUNNING_CODE_ON_DELETING_OBJECT_CRLF] = L"%4：运行代码在正在删除的进城中，对象 %1[%2] / 对象创建在：%3";
    string_tables[IDS_TJS_WARN_RUNNING_CODE_ON_DELETING_OBJECT] = L"%4：运行代码在正在删除的进城中，对象 %1[%2] / 对象创建在：%3";
    string_tables[IDS_TJS_WRITE_ERROR] = L"写入错误";
    string_tables[IDS_TJS_READ_ERROR] = L"读取错误";
    string_tables[IDS_TJS_SEEK_ERROR] = L"查询错误";
    string_tables[IDS_TJS_BYTE_CODE_BROKEN] = L"读取 Bytecode 错误。文件已损坏或者该文件不是 bytecode 文件。";
    string_tables[IDS_TJS_OBJECT_HASH_MAP_LOG_VERSION_MISMATCH] = L"对象的哈希图与日志版本不一致";
    string_tables[IDS_TJS_CURRUPTED_OBJECT_HASH_MAP_LOG] = L"对象哈希图已损坏";
    string_tables[IDS_TJS_UNKNOWN_FAILURE] = L"未知错误：%08X";
    string_tables[IDS_TJS_UNKNOWN_PACK_UNPACK_TEMPLATE_CHARCTER] = L"未知打包/解包模板字符";
    string_tables[IDS_TJS_UNKNOWN_BIT_STRING_CHARACTER] = L"未知 bit string 字符";
    string_tables[IDS_TJS_UNKNOWN_HEX_STRING_CHARACTER] = L"未知 16 进制字符";
    string_tables[IDS_TJS_NOT_SUPPORTED_UUENCODE] = L"不支持 uuencode";
    string_tables[IDS_TJS_NOT_SUPPORTED_BER] = L"不支持 BER 压缩";
    string_tables[IDS_TJS_NOT_SUPPORTED_UNPACK_LP] = L"不支持解包 'p'";
    string_tables[IDS_TJS_NOT_SUPPORTED_UNPACK_P] = L"不支持解包 'p'";
    string_tables[IDS_TVP_VERSION_INFORMATION] = L"吉里吉里Z 核心可执行文件 /%1 （编译日期 %DATE% %TIME%）TJS2 %2 Copyright (C) 1997-2013 W.Dee 及其贡献者 All rights reserved.";
    string_tables[IDS_TVP_VERSION_INFORMATION2] = L"详细版本信息可以通过添加 -about 参数来启动查看。";
    string_tables[IDS_TVP_DOWNLOAD_PAGE_URL] = L"http://krkrz.github.io/";
    string_tables[IDS_TVP_INTERNAL_ERROR] = L"在 %1 行 %2 处发生了内部错误。";
    string_tables[IDS_TVP_INVALID_PARAM] = L"参数不合法";
    string_tables[IDS_TVP_WARN_DEBUG_OPTION_ENABLED] = L"已启动调试设置。";
    string_tables[IDS_TVP_COMMAND_LINE_PARAM_IGNORED_AND_DEFAULT_USED] = L"命令行参数 %1 = %2 是不合法的。已使用默认值。";
    string_tables[IDS_TVP_INVALID_COMMAND_LINE_PARAM] = L"命令行参数 %1 = %2 是不合法的。";
    string_tables[IDS_TVP_NOT_IMPLEMENTED] = L"未实现。";
    string_tables[IDS_TVP_CANNOT_OPEN_STORAGE] = L"不能打开文件 %1";
    string_tables[IDS_TVP_CANNOT_FIND_STORAGE] = L"不能找到文件 %1";
    string_tables[IDS_TVP_CANNOT_OPEN_STORAGE_FOR_WRITE] = L"不能写入文件 %1";
    string_tables[IDS_TVP_STORAGE_IN_ARCHIVE_NOT_FOUND] = L"不能在归档 %2 中找到文件 %1";
    string_tables[IDS_TVP_INVALID_PATH_NAME] = L"非法路径名 %1";
    string_tables[IDS_TVP_UNSUPPORTED_MEDIA_NAME] = L"不支持的媒体类型文件 '%1'";
    string_tables[IDS_TVP_CANNOT_UNBIND_XP3EXE] = L"不能解绑 XP3 exe %1";
    string_tables[IDS_TVP_CANNOT_FIND_XP3MARK] = L"%1 不是 XP3 归档文件或者是未支持的文件";
    string_tables[IDS_TVP_MISSING_PATH_DELIMITER_AT_LAST] = L"请使用路径分隔符 '>' 或者 '/'";
    string_tables[IDS_TVP_FILENAME_CONTAINS_SHARP_WARN] = L"（Attention）文件名 '%1' 含有 '#'";
    string_tables[IDS_TVP_CANNOT_GET_LOCAL_NAME] = L"不能从 %1 中获取到本地名称";
    string_tables[IDS_TVP_READ_ERROR] = L"读取发生了错误";
    string_tables[IDS_TVP_WRITE_ERROR] = L"写入发生了错误";
    string_tables[IDS_TVP_SEEK_ERROR] = L"查找发生了错误";
    string_tables[IDS_TVP_TRUNCATE_ERROR] = L"截取过程中发生了错误";
    string_tables[IDS_TVP_INSUFFICIENT_MEMORY] = L"不能分配内存";
    string_tables[IDS_TVP_UNCOMPRESSION_FAILED] = L"解压缩失败";
    string_tables[IDS_TVP_COMPRESSION_FAILED] = L"压缩失败";
    string_tables[IDS_TVP_CANNOT_WRITE_TO_ARCHIVE] = L"不能写入到归档中";
    string_tables[IDS_TVP_UNSUPPORTED_CIPHER_MODE] = L"不支持密码模式 %1";
    string_tables[IDS_TVP_UNSUPPORTED_ENCODING] = L"不支持的编码格式 %1";
    string_tables[IDS_TVP_UNSUPPORTED_MODE_STRING] = L"不能识别 %1 文字列。";
    string_tables[IDS_TVP_UNKNOWN_GRAPHIC_FORMAT] = L"%1 是未知的图片格式";
    string_tables[IDS_TVP_CANNOT_SUGGEST_GRAPHIC_EXTENSION] = L"未找到对应扩展名为 %1 的文件";
    string_tables[IDS_TVP_MASK_SIZE_MISMATCH] = L"mask 图片的大小和主要图片的大小不一致";
    string_tables[IDS_TVP_PROVINCE_SIZE_MISMATCH] = L"区域图片 %1 与主要图片的大小不一致";
    string_tables[IDS_TVP_IMAGE_LOAD_ERROR] = L"载入图片中发生了错误 /%1";
    string_tables[IDS_TVP_JPEGLOAD_ERROR] = L"载入 JPEG 图片发生了错误 /%1";
    string_tables[IDS_TVP_PNGLOAD_ERROR] = L"载入 PNG 图片过程中发生了错误 /%1";
    string_tables[IDS_TVP_ERILOAD_ERROR] = L"载入 ERI 图片过程中发生了错误 /%1";
    string_tables[IDS_TVP_TLGLOAD_ERROR] = L"载入 TLG 图片过程中发生了错误 /%1";
    string_tables[IDS_TVP_INVALID_IMAGE_SAVE_TYPE] = L"图片保存格式无效（%1）";
    string_tables[IDS_TVP_INVALID_OPERATION_FOR8BPP] = L"发生了对 8bpp 图片格式无效的操作。";
    string_tables[IDS_TVP_INVALID_OPERATION_FOR32BPP] = L"发生了对 32bpp 图片格式无效的操作。";
    string_tables[IDS_TVP_SPECIFY_WINDOW] = L"请声明为 Window 类的对象。";
    string_tables[IDS_TVP_SPECIFY_LAYER] = L"请声明为 Layer 类的对象。";
    string_tables[IDS_TVP_SPECIFY_LAYER_OR_BITMAP] = L"请声明为 Layer 类或者 Bitmap 类的对象。";
    string_tables[IDS_TVP_CANNOT_ACCEPT_MODE_AUTO] = L"不能指定为 omAuto 模式。";
    string_tables[IDS_TVP_CANNOT_CREATE_EMPTY_LAYER_IMAGE] = L"图层大小不能指定为 0 以下的数值。";
    string_tables[IDS_TVP_CANNOT_SET_PRIMARY_INVISIBLE] = L"不能把主图层设为为隐藏。";
    string_tables[IDS_TVP_CANNOT_MOVE_PRIMARY] = L"不能移动主图层。";
    string_tables[IDS_TVP_CANNOT_SET_PARENT_SELF] = L"不能将父图层设置为自身。";
    string_tables[IDS_TVP_CANNOT_MOVE_NEXT_TO_SELF_OR_NOT_SIBLINGS] = L"Cannot move next to self or not siblings";
    string_tables[IDS_TVP_CANNOT_MOVE_PRIMARY_OR_SIBLINGLESS] = L"Cannot move primary or siblingless";
    string_tables[IDS_TVP_CANNOT_MOVE_TO_UNDER_OTHER_PRIMARY_LAYER] = L"Cannot move to under Other primary layer";
    string_tables[IDS_TVP_INVALID_IMAGE_POSITION] = L"图片位置设置有误。";
    string_tables[IDS_TVP_CANNOT_SET_MODE_TO_DISABLED_OR_MODAL] = L"已经设置为 Modal 层或者不能设置隐藏层/无效层为弹出层。";
    string_tables[IDS_TVP_NOT_DRAWABLE_LAYER_TYPE] = L"非可描绘层类型。";
    string_tables[IDS_TVP_SOURCE_LAYER_HAS_NO_IMAGE] = L"来源 layer 未包含图片。";
    string_tables[IDS_TVP_UNSUPPORTED_LAYER_TYPE] = L"未被支持的 layer 类型 %1";
    string_tables[IDS_TVP_NOT_DRAWABLE_FACE_TYPE] = L"不可描绘的 face 类型";
    string_tables[IDS_TVP_CANNOT_CONVERT_LAYER_TYPE_USING_GIVEN_DIRECTION] = L"不能转换 layer 类型到指定的 direction";
    string_tables[IDS_TVP_NEGATIVE_OPACITY_NOT_SUPPORTED_ON_THIS_FACE] = L"当前字型不支持负透明度。";
    string_tables[IDS_TVP_SRC_RECT_OUT_OF_BITMAP] = L"转换源包含 bitmap 以外的领域。请指定为正确的转换范围。";
    string_tables[IDS_TVP_BOX_BLUR_AREA_MUST_CONTAIN_CENTER_PIXEL] = L"模糊（blur）矩形的范围一定是包含（0,0）的。不能指定 left 和 right 两个都是正直或者都是负值。（top 和 bottom 一样。）";
    string_tables[IDS_TVP_BOX_BLUR_AREA_MUST_BE_SMALLER_THAN16MILLION] = L"指定的模糊（blur）矩形的范围太大。模糊矩形的像素值应小于 1677 万。";
    string_tables[IDS_TVP_CANNOT_CHANGE_FOCUS_IN_PROCESSING_FOCUS] = L"正在处理 focus 的过程中不能进行新的 focus 指定。";
    string_tables[IDS_TVP_WINDOW_HAS_NO_LAYER] = L"Window 内未找到 Layer。";
    string_tables[IDS_TVP_WINDOW_HAS_ALREADY_PRIMARY_LAYER] = L"Window 已经设置过 PrimaryLayer 了。";
    string_tables[IDS_TVP_SPECIFIED_EVENT_NEEDS_PARAMETER] = L"事件（Event）%1 需要指定参数（Parameter）。";
    string_tables[IDS_TVP_SPECIFIED_EVENT_NEEDS_PARAMETER2] = L"事件（Event）%1 需要指定第二个参数（Parameter）%2";
    string_tables[IDS_TVP_SPECIFIED_EVENT_NAME_IS_UNKNOWN] = L"未知事件(Event) %1";
    string_tables[IDS_TVP_OUT_OF_RECTANGLE] = L"超过允许矩形指定的范围了";
    string_tables[IDS_TVP_INVALID_METHOD_IN_UPDATING] = L"在画面更新时不能执行该方法";
    string_tables[IDS_TVP_CANNOT_CREATE_INSTANCE] = L"实例化当前类失败";
    string_tables[IDS_TVP_UNKNOWN_WAVE_FORMAT] = L"未支持的 Wave 文件格式 %1";
    string_tables[IDS_TVP_CURRENT_TRANSITION_MUST_BE_STOPPING] = L"在使用新的 Transition 之前需要先停止之前的。在同一 Layer 上执行复数的 Transition 时发生了错误。";
    string_tables[IDS_TVP_TRANS_HANDLER_ERROR] = L"Transition Handler 发生了错误：%1";
    string_tables[IDS_TVP_TRANS_ALREADY_REGISTERED] = L"Transition %1 已被注册过";
    string_tables[IDS_TVP_CANNOT_FIND_TRANS_HANDER] = L"未找到 %1 transition handler.";
    string_tables[IDS_TVP_SPECIFY_TRANSITION_SOURCE] = L"请声明 transition 发生的源";
    string_tables[IDS_TVP_LAYER_CANNOT_HAVE_IMAGE] = L"Layer 不能持有图片";
    string_tables[IDS_TVP_TRANSITION_SOURCE_AND_DESTINATION_MUST_HAVE_IMAGE] = L"执行 transition 的源和目的都需要持有图片";
    string_tables[IDS_TVP_CANNOT_LOAD_RULE_GRAPHIC] = L"载入 rule 图片 %1 失败";
    string_tables[IDS_TVP_SPECIFY_OPTION] = L"请指定 %1 设置";
    string_tables[IDS_TVP_TRANSITION_LAYER_SIZE_MISMATCH] = L"发生 transition 的源（%1）和目的（%2） 的 layer 大小不一致";
    string_tables[IDS_TVP_TRANSITION_MUTUAL_SOURCE] = L"发生 transition 的源是自身";
    string_tables[IDS_TVP_HOLD_DESTINATION_ALPHA_PARAMETER_IS_NOW_DEPRECATED] = L"警告：%1 方法的第二个参数 %2 已在 2.23 beta 2 中开始不推荐使用。请使用 Layer.holdAlpha 属性作替代。";
    string_tables[IDS_TVP_CANNOT_CONNECT_MULTIPLE_WAVE_SOUND_BUFFER_AT_ONCE] = L"不能同时连接多个 WaveSoundBuffer";
    string_tables[IDS_TVP_INVALID_WINDOW_SIZE_MUST_BE_IN64TO32768] = L"Window 大小必须在 64 到 32768 之间";
    string_tables[IDS_TVP_INVALID_OVERLAP_COUNT_MUST_BE_IN2TO32] = L"Overlap 数值范围必须在 2 到 32 之间";
    string_tables[IDS_TVP_CURRENTLY_ASYNC_LOAD_BITMAP] = L"正在异步载入 Bitmap 中";
    string_tables[IDS_TVP_FAILD_CLIPBOARD_COPY] = L"拷贝粘贴板数据失败";
    string_tables[IDS_TVP_INVALID_UTF16TO_UTF8] = L"UTF-16 载入失败，转换为 UTF-8";
    string_tables[IDS_TVP_INFO_LOADING_STARTUP_SCRIPT] = L"（info）读取启动脚本中：";
    string_tables[IDS_TVP_INFO_STARTUP_SCRIPT_ENDED] = L"（info）启动脚本读取完毕！";
    string_tables[IDS_TVP_TJS_CHAR_MUST_BE_TWO_OR_FOUR] = L"sizeof(tjs_char) 只能为 2 或者 4";
    string_tables[IDS_TVP_MEDIA_NAME_HAD_ALREADY_BEEN_REGISTERED] = L"媒体 '%1' 已被注册";
    string_tables[IDS_TVP_MEDIA_NAME_IS_NOT_REGISTERED] = L"媒体 '%1' 未注册";
    string_tables[IDS_TVP_INFO_REBUILDING_AUTO_PATH] = L"（info）自动路径再构建中...";
    string_tables[IDS_TVP_INFO_TOTAL_FILE_FOUND_AND_ACTIVATED] = L"（info）共找到 %1 个文件，已使用 %2 个文件。（耗时 %3 ms）";
    string_tables[IDS_TVP_ERROR_IN_RETRIEVING_SYSTEM_ON_ACTIVATE_ON_DEACTIVATE] = L"获取 System.onActive/onDeactive 发生错误：%1";
    string_tables[IDS_TVP_THE_HOST_IS_NOT_A16BIT_UNICODE_SYSTEM] = L"主机环境为非 16 位 Unicode 系统";
    string_tables[IDS_TVP_INFO_TRYING_TO_READ_XP3VIRTUAL_FILE_SYSTEM_INFORMATION_FROM] = L"（info）尝试从 %1 中读取 XP3 虚拟文件系统信息中";
    string_tables[IDS_TVP_SPECIFIED_STORAGE_HAD_BEEN_PROTECTED] = L"指定的文件已经被保护！";
    string_tables[IDS_TVP_INFO_FAILED] = L"（info）失败。";
    string_tables[IDS_TVP_INFO_DONE_WITH_CONTAINS] = L"（info）完成。（共计：%1 个文件，%2 块。）";
    string_tables[IDS_TVP_SEPARATOR_CRLF] = L"\r\n\r\n\r\n==============================================================================\r\n==============================================================================\r\n";
    string_tables[IDS_TVP_SEPARATOR_CR] = L"\n\n\n==============================================================================\n==============================================================================\n";
    string_tables[IDS_TVP_DEFAULT_FONT_NAME] = L"微软雅黑";
    string_tables[IDS_TVP_CANNOT_OPEN_FONT_FILE] = L"不能打开字体文件 '%1$s'";
    string_tables[IDS_TVP_FONT_CANNOT_BE_USED] = L"不能使用 '%1$s' 字体";
    string_tables[IDS_TVP_FONT_RASTERIZE_ERROR] = L"字体光栅化发生错误";
    string_tables[IDS_TVP_BIT_FIELDS_NOT_SUPPORTED] = L"不支持 BitFields";
    string_tables[IDS_TVP_COMPRESSED_BMP_NOT_SUPPORTED] = L"不支持压缩 BMP";
    string_tables[IDS_TVP_UNSUPPORTED_COLOR_MODE_FOR_PALETT_IMAGE] = L"不支持的调色图像的色彩类型";
    string_tables[IDS_TVP_NOT_WINDOWS_BMP] = L"该文件不是一个 Windows BMP 文件或者 OS/2 BMP 文件";
    string_tables[IDS_TVP_UNSUPPORTED_HEADER_VERSION] = L"不支持的头文件版本。";
    string_tables[IDS_TVP_INFO_TOUCHING] = L"（info）触下…";
    string_tables[IDS_TVP_ABORTED_TIME_OUT] = L" ... 已中断 [已超时]";
    string_tables[IDS_TVP_ABORTED_LIMIT_BYTE] = L" ... 已中断 [超出字节限制]";
    string_tables[IDS_TVP_FAILD_GLYPH_FOR_DRAW_GLYPH] = L"DrawGlyph 失败";
    string_tables[IDS_TVP_LAYER_OBJECT_IS_NOT_PROPERLY_CONSTRUCTED] = L"Panic！Layer 对象没有属性构造方法。未进行调用构造方法？";
    string_tables[IDS_TVP_UNKNOWN_UPDATE_TYPE] = L"未知 Update 类型";
    string_tables[IDS_TVP_UNKNOWN_TRANSITION_TYPE] = L"未知 transition 类型";
    string_tables[IDS_TVP_UNSUPPORTED_UPDATE_TYPE_TUT_GIVE_UPDATE] = L"Update 类型：暂不支持 tutGiveUpdate";
    string_tables[IDS_TVP_ERROR_CODE] = L"错误代码：";
    string_tables[IDS_TVP_UNSUPPORTED_JPEG_PALETTE] = L"JPEG 文件不支持色彩调整";
    string_tables[IDS_TVP_LIBPNG_ERROR] = L"libpng 错误。";
    string_tables[IDS_TVP_UNSUPPORTED_COLOR_TYPE_PALETTE] = L"未知色彩调色类型";
    string_tables[IDS_TVP_UNSUPPORTED_COLOR_TYPE] = L"未知色彩类型";
    string_tables[IDS_TVP_TOO_LARGE_IMAGE] = L"图片过大";
    string_tables[IDS_TVP_PNG_SAVE_ERROR] = L"保存 PNG 时出错";
    string_tables[IDS_TVP_TLG_UNSUPPORTED_UNIVERSAL_TRANSITION_RULE] = L"TLG 图片不能用作 province(_p) 或者 mask(_m) 图像的 universal transition rule。";
    string_tables[IDS_TVP_UNSUPPORTED_COLOR_COUNT] = L"不支持的色彩数量 : ";
    string_tables[IDS_TVP_DATA_FLAG_MUST_BE_ZERO] = L"Data flag 必须为 0 (所有的 flag 当前都未被支持)";
    string_tables[IDS_TVP_UNSUPPORTED_COLOR_TYPE_COLON] = L"不支持的色彩类型：";
    string_tables[IDS_TVP_UNSUPPORTED_EXTERNAL_GOLOMB_BIT_LENGTH_TABLE] = L"暂未支持外置 golomb bit length table.";
    string_tables[IDS_TVP_UNSUPPORTED_ENTROPY_CODING_METHOD] = L"不支持 entropy 编码方法";
    string_tables[IDS_TVP_INVALID_TLG_HEADER_OR_VERSION] = L"TLG header 无效或不支持的 TLG 版本。";
    string_tables[IDS_TVP_TLG_MALFORMED_TAG_MISSION_COLON_AFTER_NAME_LENGTH] = L"TLG SDS tag 结构异常，名字长度后面缺少列。";
    string_tables[IDS_TVP_TLG_MALFORMED_TAG_MISSION_EQUALS_AFTER_NAME] = L"TLG SDS tag 结构异常，缺少等号后面的值";
    string_tables[IDS_TVP_TLG_MALFORMED_TAG_MISSION_COLON_AFTER_VAUE_LENGTH] = L"TLG SDS tag 结构异常，值长度后面缺少列。";
    string_tables[IDS_TVP_TLG_MALFORMED_TAG_MISSION_COMMA_AFTER_TAG] = L"TLG SDS tag 结构异常，tag 后面缺少逗号";
    string_tables[IDS_TVP_FILE_SIZE_IS_ZERO] = L"文件大小为 0。";
    string_tables[IDS_TVP_MEMORY_ALLOCATION_ERROR] = L"分配内存时发生错误。";
    string_tables[IDS_TVP_FILE_READ_ERROR] = L"读取文件时发生错误。";
    string_tables[IDS_TVP_INVALID_PRERENDERED_FONT_FILE] = L"无效的预渲染字体文件或者未找到签名文件";
    string_tables[IDS_TVP_NOT16BIT_UNICODE_FONT_FILE] = L"非 16bit Unicode 字体文件。";
    string_tables[IDS_TVP_INVALID_HEADER_VERSION] = L"无效的 header 版本。";
    string_tables[IDS_TVP_TLG_INSUFFICIENT_MEMORY] = L"SaveTLG6: 内存不足";
    string_tables[IDS_TVP_TLG_TOO_LARGE_BIT_LENGTH] = L"SaveTLG6:bit 长度太大 (可能是所给图片尺寸过大的原因。)";
    string_tables[IDS_TVP_CANNOT_RETRIVE_INTERFACE_FROM_DRAW_DEVICE] = L"不能从绘图设备中获取到 interface 信息。";
    string_tables[IDS_TVP_SCRIPT_EXCEPTION_RAISED] = L"触发脚本异常";
    string_tables[IDS_TVP_HARDWARE_EXCEPTION_RAISED] = L"触发硬件异常";
    string_tables[IDS_TVP_MAIN_CDPNAME] = L"脚本编辑器（主要）";
    string_tables[IDS_TVP_EXCEPTION_CDPNAME] = L"脚本编辑器（异常通知）";
    string_tables[IDS_TVP_CANNNOT_LOCATE_UIDLLFOR_FOLDER_SELECTION] = L"未找到 krdevui.dll，因此无法手动选择文件夹或封包。\n请通过命令行手动指定目标文件夹或封包。";
    string_tables[IDS_TVP_INVALID_UIDLL] = L"krdevui.dll加载失败或版本不匹配";
    string_tables[IDS_TVP_INVALID_BPP] = L"色彩深度无效";
    string_tables[IDS_TVP_CANNOT_LOAD_PLUGIN] = L"插件加载失败：%1";
    string_tables[IDS_TVP_NOT_VALID_PLUGIN] = L"无效插件：%1";
    string_tables[IDS_TVP_PLUGIN_UNINIT_FAILED] = L"插件卸载失败";
    string_tables[IDS_TVP_CANNNOT_LINK_PLUGIN_WHILE_PLUGIN_LINKING] = L"不能在加载插件时加载其它的插件";
    string_tables[IDS_TVP_NOT_SUSIE_PLUGIN] = L"不是 Susie 插件";
    string_tables[IDS_TVP_SUSIE_PLUGIN_ERROR] = L"Susie 插件错误：%1";
    string_tables[IDS_TVP_CANNOT_RELEASE_PLUGIN] = L"插件在使用过程中不能进行释放";
    string_tables[IDS_TVP_NOT_LOADED_PLUGIN] = L"未加载插件：%1";
    string_tables[IDS_TVP_CANNOT_ALLOCATE_BITMAP_BITS] = L"不能为 Bitmap 分配内存：%1（size=%2）";
    string_tables[IDS_TVP_SCAN_LINE_RANGE_OVER] = L"%1 超过了扫描范围（正常为 0 到 %2）";
    string_tables[IDS_TVP_PLUGIN_ERROR] = L"插件出现错误：%1";
    string_tables[IDS_TVP_INVALID_CDDADRIVE] = L"无效的 CD-DA 驱动器";
    string_tables[IDS_TVP_CDDADRIVE_NOT_FOUND] = L"未找到 CD-DA 驱动器";
    string_tables[IDS_TVP_MCIERROR] = L"MCI 发生了错误：%1";
    string_tables[IDS_TVP_INVALID_SMF] = L"无效的 SMF 文件：%1";
    string_tables[IDS_TVP_MALFORMED_MIDIMESSAGE] = L"指定了无效的 MIDI 信息";
    string_tables[IDS_TVP_CANNOT_INIT_DIRECT_SOUND] = L"DirectSound 初始化失败：%1";
    string_tables[IDS_TVP_CANNOT_CREATE_DSSECONDARY_BUFFER] = L"创建 DirectSound 辅助缓冲区失败：%1/%2";
    string_tables[IDS_TVP_INVALID_LOOP_INFORMATION] = L"无效的 loop 信息：%1";
    string_tables[IDS_TVP_NOT_CHILD_MENU_ITEM] = L"指定的菜单项不是子菜单项";
    string_tables[IDS_TVP_CANNOT_INIT_DIRECT3D] = L"Direct3D 初始化失败：%1";
    string_tables[IDS_TVP_CANNOT_FIND_DISPLAY_MODE] = L"未找到合适的画面显示模式：%1";
    string_tables[IDS_TVP_CANNOT_SWITCH_TO_FULL_SCREEN] = L"不能切换到全屏模式：%1";
    string_tables[IDS_TVP_INVALID_PROPERTY_IN_FULL_SCREEN] = L"在全屏模式下无效的属性：%1";
    string_tables[IDS_TVP_INVALID_METHOD_IN_FULL_SCREEN] = L"在全屏模式下无效的方法：%2";
    string_tables[IDS_TVP_CANNOT_LOAD_CURSOR] = L"不能载入鼠标光标：%1";
    string_tables[IDS_TVP_CANNOT_LOAD_KR_MOVIE_DLL] = L"无法加载 krmovie.dll。播放视频和 flash 需要 krmovie.dll/krflash.dll。";
    string_tables[IDS_TVP_INVALID_KR_MOVIE_DLL] = L"无效的 krmovie.dll 文件。krmovie.dll/krflash.dll 版本有误。";
    string_tables[IDS_TVP_ERROR_IN_KR_MOVIE_DLL] = L"krmovie.dll 出现错误：%1";
    string_tables[IDS_TVP_WINDOW_ALREADY_MISSING] = L"Window 已经不存在";
    string_tables[IDS_TVP_PRERENDERED_FONT_MAPPING_FAILED] = L"预渲染字体映射失败：%1";
    string_tables[IDS_TVP_CONFIG_FAIL_ORIGINAL_FILE_CANNOT_BE_REWRITTEN] = L"不能写入 %1 文件";
    string_tables[IDS_TVP_CONFIG_FAIL_TEMP_EXE_NOT_ERASED] = L"不能删除文件。%1 文件不存在。";
    string_tables[IDS_TVP_EXECUTION_FAIL] = L"不能执行 %1 文件。";
    string_tables[IDS_TVP_PLUGIN_UNBOUND_FUNCTION_ERROR] = L"未在文件中找到插件所需要 %1 函数。";
    string_tables[IDS_TVP_EXCEPTION_HAD_BEEN_OCCURED] = L" = (触发异常)";
    string_tables[IDS_TVP_CONSOLE_RESULT] = L"控制台：";
    string_tables[IDS_TVP_INFO_LISTING_FILES] = L"（info）正在列出 %1 中的文件...";
    string_tables[IDS_TVP_INFO_TOTAL_PHYSICAL_MEMORY] = L"（info）总物理内存：%1";
    string_tables[IDS_TVP_INFO_SELECTED_PROJECT_DIRECTORY] = L"（info）已选择项目的目录：%1";
    string_tables[IDS_TVP_TOO_SMALL_EXECUTABLE_SIZE] = L"可执行文件太小";
    string_tables[IDS_TVP_INFO_LOADING_EXECUTABLE_EMBEDDED_OPTIONS_FAILED] = L"（info）载入可执行文件内置选项失败（已忽略）：%1";
    string_tables[IDS_TVP_INFO_LOADING_EXECUTABLE_EMBEDDED_OPTIONS_SUCCEEDED] = L"（info）成功载入可执行文件内置选项。";
    string_tables[IDS_TVP_FILE_NOT_FOUND] = L"未找到文件。";
    string_tables[IDS_TVP_INFO_LOADING_CONFIGURATION_FILE_FAILED] = L"（info）载入配置文件 '%1' 失败（已忽略）：%2";
    string_tables[IDS_TVP_INFO_LOADING_CONFIGURATION_FILE_SUCCEEDED] = L"（info）成功载入配置文件 '%1'";
    string_tables[IDS_TVP_INFO_DATA_PATH_DOES_NOT_EXIST_TRYING_TO_MAKE_IT] = L"（info）Data 路径不存在，尝试新建... %1";
    string_tables[IDS_TVP_OK] = L"成功。";
    string_tables[IDS_TVP_FAILD] = L"失败。";
    string_tables[IDS_TVP_INFO_DATA_PATH] = L"（info）Data 路径：%1";
    string_tables[IDS_TVP_INFO_SPECIFIED_OPTION_EARLIER_ITEM_HAS_MORE_PRIORITY] = L"（info）已声明设置（越早声明的优先级更高）：";
    string_tables[IDS_TVP_NONE] = L"（无）";
    string_tables[IDS_TVP_INFO_CPU_CLOCK_ROUGHLY] = L"（info）CPU 时钟频率（大致数值）：%dMHz";
    string_tables[IDS_TVP_PROGRAM_STARTED_ON] = L"程序已在 %1（%2）上启动。";
    string_tables[IDS_TVP_KIRIKIRI] = L"吉里吉里";
    string_tables[IDS_TVP_UNKNOWN_ERROR] = L"未知错误！";
    string_tables[IDS_TVP_EXIT_CODE] = L"退出识别代码：%d\n";
    string_tables[IDS_TVP_FATAL_ERROR] = L"发生了致命的错误。";
    string_tables[IDS_TVP_ENABLE_DIGITIZER] = L"指点数位设备可用。";
    string_tables[IDS_TVP_TOUCH_INTEGRATED_TOUCH] = L"当前机器有集成数位设备。";
    string_tables[IDS_TVP_TOUCH_EXTERNAL_TOUCH] = L"当前机器有外置触控数位设备。";
    string_tables[IDS_TVP_TOUCH_INTEGRATED_PEN] = L"当前机器有集成数位笔设备。";
    string_tables[IDS_TVP_TOUCH_EXTERNAL_PEN] = L"当前机器有外置数位笔设备。";
    string_tables[IDS_TVP_TOUCH_MULTI_INPUT] = L"当前机器支持多点触控。";
    string_tables[IDS_TVP_TOUCH_READY] = L"触控数位设备已准备就绪。";
    string_tables[IDS_TVP_CPU_CHECK_FAILURE] = L"CPU 检测失败。";
    string_tables[IDS_TVP_CPU_CHECK_FAILURE_CPU_FAMILY_OR_LESSER_IS_NOT_SUPPORTED] = L"CPU 检测失败：不支持低于 P4 系列的 CPU。";
    string_tables[IDS_TVP_INFO_CPU_NUMBER] = L"（info）CPU #%1 : ";
    string_tables[IDS_TVP_CPU_CHECK_FAILURE_NOT_SUPPRTED_CPU] = L"CPU 检查失败：不支持的 CPU 型号 \r\n%1";
    string_tables[IDS_TVP_INFO_FINALLY_DETECTED_CPU_FEATURES] = L"（info）最终检测出 CPU 所支持的特性：%1";
    string_tables[IDS_TVP_CPU_CHECK_FAILURE_NOT_SUPPORTED_CPU] = L"CPU 检查失败：不支持的 CPU 型号 \r\n%1";
    string_tables[IDS_TVP_INFO_CPU_CLOCK] = L"（info）CPU 时钟频率：%.1fMHz";
    string_tables[IDS_TVP_LAYER_BITMAP_BUFFER_UNDERRUN_DETECTED_CHECK_YOUR_DRAWING_CODE] = L"Layer bitmap：出现缓冲区干涸，请检查你的渲染代码！";
    string_tables[IDS_TVP_LAYER_BITMAP_BUFFER_OVERRUN_DETECTED_CHECK_YOUR_DRAWING_CODE] = L"Layer bitmap：出现缓冲区溢出，请检查你的渲染代码！";
    string_tables[IDS_TVP_FAILD_TO_CREATE_DIRECT3D] = L"创建 Direct3D9 失败。";
    string_tables[IDS_TVP_FAILD_TO_DECIDE_BACKBUFFER_FORMAT] = L"备用缓冲区格式未确定。";
    string_tables[IDS_TVP_FAILD_TO_CREATE_DIRECT3DDEVICE] = L"创建 Direct3D9 设备失败。";
    string_tables[IDS_TVP_FAILD_TO_SET_VIEWPORT] = L"设置 viewport 失败。";
    string_tables[IDS_TVP_FAILD_TO_SET_RENDER_STATE] = L"设置渲染状态失败。";
    string_tables[IDS_TVP_WARNING_IMAGE_SIZE_TOO_LARGE_MAY_BE_CANNOT_CREATE_TEXTURE] = L"warning : 图像尺寸过大。可能创建贴图会失败。";
    string_tables[IDS_TVP_USE_POWER_OF_TWO_SURFACE] = L"请使用能被 2 整除的 surface。";
    string_tables[IDS_TVP_CANNOT_ALLOCATE_D3DOFF_SCREEN_SURFACE] = L"不能分配 D3D 离屏 surface/HR=%1";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_FAILED_TO_CREATE_DIRECT3DDEVICES] = L"BasicDrawDevice: 创建 Direct3D9 设备失败：%1";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_FAILED_TO_CREATE_DIRECT3DDEVICES_UNKNOWN_REASON] = L"BasicDrawDevice: 创建 Direct3D9 设备失败：未知原因";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_TEXTURE_HAS_ALREADY_BEEN_LOCKED] = L"BasicDrawDevice: Texture 已被锁定（在未调用 EndBitmapCompletion() 的情况下 StartBitmapCompletion() 已经被调用了 2 次）,解锁中";
    string_tables[IDS_TVP_INTERNAL_ERROR_RESULT] = L"内部错误 /HR=%1";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_INF_POLYGON_DRAWING_FAILED] = L"BasicDrawDevice: (inf) 多边形渲染失败/HR=%1";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_INF_DIRECT3DDEVICE_PRESENT_FAILED] = L"BasicDrawDevice: (inf) IDirect3DDevice::Present 失败/HR=%1";
    string_tables[IDS_TVP_CHANGE_DISPLAY_SETTINGS_FAILED_DISP_CHANGE_RESTART] = L"ChangeDisplaySettings 失败: DISP_CHANGE_RESTART";
    string_tables[IDS_TVP_CHANGE_DISPLAY_SETTINGS_FAILED_DISP_CHANGE_BAD_FLAGS] = L"ChangeDisplaySettings 失败: DISP_CHANGE_BADFLAGS";
    string_tables[IDS_TVP_CHANGE_DISPLAY_SETTINGS_FAILED_DISP_CHANGE_BAD_PARAM] = L"ChangeDisplaySettings 失败: DISP_CHANGE_BADPARAM";
    string_tables[IDS_TVP_CHANGE_DISPLAY_SETTINGS_FAILED_DISP_CHANGE_FAILED] = L"ChangeDisplaySettings 失败: DISP_CHANGE_FAILED";
    string_tables[IDS_TVP_CHANGE_DISPLAY_SETTINGS_FAILED_DISP_CHANGE_BAD_MODE] = L"ChangeDisplaySettings 失败: DISP_CHANGE_BADMODE";
    string_tables[IDS_TVP_CHANGE_DISPLAY_SETTINGS_FAILED_DISP_CHANGE_NOT_UPDATED] = L"ChangeDisplaySettings 失败: DISP_CHANGE_NOTUPDATED";
    string_tables[IDS_TVP_CHANGE_DISPLAY_SETTINGS_FAILED_UNKNOWN_REASON] = L"ChangeDisplaySettings 失败: DISP_CHANGE_RESTART";
    string_tables[IDS_TVP_FAILED_TO_CREATE_SCREEN_DC] = L"创建在屏（screen） DC 失败";
    string_tables[IDS_TVP_FAILED_TO_CREATE_OFFSCREEN_BITMAP] = L"创建离屏（offscreen）bitmap 失败";
    string_tables[IDS_TVP_FAILED_TO_CREATE_OFFSCREEN_DC] = L"创建离屏（offscreen）DC 失败";
    string_tables[IDS_TVP_INFO_SUSIE_PLUGIN_INFO] = L"（info）Susie 插件信息：%1";
    string_tables[IDS_TVP_SUSIE_PLUGIN_UNSUPPORTED_BITMAP_HEADER] = L"所给定的位图文件头不被 Susie 插件支持。";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_FAILED_TO_CREATE_DIRECT3DDEVICE] = L"BasicDrawDevice: 创建 Direct3D 设备失败: %1";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_FAILED_TO_CREATE_DIRECT3DDEVICE_UNKNOWN_REASON] = L"BasicDrawDevice: 创建 Direct3D 失败: 未知原因";
    string_tables[IDS_TVP_COULD_NOT_CREATE_ANY_DRAW_DEVICE] = L"（fatal）不能创建任何绘图对象。";
    string_tables[IDS_TVP_BASIC_DRAW_DEVICE_DOES_NOT_SUPPORTE_LAYER_MANAGER_MORE_THAN_ONE] = L"不支持超过 1 个以上的 layer 管理器 'Basic Draw'";
    string_tables[IDS_TVP_INVALID_VIDEO_SIZE] = L"无效的视频尺寸";
    string_tables[IDS_TVP_ROUGH_VSYNC_INTERVAL_READ_FROM_API] = L"从 Rough VSync API 中读取：%1";
    string_tables[IDS_TVP_ROUGH_VSYNC_INTERVAL_STILL_SEEMS_WRONG] = L"Rough VSync 内部仍旧有错误，假定为默认值（16）";
    string_tables[IDS_TVP_INFO_FOUND_DIRECT3DINTERFACE] = L"（info）未找到 IDirect3D9 或更新的接口。获取正在使用的 Direct3D 驱动的信息。";
    string_tables[IDS_TVP_INFO_FAILD] = L"（info）失败。";
    string_tables[IDS_TVP_INFO_DIRECT3D] = L"（info）载入 Direct3D...";
    string_tables[IDS_TVP_CANNOT_LOAD_D3DDLL] = L"无法加载 d3d9.dll";
    string_tables[IDS_TVP_NOT_FOUND_DIRECT3DCREATE] = L"在 d3d9.dll 中未找到 Direct3DCreate9 函数。";
    string_tables[IDS_TVP_INFO_ENVIRONMENT_USING] = L"（info）environment：使用 %1";
    string_tables[IDS_TVP_INFO_SEARCH_BEST_FULLSCREEN_RESOLUTION] = L"（info）查找全屏最佳分辨率中...";
    string_tables[IDS_TVP_INFO_CONDITION_PREFERRED_SCREEN_MODE] = L"（info）condition：推荐屏幕模式：%1";
    string_tables[IDS_TVP_INFO_CONDITION_MODE] = L"（info）condition：模式：%1";
    string_tables[IDS_TVP_INFO_CONDITION_ZOOM_MODE] = L"（info）condition：缩放模式：%1";
    string_tables[IDS_TVP_INFO_ENVIRONMENT_DEFAULT_SCREEN_MODE] = L"（info）environment：默认屏幕模式：%1";
    string_tables[IDS_TVP_INFO_ENVIRONMENT_DEFAULT_SCREEN_ASPECT_RATIO] = L"（info）environment：默认屏幕比例：%1:%2";
    string_tables[IDS_TVP_INFO_ENVIRONMENT_AVAILABLE_DISPLAY_MODES] = L"（info）environment：可用显示模式：";
    string_tables[IDS_TVP_INFO_NOT_FOUND_SCREEN_MODE_FROM_DRIVER] = L"（info）Panic！未从驱动中找到合适的屏幕模式。尝试使用默认画面尺寸和色彩数。";
    string_tables[IDS_TVP_INFO_RESULT_CANDIDATES] = L"（info）result： 候选：";
    string_tables[IDS_TVP_INFO_TRY_SCREEN_MODE] = L"（info）尝试使用 %1 屏幕显示模式。";
    string_tables[IDS_TVP_ALL_SCREEN_MODE_ERROR] = L"所有屏幕显示模式都已测试，未找到可用的。";
    string_tables[IDS_TVP_INFO_CHANGE_SCREEN_MODE_SUCCESS] = L"（info）成功切换屏幕显示模式。";
    string_tables[IDS_TVP_SELECT_XP3FILE_OR_FOLDER] = L"请选择 XP3 封包或文件夹。";
    string_tables[IDS_TVP_D3D_ERR_DEVICE_LOST] = L"D3D：设备已失效，但当前无法进行重置。例如：有其他进程进行过独占式全屏或输出屏幕进行过切换。";
    string_tables[IDS_TVP_D3D_ERR_DRIVER_IINTERNAL_ERROR] = L"D3D：内部驱动错误。请关闭程序后重新启动。";
    string_tables[IDS_TVP_D3D_ERR_INVALID_CALL] = L"D3D：调用了无效的方法。例如：函数的参数不是一个有效的指针（pointer）。";
    string_tables[IDS_TVP_D3D_ERR_OUT_OF_VIDEO_MEMORY] = L"D3D：Direct3D 没有足够的显存来执行该操作。";
    string_tables[IDS_TVP_D3D_ERR_OUT_OF_MEMORY] = L"D3D：Direct3D 不能分配有效的内存来调用。";
    string_tables[IDS_TVP_D3D_ERR_WRONG_TEXTURE_FORMAT] = L"D3D：错误的纹理格式。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPORTED_COLOR_OPERATION] = L"D3D：不支持的色彩操作类型。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPORTED_COLOR_ARG] = L"D3D：不支持的色彩参数。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPORTED_AALPHT_OPERATION] = L"D3D：不支持的 alpha 通道操作类型。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPORTED_ALPHA_ARG] = L"D3D：不支持的 alpha 通道参数类型。";
    string_tables[IDS_TVP_D3D_ERR_TOO_MANY_OPERATIONS] = L"D3D：此应用程序正在请求进行多于设备支持数量的纹理过滤操作。";
    string_tables[IDS_TVP_D3D_ERR_CONFLICTIONING_TEXTURE_FILTER] = L"D3D：当前纹理过滤不能同时使用。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPORTED_FACTOR_VALUE] = L"D3D：不支持的纹理因子值：未被使用或仅用于老旧设备。";
    string_tables[IDS_TVP_D3D_ERR_CONFLICTIONING_RENDER_STATE] = L"D3D：不能同时设置渲染状态。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPPORTED_TEXTURE_FILTER] = L"D3D：不支持的纹理过滤类型。";
    string_tables[IDS_TVP_D3D_ERR_CONFLICTIONING_TEXTURE_PALETTE] = L"D3D：不能同时使用当前纹理。";
    string_tables[IDS_TVP_D3D_ERR_NOT_FOUND] = L"D3D：未找到项目未找到。";
    string_tables[IDS_TVP_D3D_ERR_MORE_DATA] = L"D3D：可以声明更多的缓冲数据。";
    string_tables[IDS_TVP_D3D_ERR_DEVICE_NOT_RESET] = L"D3D：设备已失效，当前可以进行重置。";
    string_tables[IDS_TVP_D3D_ERR_NOT_AVAILABLE] = L"D3D：当前设备不支持所查询的技术。";
    string_tables[IDS_TVP_D3D_ERR_INVALID_DEVICE] = L"D3D：请求的设备类型无效。";
    string_tables[IDS_TVP_D3D_ERR_DRIVER_INVALID_CALL] = L"未使用。";
    string_tables[IDS_TVP_D3D_ERR_WAS_STILL_DRAWING] = L"D3D：正在进行 blit 操作或未完成操作。";
    string_tables[IDS_TVP_D3D_ERR_DEVICE_HUNG] = L"D3D：硬件适配器被系统重置。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPPORTED_OVERLAY] = L"D3D：当前设备不支持指定的尺寸或者显示模式。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPPORTED_OVERLAY_FORMAT] = L"D3D：当前设备不支持在指定的 surface 格式上 overlay。";
    string_tables[IDS_TVP_D3D_ERR_CANNOT_PROTECT_CONTENT] = L"D3D：指定的内容无法被保护。";
    string_tables[IDS_TVP_D3D_ERR_UNSUPPORTED_CRYPTO] = L"D3D：不支持指定的加密算法。";
    string_tables[IDS_TVP_D3D_ERR_PRESENT_STATISTICS_DIS_JOINT] = L"D3D：当前状态下无有序序列。";
    string_tables[IDS_TVP_D3D_ERR_DEVICE_REMOVED] = L"D3D：硬件适配器已被移除。";
    string_tables[IDS_TVP_D3D_OK_NO_AUTO_GEN] = L"D3D：成功。但是，此格式不支持自动 mipmap。";
    string_tables[IDS_TVP_D3D_ERR_FAIL] = L"D3D：Direct3D 子系统内发生未知错误。";
    string_tables[IDS_TVP_D3D_ERR_INVALID_ARG] = L"D3D：回调函数接受到无效的参数。";
    string_tables[IDS_TVP_D3D_UNKNOWN_ERROR] = L"D3D：未知错误。";
    string_tables[IDS_TVP_EXCEPTION_ACCESS_VIOLATION] = L"Access Violation：该线程尝试读取或写入一个无权访问的虚拟地址。";
    string_tables[IDS_TVP_EXCEPTION_BREAKPOINT] = L"Break Point：触发断点。";
    string_tables[IDS_TVP_EXCEPTION_DATATYPE_MISALIGNMENT] = L"Data Type Misalignment：线程尝试在未提供自动对齐的设备上写入未经对齐的数据。";
    string_tables[IDS_TVP_EXCEPTION_SINGLE_STEP] = L"Single Step： 标志寄存器的TF位为1时，每执行一条指令就会引发该异常。主要用于单步调试。";
    string_tables[IDS_TVP_EXCEPTION_ARRAY_BOUNDS_EXCEEDED] = L"Array Bounds Exceede：线程尝试越界访问数组元素，边界检查由硬件底层实现。";
    string_tables[IDS_TVP_EXCEPTION_FLT_DENORMAL_OPERAND] = L"Denormal Operand：浮点运算操作数异常，由于异常值太小而无法表达为标准浮点值。";
    string_tables[IDS_TVP_EXCEPTION_FLT_DIVIDE_BY_ZERO] = L"Divide By Zero：线程尝试用一个浮点数进行零除。";
    string_tables[IDS_TVP_EXCEPTION_FLT_INEXACT_RESULT] = L"Inexact Result：浮点运算的结果不能精确地表示为小数。";
    string_tables[IDS_TVP_EXCEPTION_FLT_INVALID_OPERATION] = L"Invalid Operation：无效的运算符操作。";
    string_tables[IDS_TVP_EXCEPTION_FLT_OVERFLOW] = L"Overflow：浮点运算的指数大于相应类型允许的量值。";
    string_tables[IDS_TVP_EXCEPTION_FLT_STACK_CHECK] = L"Stack Check：由于浮点操作，堆栈已溢出或正在溢出。";
    string_tables[IDS_TVP_EXCEPTION_FLT_UNDERFLOW] = L"Underflow：浮点运算的指数小于相应类型允许的幅度。";
    string_tables[IDS_TVP_EXCEPTION_INT_DIVIDE_BY_ZERO] = L"Divide By Zero：线程尝试用一个整数进行零除。";
    string_tables[IDS_TVP_EXCEPTION_INT_OVERFLOW] = L"Overflow：整数运算结果溢出。在某些情况下，这将导致某些操作无法设置进位标志。";
    string_tables[IDS_TVP_EXCEPTION_PRIV_INSTRUCTION] = L"Priv Instruction：线程尝试执行一条当前CPU模式下不允许的指令。";
    string_tables[IDS_TVP_EXCEPTION_NONCONTINUABLE_EXCEPTION] = L"Noncontinuable Exception：在不可继续执行的错误发生后，线程依然尝试继续执行。";
    string_tables[IDS_TVP_EXCEPTION_GUARD_PAGE] = L"Guard Page：线程访问了分配在 PAGE_GUARD 修饰符上的内存。";
    string_tables[IDS_TVP_EXCEPTION_ILLEGAL_INSTRUCTION] = L"Illegal Instruction：线程尝试执行一个无效的指令集。";
    string_tables[IDS_TVP_EXCEPTION_IN_PAGE_ERROR] = L"In Page Error：线程尝试访问不存在的内存页，且系统无法加载内存页。例如：当通过网络通信时网络断开，则可能触发此错误。";
    string_tables[IDS_TVP_EXCEPTION_INVALID_DISPOSITION] = L"Invalid Disposition：异常处理程序向异常调度程序返回了一个无效值，在高级编程语言中（如C、C++）不应该出现此错误。";
    string_tables[IDS_TVP_EXCEPTION_INVALID_HANDLE] = L"Invalid Handle：线程使用了一个无效的内核句柄（可能因为已被关闭）。";
    string_tables[IDS_TVP_EXCEPTION_STACK_OVERFLOW] = L"Stack Overflow：线程栈溢出。";
    string_tables[IDS_TVP_EXCEPTION_UNWIND_CCONSOLIDATE] = L"Unwind Consolidate：已进行帧合并。";
    string_tables[IDS_TVP_CANNOT_SHOW_MODAL_AREADY_SHOWED] = L"弹窗已显示或无法显示。";
    string_tables[IDS_TVP_CANNOT_SHOW_MODAL_SINGLE_WINDOW] = L"不能在单窗口模式下显示弹窗。";
}
