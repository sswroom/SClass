#ifndef _SM_SSWR_SHPCONV_SHPCONVMAINFORM
#define _SM_SSWR_SHPCONV_SHPCONVMAINFORM
#include "DB/DBFFile.h"
#include "IO/IProgressHandler.h"
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
		class SHPConvMainForm : public UI::GUIForm, public IO::IProgressHandler, public SSWR::SHPConv::IMapEng
		{
		public:
			typedef struct
			{
				Int32 recId;
				const UTF8Char *str;
			} StrRecord;

			typedef struct
			{
				Int32 blockX;
				Int32 blockY;
				Data::ArrayList<StrRecord*> *records;
			} Block;

			static const Char *typeName[];
		private:
			UI::GUILabel *lblDirectory;
			UI::GUITextBox *txtDirectory;
			UI::GUIButton *btnDirectory;
			UI::GUILabel *lblSource;
			UI::GUITextBox *txtSource;
			UI::GUIButton *btnSBrowse;

			UI::GUILabel *lblFileLength;
			UI::GUITextBox *txtFileLength;
			UI::GUILabel *lblVersion;
			UI::GUITextBox *txtVersion;
			UI::GUILabel *lblShpType;
			UI::GUITextBox *txtShpType;
			UI::GUILabel *lblMMin;
			UI::GUITextBox *txtMMin;
			UI::GUILabel *lblMMax;
			UI::GUITextBox *txtMMax;
			UI::GUIListBox *lstRecords;

			UI::GUILabel *lblXMin;
			UI::GUITextBox *txtXMin;
			UI::GUILabel *lblXMax;
			UI::GUITextBox *txtXMax;
			UI::GUILabel *lblYMin;
			UI::GUITextBox *txtYMin;
			UI::GUILabel *lblYMax;
			UI::GUITextBox *txtYMax;
			UI::GUILabel *lblZMin;
			UI::GUITextBox *txtZMin;
			UI::GUILabel *lblZMax;
			UI::GUITextBox *txtZMax;
			UI::GUILabel *lblRecCnt;
			UI::GUITextBox *txtRecCnt;
			UI::GUILabel *lblBlkScale;
			UI::GUITextBox *txtBlkScale;

			UI::GUIListBox *lstLang;
			UI::GUITextBox *txtCodePage;

			UI::GUILabel *lblSeperator;
			UI::GUIComboBox *cboSeperator;
			UI::GUIButton *btnGroup;
			UI::GUIButton *btnFilter;
			UI::GUIButton *btnPreview;
			UI::GUIButton *btnConvert;
			UI::GUITextBox *txtLabel;
			UI::GUILabel *lblProgress;

			Media::DrawEngine *deng;
			Media::MonitorMgr *monMgr;
			Data::ArrayList<MapFilter *> *globalFilters;
			const UTF8Char *progressName;
			UInt64 totalVal;
			UOSInt currGroup;
			Text::HKSCSFix *hkscsConv;
			Bool isGrid80;

			static void __stdcall OnDirectoryClicked(void *userObj);
			static void __stdcall OnSBrowseClicked(void *userObj);
			static void __stdcall OnLangSelChg(void *userObj);
			static void __stdcall OnRecordsSelChg(void *userObj);
			static void __stdcall OnGroupClicked(void *userObj);
			static void __stdcall OnFilterClicked(void *userObj);
			static void __stdcall OnPreviewClicked(void *userObj);
			static void __stdcall OnConvertClicked(void *userObj);

			Int32 GroupConvert(Text::CString sourceFile, const UTF8Char *outFilePrefix, Data::ArrayList<const UTF8Char*> *dbCols, Int32 blkScale, Data::ArrayList<MapFilter*> *filters, IO::IProgressHandler *progress, UOSInt groupCol, Data::ArrayList<const UTF8Char*> *outNames, Data::ArrayList<UInt32> *dbCols2);
			Int32 ConvertShp(Text::CString sourceFile, const UTF8Char *outFilePrefix, Data::ArrayList<const UTF8Char*> *dbCols, Int32 blkScale, Data::ArrayList<MapFilter*> *filters, IO::IProgressHandler *progress, Data::ArrayList<UInt32> *dbCols2);
			Int32 LoadShape(Text::CString fileName, Bool updateTxt);
			void ClearFilter();

			virtual void ParseLabelStr(const UTF8Char *labelStr, Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2);
			virtual void FreeLabelStr(Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2);
			virtual const UTF8Char *GetDBFName(DB::DBFFile *dbf, Data::ArrayList<const UTF8Char*> *dbCols, UOSInt currRec, Data::ArrayList<UInt32> *dbcols2);

		public:
			SHPConvMainForm(UI::GUIClientControl *parent, UI::GUICore *ui, Media::DrawEngine *deng, Media::MonitorMgr *monMgr);
			virtual ~SHPConvMainForm();

			virtual void OnMonitorChanged();

			virtual void ProgressStart(const UTF8Char *name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
