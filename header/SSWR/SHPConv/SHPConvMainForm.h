#ifndef _SM_SSWR_SHPCONV_SHPCONVMAINFORM
#define _SM_SSWR_SHPCONV_SHPCONVMAINFORM
#include "Data/ArrayListObj.hpp"
#include "DB/DBFFile.h"
#include "IO/ProgressHandler.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/SHPConvEng.h"
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
		class SHPConvMainForm : public UI::GUIForm, public IO::ProgressHandler, public SSWR::SHPConv::SHPConvEng
		{
		public:
			typedef struct
			{
				Int32 recId;
				Optional<Text::String> str;
			} StrRecord;

			typedef struct
			{
				Int32 blockX;
				Int32 blockY;
				Data::ArrayListObj<StrRecord*> *records;
			} Block;

			static Text::CStringNN typeName[];
		private:
			NN<UI::GUILabel> lblDirectory;
			NN<UI::GUITextBox> txtDirectory;
			NN<UI::GUIButton> btnDirectory;
			NN<UI::GUILabel> lblSource;
			NN<UI::GUITextBox> txtSource;
			NN<UI::GUIButton> btnSBrowse;

			NN<UI::GUILabel> lblFileLength;
			NN<UI::GUITextBox> txtFileLength;
			NN<UI::GUILabel> lblVersion;
			NN<UI::GUITextBox> txtVersion;
			NN<UI::GUILabel> lblShpType;
			NN<UI::GUITextBox> txtShpType;
			NN<UI::GUILabel> lblMMin;
			NN<UI::GUITextBox> txtMMin;
			NN<UI::GUILabel> lblMMax;
			NN<UI::GUITextBox> txtMMax;
			NN<UI::GUIListBox> lstRecords;

			NN<UI::GUILabel> lblXMin;
			NN<UI::GUITextBox> txtXMin;
			NN<UI::GUILabel> lblXMax;
			NN<UI::GUITextBox> txtXMax;
			NN<UI::GUILabel> lblYMin;
			NN<UI::GUITextBox> txtYMin;
			NN<UI::GUILabel> lblYMax;
			NN<UI::GUITextBox> txtYMax;
			NN<UI::GUILabel> lblZMin;
			NN<UI::GUITextBox> txtZMin;
			NN<UI::GUILabel> lblZMax;
			NN<UI::GUITextBox> txtZMax;
			NN<UI::GUILabel> lblRecCnt;
			NN<UI::GUITextBox> txtRecCnt;
			NN<UI::GUILabel> lblBlkScale;
			NN<UI::GUITextBox> txtBlkScale;

			NN<UI::GUIListBox> lstLang;
			NN<UI::GUITextBox> txtCodePage;

			NN<UI::GUILabel> lblSeperator;
			NN<UI::GUIComboBox> cboSeperator;
			NN<UI::GUIButton> btnGroup;
			NN<UI::GUIButton> btnFilter;
			NN<UI::GUIButton> btnPreview;
			NN<UI::GUIButton> btnConvert;
			NN<UI::GUITextBox> txtLabel;
			NN<UI::GUILabel> lblLabelLegend;
			NN<UI::GUILabel> lblProgress;

			NN<Media::DrawEngine> deng;
			NN<Media::MonitorMgr> monMgr;
			Data::ArrayListNN<MapFilter> globalFilters;
			Text::CString progressName;
			UInt64 totalVal;
			UOSInt currGroup;
			Text::HKSCSFix hkscsConv;
			Bool isGrid80;

			static void __stdcall OnDirectoryClicked(AnyType userObj);
			static void __stdcall OnSBrowseClicked(AnyType userObj);
			static void __stdcall OnLangSelChg(AnyType userObj);
			static void __stdcall OnRecordsSelChg(AnyType userObj);
			static void __stdcall OnGroupClicked(AnyType userObj);
			static void __stdcall OnFilterClicked(AnyType userObj);
			static void __stdcall OnPreviewClicked(AnyType userObj);
			static void __stdcall OnConvertClicked(AnyType userObj);
			static void __stdcall OnFile(AnyType userObj, Data::DataArray<NN<Text::String>> files);

			Int32 GroupConvert(Text::CStringNN sourceFile, Text::CString outFilePrefix, Data::ArrayListObj<const UTF8Char*> *dbCols, Int32 blkScale, NN<Data::ArrayListNN<MapFilter>> filters, IO::ProgressHandler *progress, UOSInt groupCol, Data::ArrayListObj<const UTF8Char*> *outNames, NN<Data::ArrayListNative<UInt32>> dbCols2);
			Int32 ConvertShp(Text::CStringNN sourceFile, Text::CString outFilePrefix, Data::ArrayListObj<const UTF8Char*> *dbCols, Int32 blkScale, NN<Data::ArrayListNN<MapFilter>> filters, IO::ProgressHandler *progress, NN<Data::ArrayListNative<UInt32>> dbCols2);
			Int32 LoadShape(Text::CStringNN fileName, Bool updateTxt);
			void ClearFilter();

			virtual void ParseLabelStr(Text::CStringNN labelStr, Data::ArrayListObj<const UTF8Char*> *dbCols, Data::ArrayListNative<UInt32> *dbCols2);
			virtual void FreeLabelStr(Data::ArrayListObj<const UTF8Char*> *dbCols, Data::ArrayListNative<UInt32> *dbCols2);
			virtual NN<Text::String> GetNewDBFName(DB::DBFFile *dbf, Data::ArrayListObj<const UTF8Char*> *dbCols, UOSInt currRec, NN<Data::ArrayListNative<UInt32>> dbcols2);

		public:
			SHPConvMainForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::DrawEngine> deng, NN<Media::MonitorMgr> monMgr);
			virtual ~SHPConvMainForm();

			virtual void OnMonitorChanged();

			virtual void ProgressStart(Text::CStringNN name, UInt64 count);
			virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount);
			virtual void ProgressEnd();
		};
	}
}
#endif
