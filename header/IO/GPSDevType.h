#ifndef _SM_IO_GPSDEVTYPE
#define _SM_IO_GPSDEVTYPE

namespace IO
{
	enum class GPSDevType
	{
		Unknown,

		SP3000 = 257,
		SP1000 = 258,
		MTuL = 259,
		SP4600 = 260,
		SP2000 = 261,
		Tracker3G = 262,
		MiniTracker = 263,
		Navizot = 264,
		TK109 = 265,
		GPSDev = 266,
		Q10 = 267,
		FOX3 = 268,
		SpaceinetWeb = 269,
		GPSDevUDP = 270,
		JTT808 = 271,
		JMVL01 = 272
	};
}
#endif
