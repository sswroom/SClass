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
			SSWR::AVIRead::AVIRCore *core;
			Media::Resizer::LanczosResizer8_C8 *resizer;
			Map::MapEnv *env;
			Parser::ParserList *parsers;
			UOSInt imgIndex;

			UI::GUILabel *lbl;
			UI::GUIPanel *pnlButtons;
			UI::GUIPictureList *plIcons;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnOKClick(void *userObj);
			static void __stdcall OnCancelClick(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, const UTF8Char **fileNames, OSInt fileCnt);
			void UpdateImages();
		public:
			AVIRGISImageForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, UOSInt imgIndex);
			virtual ~AVIRGISImageForm();

			virtual void OnMonitorChanged();

			UOSInt GetImgIndex();
		};
	}
}
#endif
