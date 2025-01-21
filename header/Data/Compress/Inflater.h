#ifndef _SM_DATA_COMPRESS_INFLATER
#define _SM_DATA_COMPRESS_INFLATER
#include "IO/Stream.h"

namespace Data
{
	namespace Compress
	{
		enum class InflateStatus
		{
			FailedCannotMakeProgress = -4,
			BadParam = -3,
			Adler32Mismatch = -2,
			Failed = -1,
			Done = 0,
			NeedsMoreInput = 1,
			HasMoreOutput = 2
		};

		enum class InflateResult
		{
			Ok = 0,
			StreamEnd = 1,
			NeedDict = 2,
			Errno = -1,
			StreamError = -2,
			DataError = -3,
			MemError = -4,
			BufError = -5,
			VersionError = -6,
			ParamError = -10000
		};

		struct InflateState;

		class Inflater
		{
		private:
			NN<IO::Stream> decStm;
			const UInt8 *next_in; /* pointer to next byte to read */
			UInt32 avail_in;        /* number of bytes available at next_in */
			UInt32 total_in;            /* total number of bytes consumed so far */

			UInt8 *next_out; /* pointer to next byte to write */
			UInt32 avail_out;  /* number of bytes that can be written to next_out */
			UInt32 total_out;      /* total number of bytes produced so far */

			Char *msg;                       /* error msg (unused) */
			NN<InflateState> state; /* internal state, allocated by zalloc/zfree */

			Int32 data_type;     /* data_type (unused) */
			UInt32 adler;    /* adler32 of the source or uncompressed data */
			UInt32 reserved; /* not used */

			InflateResult Inflate(Bool isEnd);
		public:
			Inflater(NN<IO::Stream> decStm, Bool hasHeader);
			virtual ~Inflater();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(Data::ByteArrayR buff);
		};
	}
}
#endif
