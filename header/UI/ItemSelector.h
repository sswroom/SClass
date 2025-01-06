#ifndef _SM_UI_ITEMSELECTOR
#define _SM_UI_ITEMSELECTOR
#include "AnyType.h"

namespace UI
{
	class ItemSelector
	{
	public:
		virtual UOSInt AddItem(Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual void SetSelectedIndex(UOSInt index) = 0;
		virtual AnyType GetSelectedItem() = 0;
	};
}
#endif
