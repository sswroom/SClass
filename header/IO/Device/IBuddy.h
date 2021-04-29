#ifndef _SM_IO_DEVICE_IBUDDY
#define _SM_IO_DEVICE_IBUDDY
#include "IO/Stream.h"

namespace IO
{
	namespace Device
	{
		class IBuddy
		{
		private:
			void *hand;
			UInt8 lastEffect;
			IO::Stream *stm;

			void InitDevice(const UTF8Char *devName);
		public:
			typedef enum
			{
				IBBE_OFF = 0,
				IBBE_TURN_LEFT = 1,
				IBBE_TURN_RIGHT = 2
			} IBuddyBodyEffect;

			typedef enum
			{
				IBHDE_OFF = 0,
				IBHDE_LIGHT_RED = 0x10,
				IBHDE_LIGHT_GREEN = 0x20,
				IBHDE_LIGHT_BLUE = 0x40,
				IBHDE_LIGHT_YELLOW = 0x30,
				IBHDE_LIGHT_MAGENTA = 0x50,
				IBHDE_LIGHT_CYAN = 0x60,
				IBHDE_LIGHT_WHITE = 0x70
			} IBuddyHeadEffect;

			typedef enum
			{
				IBHRE_OFF = 0,
				IBHRE_LIGHT = 0x80
			} IBuddyHeartEffect;

			typedef enum
			{
				IBWE_OFF = 0,
				IBWE_WING_UP = 0x8,
				IBWE_WING_DOWN = 0x4
			} IBuddyWingEffect;

			static UOSInt GetNumDevice();

			IBuddy(UOSInt devNo);
			~IBuddy();

			Bool IsError();
			void PlayEffect(IBuddyBodyEffect be, IBuddyHeadEffect hde, IBuddyHeartEffect hre, IBuddyWingEffect we);
		};
	}
}

#endif
