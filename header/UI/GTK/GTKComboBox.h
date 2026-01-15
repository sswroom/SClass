#ifndef _SM_UI_GTK_GTKCOMBOBOX
#define _SM_UI_GTK_GTKCOMBOBOX
#include "Data/ArrayListObj.hpp"
#include "UI/GUIComboBox.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKComboBox : public GUIComboBox
		{
		private:
			Data::ArrayListObj<AnyType> items;
			GtkTreeModel *model;
			UIntOS minVisible;
			Bool allowEdit;

		public:
			GTKComboBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool allowEditing);
			virtual ~GTKComboBox();

			virtual void SetText(Text::CStringNN text);
			virtual UnsafeArrayOpt<UTF8Char> GetText(UnsafeArray<UTF8Char> buff);
			virtual Bool GetText(NN<Text::StringBuilderUTF8> sb);

			virtual void BeginUpdate();
			virtual void EndUpdate();
			virtual UIntOS AddItem(NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj);
			virtual AnyType RemoveItem(UIntOS index);
			virtual void ClearItems();
			virtual UIntOS GetCount();
			virtual void SetSelectedIndex(UIntOS index);
			virtual UIntOS GetSelectedIndex();
			virtual AnyType GetSelectedItem();
			virtual AnyType GetItem(UIntOS index);

			virtual Math::Size2DDbl GetSize();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
			virtual void SetTextSelection(UIntOS startPos, UIntOS endPos);
		};
	}
}
#endif
