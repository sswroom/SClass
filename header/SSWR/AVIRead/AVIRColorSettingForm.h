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
			UI::GUILabel *lblMonitor;
			UI::GUITextBox *txtMonitor;
			NotNullPtr<UI::GUIPanel> pnlButtons;
			UI::GUITabControl *tcMain;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<UI::GUITabPage> tpSetting;
			UI::GUILabel *lblMonProfile;
			UI::GUIComboBox *cboMonProfile;
			NotNullPtr<UI::GUIButton> btnMonProfile;
			NotNullPtr<UI::GUIGroupBox> grpMonProfile;
			UI::GUILabel *lblMonTran;
			UI::GUITextBox *txtMonTran;
			UI::GUILabel *lblMonRGBGamma;
			UI::GUITextBox *txtMonRGBGamma;
			UI::GUILabel *lblMonPrimaries;
			UI::GUITextBox *txtMonPrimaries;
			NotNullPtr<UI::GUIButton> btnMonCustom;
			UI::GUILabel *lblDefVProfile;
			UI::GUIComboBox *cboDefVProfile;
			UI::GUILabel *lblDefPProfile;
			UI::GUIComboBox *cboDefPProfile;
			UI::GUICheckBox *chk10Bit;
			UI::GUILabel *lblLuminance;
			UI::GUITextBox *txtLuminance;
			UI::GUILabel *lblLuminanceUnit;
			NotNullPtr<UI::GUIGroupBox> grpYUVDef;
			UI::GUIRadioButton *radYUVDefBT601;
			UI::GUIRadioButton *radYUVDefBT709;
			UI::GUIRadioButton *radYUVDefFCC;
			UI::GUIRadioButton *radYUVDefBT470BG;
			UI::GUIRadioButton *radYUVDefSMPTE170M;
			UI::GUIRadioButton *radYUVDefSMPTE240M;

			NotNullPtr<UI::GUITabPage> tpCal;
			NotNullPtr<UI::GUIPanel> pnlCalRed;
			UI::GUILabel *lblCalRedB;
			UI::GUIHScrollBar *hsbCalRedB;
			UI::GUILabel *lblCalRedBV;
			UI::GUILabel *lblCalRedC;
			UI::GUIHScrollBar *hsbCalRedC;
			UI::GUILabel *lblCalRedCV;
			UI::GUILabel *lblCalRedG;
			UI::GUIHScrollBar *hsbCalRedG;
			UI::GUILabel *lblCalRedGV;
			NotNullPtr<UI::GUIPanel> pnlCalGreen;
			UI::GUILabel *lblCalGreenB;
			UI::GUIHScrollBar *hsbCalGreenB;
			UI::GUILabel *lblCalGreenBV;
			UI::GUILabel *lblCalGreenC;
			UI::GUIHScrollBar *hsbCalGreenC;
			UI::GUILabel *lblCalGreenCV;
			UI::GUILabel *lblCalGreenG;
			UI::GUIHScrollBar *hsbCalGreenG;
			UI::GUILabel *lblCalGreenGV;
			NotNullPtr<UI::GUIPanel> pnlCalBlue;
			UI::GUILabel *lblCalBlueB;
			UI::GUIHScrollBar *hsbCalBlueB;
			UI::GUILabel *lblCalBlueBV;
			UI::GUILabel *lblCalBlueC;
			UI::GUIHScrollBar *hsbCalBlueC;
			UI::GUILabel *lblCalBlueCV;
			UI::GUILabel *lblCalBlueG;
			UI::GUIHScrollBar *hsbCalBlueG;
			UI::GUILabel *lblCalBlueGV;

			NotNullPtr<UI::GUITabPage> tpRGB;
			UI::GUILabel *lblMonVBrig;
			UI::GUIHScrollBar *hsbMonVBrig;
			UI::GUILabel *lblMonVBrigV;
			UI::GUILabel *lblMonPBrig;
			UI::GUIHScrollBar *hsbMonPBrig;
			UI::GUILabel *lblMonPBrigV;
			UI::GUILabel *lblMonRBrig;
			UI::GUIHScrollBar *hsbMonRBrig;
			UI::GUILabel *lblMonRBrigV;
			UI::GUILabel *lblMonGBrig;
			UI::GUIHScrollBar *hsbMonGBrig;
			UI::GUILabel *lblMonGBrigV;
			UI::GUILabel *lblMonBBrig;
			UI::GUIHScrollBar *hsbMonBBrig;
			UI::GUILabel *lblMonBBrigV;

			NotNullPtr<UI::GUITabPage> tpYUV;
			UI::GUILabel *lblYUVBrig;
			UI::GUIHScrollBar *hsbYUVBrig;
			UI::GUILabel *lblYUVBrigV;
			UI::GUILabel *lblYUVCont;
			UI::GUIHScrollBar *hsbYUVCont;
			UI::GUILabel *lblYUVContV;
			UI::GUILabel *lblYUVSat;
			UI::GUIHScrollBar *hsbYUVSat;
			UI::GUILabel *lblYUVSatV;
			UI::GUILabel *lblYUVYGamma;
			UI::GUIHScrollBar *hsbYUVYGamma;
			UI::GUILabel *lblYUVYGammaV;
			UI::GUILabel *lblYUVCGamma;
			UI::GUIHScrollBar *hsbYUVCGamma;
			UI::GUILabel *lblYUVCGammaV;

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
