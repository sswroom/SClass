#ifndef _SM_SSWR_AVIREAD_AVIRFILESIZEPACKFORM
#define _SM_SSWR_AVIREAD_AVIRFILESIZEPACKFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileSizePackForm : public UI::GUIForm
		{
		private:
			class MyFile : public Data::IComparable
			{
			private:
				NN<Text::String> fileName;
				UInt64 fileSize;
			public:
				MyFile(Text::CString fileName, UInt64 fileSize);
				virtual ~MyFile();

				Bool ToString(NN<Text::StringBuilderUTF8> sb) const;
				NN<Text::String> GetName() const;
				UInt64 GetSize() const;
				virtual OSInt CompareTo(Data::IComparable *obj) const;
			};

		private:
			Data::ArrayListNN<MyFile> fileList;
			Data::ArrayListNN<MyFile> packList;
			const UTF8Char *filePath;
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlFile;
			NN<UI::GUILabel> lblDir;
			NN<UI::GUITextBox> txtDir;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblMaxSize;
			NN<UI::GUIComboBox> cboMaxSize;
			NN<UI::GUILabel> lblTotalSize;
			NN<UI::GUITextBox> txtTotalSize;
			NN<UI::GUILabel> lblDirName;
			NN<UI::GUITextBox> txtDirName;
			NN<UI::GUIButton> btnMove;
			NN<UI::GUIListBox> lbFilePack;
			NN<UI::GUIHSplitter> hspFile;
			NN<UI::GUIListBox> lbFileDir;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnMoveClicked(AnyType userObj);

			void ReleaseObjects();
			void GenList();
			UInt64 NewCalc(NN<Data::ArrayListNN<MyFile>> fileList, NN<Data::ArrayListNN<MyFile>> packList, UInt64 maxSize, UInt64 minSize);
		public:
			AVIRFileSizePackForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileSizePackForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
