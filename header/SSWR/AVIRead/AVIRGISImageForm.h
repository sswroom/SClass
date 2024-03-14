#ifndef _SM_SSWR_AVIREAD_AVIRGISIMAGEFORM
#define _SM_SSWR_AVIREAD_AVIRGISIMAGEFORM
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureList.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGISImageForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::Resizer::LanczosResizer8_C8 *resizer;
			NotNullPtr<Map::MapEnv> env;
			NotNullPtr<Parser::ParserList> parsers;
			UOSInt imgIndex;

			NotNullPtr<UI::GUILabel> lbl;
			NotNullPtr<UI::GUIPanel> pnlButtons;
			UI::GUIPictureList *plIcons;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnOKClick(AnyType userObj);
			static void __stdcall OnCancelClick(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> fileNames);
			void UpdateImages();
		public:
			AVIRGISImageForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, UOSInt imgIndex);
			virtual ~AVIRGISImageForm();

			virtual void OnMonitorChanged();

			UOSInt GetImgIndex();
		};
	}
}
#endif
