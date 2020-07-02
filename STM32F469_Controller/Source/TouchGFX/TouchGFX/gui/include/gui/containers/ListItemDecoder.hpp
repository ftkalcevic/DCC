#ifndef LISTITEMDECODER_HPP
#define LISTITEMDECODER_HPP

#include <gui_generated/containers/ListItemDecoderBase.hpp>

class ListItemDecoder : public ListItemDecoderBase
{
public:
    ListItemDecoder();
    virtual ~ListItemDecoder() {}

    virtual void initialize();
	void setIndex(int i, bool selected);
protected:
	int index;
};

#endif // LISTITEMDECODER_HPP
