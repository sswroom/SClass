#include "Stdafx.h"
#include "Text/MyString.h"
#include "UI/GUIComboBoxUtil.h"

void UI::GUIComboBoxUtil::AddYearItems(UI::GUIComboBox *cbo, UOSInt nYears)
{
	UOSInt k;
	UTF8Char sbuff[6];
	UTF8Char *sptr;
	Data::DateTime dt;
	dt.SetCurrTime();
	UOSInt j = dt.GetYear();
	UOSInt i = j - nYears;
	while (i < j)
	{
		i++;
		sptr = Text::StrUOSInt(sbuff, i);
		k = cbo->AddItem(CSTRP(sbuff, sptr), (void*)i);
		if (i == j)
			cbo->SetSelectedIndex(k);
	}
}

void UI::GUIComboBoxUtil::AddMonthItems(UI::GUIComboBox *cbo)
{
	Data::DateTime dt;
	dt.SetCurrTime();
	UOSInt k;
	UOSInt i = 0;
	UOSInt j = dt.GetMonth();
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

void UI::GUIComboBoxUtil::AddDayItems(UI::GUIComboBox *cbo)
{
	Data::DateTime dt;
	dt.SetCurrTime();
	UTF8Char sbuff[6];
	UTF8Char *sptr;
	UOSInt k;
	UOSInt i = 0;
	UOSInt j = dt.GetDay();
	while (i < 31)
	{
		i++;
		sptr = Text::StrUOSInt(sbuff, i);
		k = cbo->AddItem(CSTRP(sbuff, sptr), (void*)i);
		if (i == j)
		{
			cbo->SetSelectedIndex(k);
		}
	}
}
