#ifndef _SM_SSWR_SHPCONV_SHPCONVMAINFORM
#define _SM_SSWR_SHPCONV_SHPCONVMAINFORM
#include "DB/DBFFile.h"
#include "IO/ProgressHandler.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/IMapEng.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "Text/HKSCSFix.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvMainForm : public UI::GUIForm, public IO::ProgressHandler, public SSWR::SHPConv::IMapEng
		{
		public:
			typedef struct
			{
				Int32 recId;
				Text::String *str;
			} StrRecord;

			typedef struct
			{
				Int32 blockX;
				Int32 blockY;
				Data::ArrayList<StrRecord*> *records;
			} Block;

			static Text::CStringNN typeName[];
		private:
			NotNullPtr<UI::GUILabel> lblDirectory;
			UI::GUITextBox *txtDirectory;
			NotNullPtr<UI::GUIButton> btnDirectory;
			NotNullPtr<UI::GUILabel> lblSource;
			UI::GUITextBox *txtSource;
			NotNullPtr<UI::GUIButton> btnSBrowse;

			NotNullPtr<UI::GUILabel> lblFileLength;
			UI::GUITextBox *txtFileLength;
			NotNullPtr<UI::GUILabel> lblVersion;
			UI::GUITextBox *txtVersion;
			NotNullPtr<UI::GUILabel> lblShpType;
			UI::GUITextBox *txtShpType;
			NotNullPtr<UI::GUILabel> lblMMin;
			UI::GUITextBox *txtMMin;
			NotNullPtr<UI::GUILabel> lblMMax;
			UI::GUITextBox *txtMMax;
			UI::GUIListBox *lstRecords;

			NotNullPtr<UI::GUILabel> lblXMin;
			UI::GUITextBox *txtXMin;
			NotNullPtr<UI::GUILabel> lblXMax;
			UI::GUITextBox *txtXMax;
			NotNullPtr<UI::GUILabel> lblYMin;
			UI::GUITextBox *txtYMin;
			NotNullPtr<UI::GUILabel> lblYMax;
			UI::GUITextBox *txtYMax;
			NotNullPtr<UI::GUILabel> lblZMin;
			UI::GUITextBox *txtZMin;
			NotNullPtr<UI::GUILabel> lblZMax;
			UI::GUITextBox *txtZMax;
			NotNullPtr<UI::GUILabel> lblRecCnt;
			UI::GUITextBox *txtRecCnt;
			NotNullPtr<UI::GUILabel> lblBlkScale;
			UI::GUITextBox *txtBlkScale;

			UI::GUIListBox *lstLang;
			UI::GUITextBox *txtCodePage;

			NotNullPtr<UI::GUILabel> lblSeperator;
			NotNullPtr<UI::GUIComboBox> cboSeperator;
			NotNullPtr<UI::GUIButton> btnGroup;
			NotNullPtr<UI::GUIButton> btnFilter;
			NotNullPtr<UI::GUIButton> btnPreview;
			NotNullPtr<UI::GUIButton> btnConvert;
			UI::GUITextBox *txtLabel;
			NotNullPtr<UI::GUILabel> lblLabelLegend;
			NotNullPtr<UI::GUILabel> lblProgress;

			NotNullPtr<Media::DrawEngine> deng;
			Media::MonitorMgr *monMgr;
			Data::ArrayList<MapFilter *> globalFilters;
			Text::CString progressName;
			UInt64 totalVal;
			UOSInt currGroup;
			Text::HKSCSFix hkscsConv;
			Bool isGrid80;

			static void __stdcall OnDirectoryClicked(void *userObj);
			static void __stdcall OnSBrowseClicked(void *userObj);
			static void __stdcall OnLangSelChg(void *userObj);
			static void __stdcall OnRecordsSelChg(void *userObj);
			static void __stdcall OnGroupClicked(void *userObj);
			static void __stdcall OnFilterClicked(void *userObj);
			static void __stdcall OnPreviewClicked(void *userObj);
			static void __stdcall OnConvertClicked(void *userObj);
			static void __stdcall OnFile(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);

			Int32 GroupConvert(Text::CStringNN sourceFile, Text::CString outFilePrefix, Data::ArrayList<const UTF8Char*> *dbCols, Int32 blkScale, NotNullPtr<Data::ArrayList<MapFilter*>> filters, IO::ProgressHandler *progress, UOSInt groupCol, Data::ArrayList<const UTF8Char*> *outNames, NotNullPtr<Data::ArrayList<UInt32>> dbCols2);
			Int32 ConvertShp(Text::CStringNN sourceFile, Text::CString outFilePrefix, Data::ArrayList<const UTF8Char*> *dbCols, Int32 blkScale, NotNullPtr<Data::ArrayList<MapFilter*>> filters, IO::ProgressHandler *progress, NotNullPtr<Data::ArrayList<UInt32>> dbCols2);
			Int32 LoadShape(Text::CStringNN fileName, Bool updateTxt);
			void ClearFilter();

			virtual void ParseLabelStr(Text::CString labelStr, Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2);
			virtual void FreeLabelStr(Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2);
			virtual NotNullPtr<Text::String> GetNewDBFName(DB::DBFFile *dbf, Data::ArrayList<const UTF8Char*> *dbCols, UOSInt currRec, NotNullPtr<Data::ArrayList<UInt32>> dbcols2);

		public:
			SHPConvMainForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<Media::DrawEngine> deng, Media::MonitorMgr *monMgr);
			virtual ~SHPConvMainForm();

			virtual void OnMonitorChanged();

			virtual void ProgressStart(Text::CString name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
