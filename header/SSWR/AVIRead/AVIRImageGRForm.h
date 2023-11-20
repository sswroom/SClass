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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			NotNullPtr<Media::StaticImage> srcImg;
			NotNullPtr<Media::StaticImage> destImg;
			NotNullPtr<Media::StaticImage> srcPrevImg;
			NotNullPtr<Media::StaticImage> destPrevImg;
			UI::GUIPictureBoxDD *previewCtrl;
			Media::GRFilter *grFilter;
			UOSInt currLayer;
			Bool modifying;

			NotNullPtr<UI::GUIPanel> pnlLayers;
			UI::GUIListBox *lbLayers;
			UI::GUIHSplitter *hspLayers;
			NotNullPtr<UI::GUIPanel> pnlSetting;
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

			static void __stdcall OnHOfstChanged(void *userObj, UOSInt newPos);
			static void __stdcall OnVOfstChanged(void *userObj, UOSInt newPos);
			static void __stdcall OnLevelChanged(void *userObj, UOSInt newPos);
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
			AVIRImageGRForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::StaticImage> srcImg, NotNullPtr<Media::StaticImage> destImg, UI::GUIPictureBoxDD *previewCtrl);
			virtual ~AVIRImageGRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
