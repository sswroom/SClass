#ifndef _SM_SSWR_ORGANMGR_ORGANBOOKFORM
#define _SM_SSWR_ORGANMGR_ORGANBOOKFORM

#include "Data/SortableArrayListNN.hpp"
#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIButton.h"
#include "UI/GUIDateTimePicker.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class BookArrayList : public Data::SortableArrayListNN<OrganBook>
		{
		public:
			BookArrayList();
			virtual ~BookArrayList();

			void Sort();
			static OSInt __stdcall CompareBook(NN<OrganBook> book1, NN<OrganBook> book2);
			virtual OSInt Compare(NN<OrganBook> book1, NN<OrganBook> book2) const;
		};

		class OrganBookForm : public UI::GUIForm
		{
		private:
			NN<OrganEnv> env;

			NN<UI::GUIPanel> pnlBook;
			NN<UI::GUIPanel> pnlBookDet;
			NN<UI::GUIListView> lvBook;
			NN<UI::GUIHSplitter> hspBook;
			NN<UI::GUIButton> btnBookPaste;
			NN<UI::GUILabel> lblBookPublish;
			NN<UI::GUIDateTimePicker> dtpBookPublish;
			NN<UI::GUILabel> lblBookAuthor;
			NN<UI::GUITextBox> txtBookAuthor;
			NN<UI::GUILabel> lblBookTitle;
			NN<UI::GUITextBox> txtBookTitle;
			NN<UI::GUILabel> lblBookSource;
			NN<UI::GUITextBox> txtBookSource;
			NN<UI::GUILabel> lblBookURL;
			NN<UI::GUITextBox> txtBookURL;
			NN<UI::GUILabel> lblBookGroup;
			NN<UI::GUITextBox> txtBookGroup;
			NN<UI::GUIButton> btnBookAdd;
			NN<UI::GUITabControl> tcBook;
			NN<UI::GUITabPage> tpBookRef;
			NN<UI::GUITabPage> tpBookAuthor;

			BookArrayList bookList;
			Bool changed;

			static void __stdcall OnBookPublishChg(AnyType userObj, NN<Data::DateTime> newDate);
			static void __stdcall OnBookAddClicked(AnyType userObj);
			static void __stdcall OnBookPasteClicked(AnyType userObj);

			void UpdateBookList();
		public:
			OrganBookForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env);
			virtual ~OrganBookForm();

			virtual void OnMonitorChanged();

			Bool IsChanged();
		};
	}
}
#endif