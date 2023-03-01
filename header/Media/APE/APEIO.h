#ifndef _SM_MEDIA_APE_APEIO
#define _SM_MEDIA_APE_APEIO
#include "IO/StreamData.h"
#include <windows.h>
#include "Media/APE/IO.h"

namespace Media
{
	namespace APE
	{
		class APEIO : public CIO
		{
		private:
			IO::StreamData *data;
			Int64 currPos;

		public:
			APEIO(IO::StreamData *data);
			virtual ~APEIO();

			virtual int Open(const WChar * pName, BOOL bOpenReadOnly = FALSE);
			virtual int Close();
		    
			virtual int Read(void * pBuffer, UInt32 nBytesToRead, UInt32 * pBytesRead);
			virtual int Write(const void * pBuffer, UInt32 nBytesToWrite, UInt32 * pBytesWritten);
		    
			virtual int Seek(Int32 nDistance, UInt32 nMoveMode);
		    
			virtual int Create(const WChar * pName);
			virtual int Delete();

			virtual int SetEOF();

			virtual int GetPosition();
			virtual int GetSize();
			virtual int GetName(WChar * pBuffer);

			IO::StreamData *GetData();
		};
	};
};
#endif
