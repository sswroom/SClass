#ifndef _SM_SSWR_AVIREAD_AVIREXEFORM
#define _SM_SSWR_AVIREAD_AVIREXEFORM
#include "Data/ArrayListStrUTF8.h"
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
				Data::ArrayListString *codeList;
			} ExeB16Addr;
		private:
			UI::GUITabControl *tcEXE;

			UI::GUITabPage *tpProp;
			UI::GUIListView *lvProp;

			UI::GUITabPage *tpImport;
			UI::GUIListBox *lbImport;
			UI::GUIHSplitter *hspImport;
			UI::GUIListView *lvImport;

			UI::GUITabPage *tpExport;
			UI::GUIListBox *lbExport;

			UI::GUITabPage *tpResource;
			UI::GUIListBox *lbResource;
			UI::GUIHSplitter *hspResource;
			UI::GUITextBox *txtResource;

			UI::GUITabPage *tp16Bit;
			UI::GUIPanel *pnl16BitInfo;
			UI::GUIListBox *lb16BitFuncs;
			UI::GUIHSplitter *hsp16Bit;
			UI::GUIListBox *lb16BitCont;

			SSWR::AVIRead::AVIRCore *core;
			IO::EXEFile *exeFile;
			Data::ArrayList<ExeB16Addr*> *parts;
			Data::ArrayList<Data::ArrayListString*> *codesList;

			void ParseSess16(Manage::DasmX86_16::DasmX86_16_Sess *sess, Data::ArrayListString *codes, Data::ArrayList<ExeB16Addr*> *parts, Data::ArrayListInt32 *partIndex, ExeB16Addr *startAddr, Manage::DasmX86_16 *dasm, UOSInt codeSize);
			void InitSess16();
			static void __stdcall On16BitFuncsChg(void *userObj);
			static void __stdcall OnImportSelChg(void *userObj);
			static void __stdcall OnResourceSelChg(void *userObj);
			static void __stdcall OnResourceDblClk(void *userObj);
		public:
			AVIRExeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::EXEFile *exeFile);
			virtual ~AVIRExeForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
