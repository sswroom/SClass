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
				NN<Data::ArrayListStringNN> codeList;
			} ExeB16Addr;
		private:
			NN<UI::GUITabControl> tcEXE;

			NN<UI::GUITabPage> tpProp;
			NN<UI::GUIListView> lvProp;

			NN<UI::GUITabPage> tpImport;
			NN<UI::GUIListBox> lbImport;
			NN<UI::GUIHSplitter> hspImport;
			NN<UI::GUIListView> lvImport;

			NN<UI::GUITabPage> tpExport;
			NN<UI::GUIListBox> lbExport;

			NN<UI::GUITabPage> tpResource;
			NN<UI::GUIListBox> lbResource;
			NN<UI::GUIHSplitter> hspResource;
			NN<UI::GUITextBox> txtResource;

			NN<UI::GUITabPage> tp16Bit;
			NN<UI::GUIPanel> pnl16BitInfo;
			NN<UI::GUIListBox> lb16BitFuncs;
			NN<UI::GUIHSplitter> hsp16Bit;
			NN<UI::GUIListBox> lb16BitCont;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::EXEFile> exeFile;
			Optional<Data::ArrayListNN<ExeB16Addr>> parts;
			Optional<Data::ArrayListNN<Data::ArrayListStringNN>> codesList;

			void ParseSess16(NN<Manage::DasmX86_16::DasmX86_16_Sess> sess, NN<Data::ArrayListStringNN> codes, NN<Data::ArrayListNN<ExeB16Addr>> parts, NN<Data::ArrayListInt32> partIndex, NN<ExeB16Addr> startAddr, NN<Manage::DasmX86_16> dasm, UIntOS codeSize);
			void InitSess16();
			static void __stdcall On16BitFuncsChg(AnyType userObj);
			static void __stdcall OnImportSelChg(AnyType userObj);
			static void __stdcall OnExportDblClk(AnyType userObj);
			static void __stdcall OnResourceSelChg(AnyType userObj);
			static void __stdcall OnResourceDblClk(AnyType userObj);
		public:
			AVIRExeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::EXEFile> exeFile);
			virtual ~AVIRExeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
