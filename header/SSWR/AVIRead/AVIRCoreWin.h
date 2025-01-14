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
			AVIRCoreWin(NN<UI::GUICore> ui);
			virtual ~AVIRCoreWin();

			virtual void OpenObject(NN<IO::ParsedObject> pobj);
			virtual void SaveData(NN<UI::GUIForm> ownerForm, NN<IO::ParsedObject> pobj, const WChar *dialogName);
			virtual Optional<Media::Printer> SelectPrinter(Optional<UI::GUIForm> frm);
		};
	}
}
#endif
