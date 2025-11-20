#ifndef _SM_SSWR_AVIREADCE_AVIRCEBASEFORM
#define _SM_SSWR_AVIREADCE_AVIRCEBASEFORM
#include "Data/FastMapNN.hpp"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace AVIReadCE
	{
		class AVIRCEBaseForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NN<Text::String> name;
				Int32 item;
			} MenuInfo;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUIListBox> lbCategory;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUIListBox> lbContent;
			Data::Int32FastMapNN<Data::ArrayListNN<MenuInfo>> menuItems;

		private:
			static void __stdcall FileHandler(AnyType userObj, const UTF8Char **files, UOSInt nFiles);
			static void __stdcall OnCategoryChg(AnyType userObj);
			static void __stdcall OnContentClick(AnyType userObj);

			static NN<MenuInfo> __stdcall NewMenuItem(Text::CStringNN name, Int32 item);
		public:
			AVIRCEBaseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCEBaseForm();

			virtual void EventMenuClicked(UInt16 cmdId);
		};
	}
}
#endif
