#ifndef _SM_SSWR_AVIREAD_AVIRSELENIUMLOGFORM
#define _SM_SSWR_AVIREAD_AVIRSELENIUMLOGFORM
#include "IO/PackageFile.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSeleniumLogForm : public UI::GUIForm
		{
		private:
			struct LogItem
			{
				Int64 startTime;
				Int64 endTime;
				NN<Text::String> status;
				NN<Text::String> title;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlControl;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIButton> btnCSV;
			NN<UI::GUIListView> lvContent;
			
			Data::ArrayListNN<LogItem> logItems;
			Optional<Text::String> srcFile;

			static void __stdcall OnBrowseClick(AnyType userObj);
			static void __stdcall OnCSVClick(AnyType userObj);
			static void __stdcall OnDirectoryDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			void OpenDir(Text::CStringNN dir);
			void OpenPackage(NN<IO::PackageFile> pkg);
			void ClearItems();
			static void __stdcall FreeItem(NN<LogItem> item);
		public:
			AVIRSeleniumLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRSeleniumLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
