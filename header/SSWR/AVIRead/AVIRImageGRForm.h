#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEGRFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEGRFORM
#include "Media/GRFilter.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageGRForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;

			Media::StaticImage *srcImg;
			Media::StaticImage *destImg;
			Media::StaticImage *srcPrevImg;
			Media::StaticImage *destPrevImg;
			UI::GUIPictureBoxDD *previewCtrl;
			Media::GRFilter *grFilter;
			UOSInt currLayer;
			Bool modifying;

			UI::GUIPanel *pnlLayers;
			UI::GUIListBox *lbLayers;
			UI::GUIHSplitter *hspLayers;
			UI::GUIPanel *pnlSetting;
			UI::GUILabel *lblHOfst;
			UI::GUIHScrollBar *hsbHOfst;
			UI::GUITextBox *txtHOfst;
			UI::GUILabel *lblVOfst;
			UI::GUIHScrollBar *hsbVOfst;
			UI::GUITextBox *txtVOfst;
			UI::GUILabel *lblLevel;
			UI::GUIHScrollBar *hsbLevel;
			UI::GUITextBox *txtLevel;
			UI::GUILabel *lblType;
			UI::GUIComboBox *cboType;
			UI::GUICheckBox *chkEnable;
			UI::GUIButton *btnAddLayer;
			UI::GUIButton *btnRemoveLayer;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;

			static void __stdcall OnHOfstChanged(void *userObj, OSInt newPos);
			static void __stdcall OnVOfstChanged(void *userObj, OSInt newPos);
			static void __stdcall OnLevelChanged(void *userObj, OSInt newPos);
			static void __stdcall OnTypeChanged(void *userObj);
			static void __stdcall OnEnableChanged(void *userObj, Bool newVal);
			static void __stdcall OnAddLayerClicked(void *userObj);
			static void __stdcall OnRemoveLayerClicked(void *userObj);
			static void __stdcall OnLayersChanged(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

			void UpdatePreview();
			void UpdateLayers();
		public:
			AVIRImageGRForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::StaticImage *srcImg, Media::StaticImage *destImg, UI::GUIPictureBoxDD *previewCtrl);
			virtual ~AVIRImageGRForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
