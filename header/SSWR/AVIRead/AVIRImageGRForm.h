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
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<Media::StaticImage> srcImg;
			NN<Media::StaticImage> destImg;
			Optional<Media::StaticImage> srcPrevImg;
			Optional<Media::StaticImage> destPrevImg;
			NN<UI::GUIPictureBoxDD> previewCtrl;
			NN<Media::GRFilter> grFilter;
			UOSInt currLayer;
			Bool modifying;

			NN<UI::GUIPanel> pnlLayers;
			NN<UI::GUIListBox> lbLayers;
			NN<UI::GUIHSplitter> hspLayers;
			NN<UI::GUIPanel> pnlSetting;
			NN<UI::GUILabel> lblHOfst;
			NN<UI::GUIHScrollBar> hsbHOfst;
			NN<UI::GUITextBox> txtHOfst;
			NN<UI::GUILabel> lblVOfst;
			NN<UI::GUIHScrollBar> hsbVOfst;
			NN<UI::GUITextBox> txtVOfst;
			NN<UI::GUILabel> lblLevel;
			NN<UI::GUIHScrollBar> hsbLevel;
			NN<UI::GUITextBox> txtLevel;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUICheckBox> chkEnable;
			NN<UI::GUIButton> btnAddLayer;
			NN<UI::GUIButton> btnRemoveLayer;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

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
			AVIRImageGRForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::StaticImage> srcImg, NN<Media::StaticImage> destImg, NN<UI::GUIPictureBoxDD> previewCtrl);
			virtual ~AVIRImageGRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
