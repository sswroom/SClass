#include "Stdafx.h"
#include "Text/MyString.h"
#include "UI/GUIComboBoxUtil.h"

void UI::GUIComboBoxUtil::AddYearItems(NN<UI::GUIComboBox> cbo, UIntOS nYears)
{
	UIntOS k;
	UTF8Char sbuff[6];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	dt.SetCurrTime();
	IntOS j = dt.GetYear();
	IntOS i = j - (IntOS)nYears;
	while (i < j)
	{
		i++;
		sptr = Text::StrIntOS(sbuff, i);
		k = cbo->AddItem(CSTRP(sbuff, sptr), (void*)i);
		if (i == j)
			cbo->SetSelectedIndex(k);
	}
}

void UI::GUIComboBoxUtil::AddMonthItems(NN<UI::GUIComboBox> cbo)
{
	Data::DateTime dt;
	dt.SetCurrTime();
	UIntOS k;
	UIntOS i = 0;
	UIntOS j = dt.GetMonth();
	while (i < 12)
	{
		k = cbo->AddItem(Text::CStringNN((const UTF8Char*)Data::DateTimeUtil::monString[i], 3), (void*)(i + 1));
		i++;
		if (i == j)
		{
			cbo->SetSelectedIndex(k);
		}
	}
}

void UI::GUIComboBoxUtil::AddDayItems(NN<UI::GUIComboBox> cbo)
{
	Data::DateTime dt;
	dt.SetCurrTime();
	UTF8Char sbuff[6];
	UnsafeArray<UTF8Char> sptr;
	UIntOS k;
	UIntOS i = 0;
	UIntOS j = dt.GetDay();
	while (i < 31)
	{
		i++;
		sptr = Text::StrUIntOS(sbuff, i);
		k = cbo->AddItem(CSTRP(sbuff, sptr), (void*)i);
		if (i == j)
		{
			cbo->SetSelectedIndex(k);
		}
	}
}
