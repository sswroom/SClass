#ifndef _SM_SSWR_AVIREAD_AVIRCOVID19FORM
#define _SM_SSWR_AVIREAD_AVIRCOVID19FORM
#include "Data/Integer64Map.h"
#include "Data/StringUTF8Map.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCOVID19Form : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Int64 timeTicks;
				Int64 totalCases;
				Int64 totalDeaths;
			} DailyRecord;

			typedef struct
			{
				const UTF8Char *isoCode;
				const UTF8Char *name;
				Double population;
				Data::Integer64Map<DailyRecord*> *records;
			} CountryInfo;
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Data::StringUTF8Map<CountryInfo*> *countries;

			UI::GUIPanel *pnlRequest;
			UI::GUIButton *btnFile;
			UI::GUIButton *btnDownload;
			UI::GUIPictureBoxSimple *pbNewCases;
			UI::GUIVSplitter *vspNewCases;
			UI::GUIListView *lvCountry;

			static void __stdcall OnFileClicked(void *userObj);
			static void __stdcall OnDownloadClicked(void *userObj);
			static void __stdcall OnCountrySelChg(void *userObj);
			static void __stdcall OnNewCasesSizeChanged(void *userObj);

			void ClearRecords();
			Bool LoadCSV(IO::SeekableStream *stm);
		public:
			AVIRCOVID19Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRCOVID19Form();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
