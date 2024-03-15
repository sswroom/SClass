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
			NotNullPtr<UI::GUIPictureBoxDD> previewCtrl;
			Media::GRFilter *grFilter;
			UOSInt currLayer;
			Bool modifying;

			NotNullPtr<UI::GUIPanel> pnlLayers;
			NotNullPtr<UI::GUIListBox> lbLayers;
			NotNullPtr<UI::GUIHSplitter> hspLayers;
			NotNullPtr<UI::GUIPanel> pnlSetting;
			NotNullPtr<UI::GUILabel> lblHOfst;
			NotNullPtr<UI::GUIHScrollBar> hsbHOfst;
			NotNullPtr<UI::GUITextBox> txtHOfst;
			NotNullPtr<UI::GUILabel> lblVOfst;
			NotNullPtr<UI::GUIHScrollBar> hsbVOfst;
			NotNullPtr<UI::GUITextBox> txtVOfst;
			NotNullPtr<UI::GUILabel> lblLevel;
			NotNullPtr<UI::GUIHScrollBar> hsbLevel;
			NotNullPtr<UI::GUITextBox> txtLevel;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUICheckBox> chkEnable;
			NotNullPtr<UI::GUIButton> btnAddLayer;
			NotNullPtr<UI::GUIButton> btnRemoveLayer;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			static void __stdcall OnHOfstChanged(AnyType userObj, UOSInt newPos);
			static void __stdcall OnVOfstChanged(AnyType userObj, UOSInt newPos);
			static void __stdcall OnLevelChanged(AnyType userObj, UOSInt newPos);
			static void __stdcall OnTypeChanged(AnyType userObj);
			static void __stdcall OnEnableChanged(AnyType userObj, Bool newVal);
			static void __stdcall OnAddLayerClicked(AnyType userObj);
			static void __stdcall OnRemoveLayerClicked(AnyType userObj);
			static void __stdcall OnLayersChanged(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);

			void UpdatePreview();
			void UpdateLayers();
		public:
			AVIRImageGRForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::StaticImage> srcImg, NotNullPtr<Media::StaticImage> destImg, NotNullPtr<UI::GUIPictureBoxDD> previewCtrl);
			virtual ~AVIRImageGRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
