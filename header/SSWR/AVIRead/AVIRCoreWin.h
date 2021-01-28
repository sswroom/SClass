#ifndef _SM_SSWR_AVIREAD_AVIRCOREWIN
#define _SM_SSWR_AVIREAD_AVIRCOREWIN
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRCoreWin : public AVIRCore
		{
		public:
			AVIRCoreWin(UI::GUICore *ui);
			virtual ~AVIRCoreWin();

			virtual void OpenObject(IO::ParsedObject *pobj);
			virtual void SaveData(UI::GUIForm *ownerForm, IO::ParsedObject *pobj, const WChar *dialogName);
			virtual Media::Printer *SelectPrinter(UI::GUIForm *frm);
		};

	};
};
#endif
