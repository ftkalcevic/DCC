#ifndef COMBOBOX_HPP
#define COMBOBOX_HPP

#include <gui/containers/ComboBoxBase.hpp>
#include <vector>

class ComboBox : public ComboBoxBase
{
	struct ItemData
	{
		const char16_t *str;
		int id;
		bool heap;
	};
	
	std::vector<ItemData> itemList;
public:
    ComboBox();
	virtual ~ComboBox();

	virtual void addComboItem(const char16_t *item, int id);
	virtual void addComboItem(const char *item, int id);
	void setSelectionChangedCallback( GenericCallback<ComboBoxBase&, int16_t, int16_t> *callback ) { selectionChangedCallback = callback; }
	
	void setSelectedId(int16_t id) 
	{ 
		for (int i = 0; i < itemList.size(); i++)
		{
			if (itemList[i].id == id)	
			{
				setSelectedItem(i);
				break;
			}
		}
	}
	int getSelectedId() const 
	{ 
		int index = getSelectedItem();
		return itemList[index].id;
	}
	
protected:
    touchgfx::Callback<ComboBox, ComboBoxBase&, ComboItem&, int16_t> updateItemCallback;
    void updateItemHandler(ComboBoxBase&, ComboItem&, int16_t);
    touchgfx::Callback<ComboBox, ComboBoxBase&, int16_t> selectedIndexChangedCallback;
    void selectedIndexChangedHandler(ComboBoxBase&, int16_t);
	
    GenericCallback<ComboBoxBase&, int16_t, int16_t> * selectionChangedCallback;     // The callback when the selection changes
};

#endif // COMBOBOXBASE_HPP
