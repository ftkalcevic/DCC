#ifndef FILEITEM_HPP
#define FILEITEM_HPP

#include <gui_generated/containers/FileItemBase.hpp>

class FileItem : public FileItemBase
{
public:
    FileItem();
    virtual ~FileItem() {}

    virtual void initialize();
	
	void setText(const char16_t *s)
	{
		text.setWildcard((const Unicode::UnicodeChar*)s);
	}
protected:
};

#endif // FILEITEM_HPP
