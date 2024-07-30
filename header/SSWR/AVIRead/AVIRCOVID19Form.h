#ifndef _SM_SSWR_AVIREAD_AVIRCOVID19FORM
#define _SM_SSWR_AVIREAD_AVIRCOVID19FORM
#include "Data/FastMapNN.h"
#include "Data/StringMapNN.h"
#include "Net/SSLEngine.h"
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
				NN<Text::String> isoCode;
				NN<Text::String> name;
				Double population;
				Data::FastMapNN<Int64, DailyRecord> records;
			} CountryInfo;
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Data::StringMapNN<CountryInfo> countries;

			NN<UI::GUIPanel> pnlRequest;
			NN<UI::GUIButton> btnFile;
			NN<UI::GUIButton> btnDownload;
			NN<UI::GUIPictureBoxSimple> pbNewCases;
			NN<UI::GUIVSplitter> vspNewCases;
			NN<UI::GUIListView> lvCountry;

			static void __stdcall OnFileClicked(AnyType userObj);
			static void __stdcall OnDownloadClicked(AnyType userObj);
			static void __stdcall OnCountrySelChg(AnyType userObj);
			static void __stdcall OnNewCasesSizeChanged(AnyType userObj);

			void ClearRecords();
			Bool LoadCSV(NN<IO::SeekableStream> stm);
		public:
			AVIRCOVID19Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRCOVID19Form();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
