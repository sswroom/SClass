#ifndef _SM_SSWR_AVIREAD_AVIRMD5COMPAREFORM
#define _SM_SSWR_AVIREAD_AVIRMD5COMPAREFORM
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMD5CompareForm : public UI::GUIForm
		{
		private:
			struct MD5Entry
			{
				NN<Text::String> fileName;
				NN<Text::String> md5;
			};

			class MD5Comparator;

		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Bool nextIs2;

			NN<UI::GUIButton> btnFile1;
			NN<UI::GUITextBox> txtFile1;
			NN<UI::GUIButton> btnFile2;
			NN<UI::GUITextBox> txtFile2;
			NN<UI::GUILabel> lblMoreCount;
			NN<UI::GUITextBox> txtMoreCount;
			NN<UI::GUIButton> btnMore;
			NN<UI::GUILabel> lblLessCount;
			NN<UI::GUITextBox> txtLessCount;
			NN<UI::GUIButton> btnLess;
			NN<UI::GUILabel> lblMismatchCount;
			NN<UI::GUITextBox> txtMismatchCount;
			NN<UI::GUIButton> btnMismatch;;

			static void __stdcall FreeMD5Entry(NN<MD5Entry> entry);
			static void LoadMD5(Text::CStringNN fileName, NN<Data::ArrayListNN<MD5Entry>> md5List);
			static Optional<MD5Entry> MD5FindEntry(NN<Data::ArrayListNN<MD5Entry>> md5List, Text::CStringNN fileName);
			static void __stdcall OnFile1Clicked(AnyType userObj);
			static void __stdcall OnFile2Clicked(AnyType userObj);
			static void __stdcall OnMoreClicked(AnyType userObj);
			static void __stdcall OnLessClicked(AnyType userObj);
			static void __stdcall OnMismatchClicked(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			void UpdateStatus();
		public:
			AVIRMD5CompareForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMD5CompareForm();
		
			virtual void OnMonitorChanged();
		};
	}
}
#endif
