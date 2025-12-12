#include "Stdafx.h"
#include "Java/JavaJOptionPane.h"
#include "UI/JavaUI/JUIMessageDialog.h"

void UI::JavaUI::JUIMessageDialog::ShowOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	Java::JavaJOptionPane::ShowMessageDialog(0, message, title, Java::JavaJOptionPane::MessageType::PLAIN_MESSAGE);
}

Bool UI::JavaUI::JUIMessageDialog::ShowYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	return Java::JavaJOptionPane::ShowConfirmDialog(0, message, title, Java::JavaJOptionPane::OptionType::YES_NO_OPTION, Java::JavaJOptionPane::MessageType::PLAIN_MESSAGE) == Java::JavaJOptionPane::SelectedOption::YES_OPTION;
}
