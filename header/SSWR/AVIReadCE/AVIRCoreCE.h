#ifndef _SM_SSWR_AVIREADCE_AVIRCORECE
#define _SM_SSWR_AVIREADCE_AVIRCORECE
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIReadCE
	{
		class AVIRCoreCE : public SSWR::AVIRead::AVIRCore
		{
		public:
			AVIRCoreCE(UI::GUICore *ui);
			virtual ~AVIRCoreCE();

			virtual void BeginLoad();
			virtual void EndLoad();
			virtual void OpenObject(IO::ParsedObject *pobj);
			virtual void SaveData(UI::GUIForm *ownerForm, IO::ParsedObject *pobj, const WChar *dialogName);
			virtual void OpenGSMModem(IO::Stream *modemPort);
			virtual Media::Printer *SelectPrinter(UI::GUIForm *frm);
			virtual IO::Stream *OpenStream(StreamType *st, UI::GUIForm *ownerFrm, Int32 defBaudRate, Bool allowReadOnly);
		};
	}
}
#endif
