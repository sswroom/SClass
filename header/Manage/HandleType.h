#ifndef _SM_MANAGE_HANDLETYPE
#define _SM_MANAGE_HANDLETYPE
#include "Text/CString.h"

namespace Manage
{
	enum class HandleType
	{
		Unknown,
		File,
		Directory,
		Device,
		EPoll,
		Event,
		INotify,
		Signal,
		Timer,
		Socket,
		Pipe,
		Memory,
		Key,
		WaitCompletionPacket,
		IoCompletion,
		Mutant,
		TpWorkerFactory,
		Section,
		IRTimer,
		ALPC_Port,
		Semaphore,
		Thread,
		IoCompletionReserve,
		WindowStation,
		Desktop,
		Token,
		Process,
		DxgkCompositionObject,
		EtwRegistration
	};

	Text::CStringNN HandleTypeGetName(HandleType handleType);
}
#endif
