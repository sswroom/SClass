#ifndef _SM_SSWR_AVIREAD_AVIRCOLORSETTINGFORM
#define _SM_SSWR_AVIREAD_AVIRCOLORSETTINGFORM
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHScrollBar.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRColorSettingForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ColorManager *colorMgr;
			NotNullPtr<Media::MonitorColorManager> monColor;
			Bool normalClose;
			Text::String *monFileName;
			MonitorHandle *hMon;

			NotNullPtr<UI::GUIPanel> pnlMonitor;
			NotNullPtr<UI::GUILabel> lblMonitor;
			NotNullPtr<UI::GUITextBox> txtMonitor;
			NotNullPtr<UI::GUIPanel> pnlButtons;
			NotNullPtr<UI::GUITabControl> tcMain;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<UI::GUITabPage> tpSetting;
			NotNullPtr<UI::GUILabel> lblMonProfile;
			NotNullPtr<UI::GUIComboBox> cboMonProfile;
			NotNullPtr<UI::GUIButton> btnMonProfile;
			NotNullPtr<UI::GUIGroupBox> grpMonProfile;
			NotNullPtr<UI::GUILabel> lblMonTran;
			NotNullPtr<UI::GUITextBox> txtMonTran;
			NotNullPtr<UI::GUILabel> lblMonRGBGamma;
			NotNullPtr<UI::GUITextBox> txtMonRGBGamma;
			NotNullPtr<UI::GUILabel> lblMonPrimaries;
			NotNullPtr<UI::GUITextBox> txtMonPrimaries;
			NotNullPtr<UI::GUIButton> btnMonCustom;
			NotNullPtr<UI::GUILabel> lblDefVProfile;
			NotNullPtr<UI::GUIComboBox> cboDefVProfile;
			NotNullPtr<UI::GUILabel> lblDefPProfile;
			NotNullPtr<UI::GUIComboBox> cboDefPProfile;
			NotNullPtr<UI::GUICheckBox> chk10Bit;
			NotNullPtr<UI::GUILabel> lblLuminance;
			NotNullPtr<UI::GUITextBox> txtLuminance;
			NotNullPtr<UI::GUILabel> lblLuminanceUnit;
			NotNullPtr<UI::GUIGroupBox> grpYUVDef;
			UI::GUIRadioButton *radYUVDefBT601;
			UI::GUIRadioButton *radYUVDefBT709;
			UI::GUIRadioButton *radYUVDefFCC;
			UI::GUIRadioButton *radYUVDefBT470BG;
			UI::GUIRadioButton *radYUVDefSMPTE170M;
			UI::GUIRadioButton *radYUVDefSMPTE240M;

			NotNullPtr<UI::GUITabPage> tpCal;
			NotNullPtr<UI::GUIPanel> pnlCalRed;
			NotNullPtr<UI::GUILabel> lblCalRedB;
			UI::GUIHScrollBar *hsbCalRedB;
			NotNullPtr<UI::GUILabel> lblCalRedBV;
			NotNullPtr<UI::GUILabel> lblCalRedC;
			UI::GUIHScrollBar *hsbCalRedC;
			NotNullPtr<UI::GUILabel> lblCalRedCV;
			NotNullPtr<UI::GUILabel> lblCalRedG;
			UI::GUIHScrollBar *hsbCalRedG;
			NotNullPtr<UI::GUILabel> lblCalRedGV;
			NotNullPtr<UI::GUIPanel> pnlCalGreen;
			NotNullPtr<UI::GUILabel> lblCalGreenB;
			UI::GUIHScrollBar *hsbCalGreenB;
			NotNullPtr<UI::GUILabel> lblCalGreenBV;
			NotNullPtr<UI::GUILabel> lblCalGreenC;
			UI::GUIHScrollBar *hsbCalGreenC;
			NotNullPtr<UI::GUILabel> lblCalGreenCV;
			NotNullPtr<UI::GUILabel> lblCalGreenG;
			UI::GUIHScrollBar *hsbCalGreenG;
			NotNullPtr<UI::GUILabel> lblCalGreenGV;
			NotNullPtr<UI::GUIPanel> pnlCalBlue;
			NotNullPtr<UI::GUILabel> lblCalBlueB;
			UI::GUIHScrollBar *hsbCalBlueB;
			NotNullPtr<UI::GUILabel> lblCalBlueBV;
			NotNullPtr<UI::GUILabel> lblCalBlueC;
			UI::GUIHScrollBar *hsbCalBlueC;
			NotNullPtr<UI::GUILabel> lblCalBlueCV;
			NotNullPtr<UI::GUILabel> lblCalBlueG;
			UI::GUIHScrollBar *hsbCalBlueG;
			NotNullPtr<UI::GUILabel> lblCalBlueGV;

			NotNullPtr<UI::GUITabPage> tpRGB;
			NotNullPtr<UI::GUILabel> lblMonVBrig;
			UI::GUIHScrollBar *hsbMonVBrig;
			NotNullPtr<UI::GUILabel> lblMonVBrigV;
			NotNullPtr<UI::GUILabel> lblMonPBrig;
			UI::GUIHScrollBar *hsbMonPBrig;
			NotNullPtr<UI::GUILabel> lblMonPBrigV;
			NotNullPtr<UI::GUILabel> lblMonRBrig;
			UI::GUIHScrollBar *hsbMonRBrig;
			NotNullPtr<UI::GUILabel> lblMonRBrigV;
			NotNullPtr<UI::GUILabel> lblMonGBrig;
			UI::GUIHScrollBar *hsbMonGBrig;
			NotNullPtr<UI::GUILabel> lblMonGBrigV;
			NotNullPtr<UI::GUILabel> lblMonBBrig;
			UI::GUIHScrollBar *hsbMonBBrig;
			NotNullPtr<UI::GUILabel> lblMonBBrigV;

			NotNullPtr<UI::GUITabPage> tpYUV;
			NotNullPtr<UI::GUILabel> lblYUVBrig;
			UI::GUIHScrollBar *hsbYUVBrig;
			NotNullPtr<UI::GUILabel> lblYUVBrigV;
			NotNullPtr<UI::GUILabel> lblYUVCont;
			UI::GUIHScrollBar *hsbYUVCont;
			NotNullPtr<UI::GUILabel> lblYUVContV;
			NotNullPtr<UI::GUILabel> lblYUVSat;
			UI::GUIHScrollBar *hsbYUVSat;
			NotNullPtr<UI::GUILabel> lblYUVSatV;
			NotNullPtr<UI::GUILabel> lblYUVYGamma;
			UI::GUIHScrollBar *hsbYUVYGamma;
			NotNullPtr<UI::GUILabel> lblYUVYGammaV;
			NotNullPtr<UI::GUILabel> lblYUVCGamma;
			UI::GUIHScrollBar *hsbYUVCGamma;
			NotNullPtr<UI::GUILabel> lblYUVCGammaV;

			static void __stdcall OnMonProfileChg(void *userObj);
			static void __stdcall OnMonProfileClicked(void *userObj);
			static void __stdcall OnMonCustomClicked(void *userObj);
			static void __stdcall OnDefVProfileChg(void *userObj);
			static void __stdcall OnDefPProfileChg(void *userObj);
			static void __stdcall On10BitChanged(void *userObj, Bool newVal);

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);

			static void __stdcall OnCalRedBChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalRedCChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalRedGChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalGreenBChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalGreenCChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalGreenGChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalBlueBChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalBlueCChg(void *userObj, UOSInt newPos);
			static void __stdcall OnCalBlueGChg(void *userObj, UOSInt newPos);

			static void __stdcall OnMonVBrigChg(void *userObj, UOSInt newPos);
			static void __stdcall OnMonPBrigChg(void *userObj, UOSInt newPos);
			static void __stdcall OnMonRBrigChg(void *userObj, UOSInt newPos);
			static void __stdcall OnMonGBrigChg(void *userObj, UOSInt newPos);
			static void __stdcall OnMonBBrigChg(void *userObj, UOSInt newPos);

			static void __stdcall OnYUVBrigChg(void *userObj, UOSInt newPos);
			static void __stdcall OnYUVContChg(void *userObj, UOSInt newPos);
			static void __stdcall OnYUVSatChg(void *userObj, UOSInt newPos);
			static void __stdcall OnYUVYGammaChg(void *userObj, UOSInt newPos);
			static void __stdcall OnYUVCGammaChg(void *userObj, UOSInt newPos);
			static void __stdcall OnYUVDefBT601Chg(void *userObj, Bool checked);
			static void __stdcall OnYUVDefBT709Chg(void *userObj, Bool checked);
			static void __stdcall OnYUVDefFCCChg(void *userObj, Bool checked);
			static void __stdcall OnYUVDefBT470BGChg(void *userObj, Bool checked);
			static void __stdcall OnYUVDefSMPTE170MChg(void *userObj, Bool checked);
			static void __stdcall OnYUVDefSMPTE240MChg(void *userObj, Bool checked);
			
		public:
			AVIRColorSettingForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, MonitorHandle *hMon);
			virtual ~AVIRColorSettingForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
