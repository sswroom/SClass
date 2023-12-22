#ifndef _SM_SSWR_AVIREAD_AVIREXEFORM
#define _SM_SSWR_AVIREAD_AVIREXEFORM
#include "Data/ArrayListStringNN.h"
#include "IO/EXEFile.h"
#include "Manage/DasmX86_16.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRExeForm : public UI::GUIForm
		{
		public:
			typedef struct
			{
				UInt16 segm;
				UInt16 addr;
				UInt16 endAddr;
				Data::ArrayListStringNN *codeList;
			} ExeB16Addr;
		private:
			NotNullPtr<UI::GUITabControl> tcEXE;

			NotNullPtr<UI::GUITabPage> tpProp;
			NotNullPtr<UI::GUIListView> lvProp;

			NotNullPtr<UI::GUITabPage> tpImport;
			UI::GUIListBox *lbImport;
			NotNullPtr<UI::GUIHSplitter> hspImport;
			NotNullPtr<UI::GUIListView> lvImport;

			NotNullPtr<UI::GUITabPage> tpExport;
			UI::GUIListBox *lbExport;

			NotNullPtr<UI::GUITabPage> tpResource;
			UI::GUIListBox *lbResource;
			NotNullPtr<UI::GUIHSplitter> hspResource;
			NotNullPtr<UI::GUITextBox> txtResource;

			NotNullPtr<UI::GUITabPage> tp16Bit;
			NotNullPtr<UI::GUIPanel> pnl16BitInfo;
			UI::GUIListBox *lb16BitFuncs;
			NotNullPtr<UI::GUIHSplitter> hsp16Bit;
			UI::GUIListBox *lb16BitCont;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::EXEFile *exeFile;
			Data::ArrayList<ExeB16Addr*> *parts;
			Data::ArrayList<Data::ArrayListStringNN*> *codesList;

			void ParseSess16(Manage::DasmX86_16::DasmX86_16_Sess *sess, Data::ArrayListStringNN *codes, Data::ArrayList<ExeB16Addr*> *parts, Data::ArrayListInt32 *partIndex, ExeB16Addr *startAddr, Manage::DasmX86_16 *dasm, UOSInt codeSize);
			void InitSess16();
			static void __stdcall On16BitFuncsChg(void *userObj);
			static void __stdcall OnImportSelChg(void *userObj);
			static void __stdcall OnExportDblClk(void *userObj);
			static void __stdcall OnResourceSelChg(void *userObj);
			static void __stdcall OnResourceDblClk(void *userObj);
		public:
			AVIRExeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::EXEFile *exeFile);
			virtual ~AVIRExeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
