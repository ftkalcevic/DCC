#ifndef COMBOBOX_HPP
#define COMBOBOX_HPP

#include <gui_generated/containers/ComboBoxBase.hpp>

class ComboBox : public ComboBoxBase
{
	class Lock
	{
		bool &locked;
	public:
		Lock(bool &l) :locked(l) {assert(!locked); locked = true;}
		~Lock() { locked = false; }
	};
	
public:
    ComboBox();
    virtual ~ComboBox() {}

    virtual void initialize();
	virtual void scrollWheelUpdateItem(ComboItem& item, int16_t itemIndex);
	void setUpdateItemCallback( GenericCallback<ComboBox&, ComboItem&, int16_t> *callback ) { updateCallback = callback; }
	void setSelectionChangedCallback( GenericCallback<ComboBox&, int16_t> *callback ) { selectionChangedCallback = callback; }

    void setNumberOfItems(int16_t n) { scrollWheel.setNumberOfItems(n); }
	void setSelectedItem(int16_t index, bool animate = false) { Lock l(selectingNewItem); scrollWheel.animateToItem(index, animate ? -1 : 0); }
protected:
    GenericCallback<ComboBox&, ComboItem&, int16_t> * updateCallback;   // The callback to be executed to paint an item
    GenericCallback<ComboBox&, int16_t> * selectionChangedCallback;     // The callback when the selection changes

    Callback<ComboBox, int16_t> cboAnimateToPositionCallback; // The callback to be executed to paint an item
    void cboAnimateToPositionHandler(int16_t index);
	bool selectingNewItem;
};

#endif // COMBOBOX_HPP
