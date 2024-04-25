#ifndef _SM_SSWR_AVIREADCE_AVIRCEBASEFORM
#define _SM_SSWR_AVIREADCE_AVIRCEBASEFORM
#include "Data/FastMap.h"
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
				const WChar *name;
				Int32 item;
			} MenuInfo;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUIListBox> lbCategory;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUIListBox> lbContent;
			Data::Int32FastMap<Data::ArrayList<MenuInfo*>*> *menuItems;

		private:
			static void __stdcall FileHandler(void *userObj, const UTF8Char **files, OSInt nFiles);
			static void __stdcall OnCategoryChg(void *userObj);
			static void __stdcall OnContentClick(void *userObj);

			static MenuInfo *__stdcall NewMenuItem(const WChar *name, Int32 item);
		public:
			AVIRCEBaseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCEBaseForm();

			virtual void EventMenuClicked(UInt16 cmdId);
		};
	};
};
#endif
