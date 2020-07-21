#ifndef COMBOBOXBASE_HPP
#define COMBOBOXBASE_HPP

#include <gui_generated/containers/ComboBoxBaseBase.hpp>

class ComboBoxBase : public ComboBoxBaseBase
{
	class Lock
	{
		bool &locked;
	public:
		Lock(bool &l) :locked(l) {assert(!locked); locked = true;}
		~Lock() { locked = false; }
	};
	
public:
    ComboBoxBase();
    virtual ~ComboBoxBase() {}

    virtual void initialize();
	virtual void scrollWheelUpdateItem(ComboItem& item, int16_t itemIndex);
	void setUpdateItemCallback( GenericCallback<ComboBoxBase&, ComboItem&, int16_t> *callback ) { updateCallback = callback; }
	void setSelectedIndexChangedCallback( GenericCallback<ComboBoxBase&, int16_t> *callback ) { selectedIndexChangedCallback = callback; }

    void setNumberOfItems(int16_t n) { scrollWheel.setNumberOfItems(n); }
    uint16_t getNumberOfItems() const { return scrollWheel.getNumberOfItems(); }
	void setSelectedItem(int16_t index, bool animate = false) { Lock l(selectingNewItem); scrollWheel.animateToItem(index, animate ? -1 : 0); }
	int getSelectedItem() const { return scrollWheel.getSelectedItem(); }
	
protected:
    GenericCallback<ComboBoxBase&, ComboItem&, int16_t> * updateCallback;   // The callback to be executed to paint an item
    GenericCallback<ComboBoxBase&, int16_t> * selectedIndexChangedCallback;     // The callback when the selection changes

    Callback<ComboBoxBase, int16_t> cboAnimateToPositionCallback; // The callback to be executed to paint an item
    void cboAnimateToPositionHandler(int16_t index);
	bool selectingNewItem;
};

#endif // COMBOBOXBASE_HPP
