#ifndef _SM_MEDIA_G711MULAWRAWSOURCE
#define _SM_MEDIA_G711MULAWRAWSOURCE
#include "Media/LPCMSource.h"

namespace Media
{
	class G711muLawRAWSource : public LPCMSource
	{
	public:
		G711muLawRAWSource(IO::StreamData *fd, Int64 ofst, Int64 length, const WChar *name);
		virtual ~G711muLawRAWSource();
	};
};
#endif
