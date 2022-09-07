#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEBATCHCONVFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEBATCHCONVFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageBatchConvForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			UI::GUILabel *lblDir;
			UI::GUITextBox *txtDir;
			UI::GUIButton *btnBrowse;
			UI::GUILabel *lblOutFormat;
			UI::GUIRadioButton *radFormatJPG;
			UI::GUIRadioButton *radFormatWebP;
			UI::GUILabel *lblQuality;
			UI::GUITextBox *txtQuality;
			UI::GUICheckBox *chkSubdir;
			UI::GUITextBox *txtSubdir;
			UI::GUIButton *btnConvert;

			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnConvertClicked(void *userObj);
		public:
			AVIRImageBatchConvForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRImageBatchConvForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
