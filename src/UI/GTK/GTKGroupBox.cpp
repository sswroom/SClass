#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKGroupBox.h"
#include <gtk/gtk.h>

UI::GTK::GTKGroupBox::GTKGroupBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN text) : UI::GUIGroupBox(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_frame_new((const Char*)text.v.Ptr());
	parent->AddChild(*this);
	this->Show();
}

Math::Coord2DDbl UI::GTK::GTKGroupBox::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::GTK::GTKGroupBox::GetClientSize()
{
	return this->GetSize() - Math::Double2D(6, 18);
}

UI::GTK::GTKGroupBox::~GTKGroupBox()
{
}
