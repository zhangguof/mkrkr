#ifndef __APPLICATION_SPECIAL_PATH_H__
#define __APPLICATION_SPECIAL_PATH_H__
#include <string>

class ApplicationSpecialPath {
public:
	static inline std::wstring ReplaceStringAll( std::wstring src, const std::wstring& target, const std::wstring& dest ) {
		std::wstring::size_type nPos = 0;
		while( (nPos = src.find(target, nPos)) != std::wstring::npos ) {
			src.replace( nPos, target.length(), dest );
		}
		return src;
	}
};
#endif