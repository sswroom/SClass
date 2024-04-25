#ifndef _SM_UI_GUICOMBOBOXUTIL
#define _SM_UI_GUICOMBOBOXUTIL
#include "UI/GUIComboBox.h"

#define CBOADDENUM(cbo, enumCls, enumVal) cbo->AddItem(enumCls ## GetName(enumCls::enumVal), (void*)(OSInt)(enumCls::enumVal))

namespace UI
{
	class GUIComboBoxUtil
	{
	public:
		static void AddYearItems(NN<UI::GUIComboBox> cbo, UOSInt nYears);
		static void AddMonthItems(NN<UI::GUIComboBox> cbo);
		static void AddDayItems(NN<UI::GUIComboBox> cbo);
	};
}
#endif
