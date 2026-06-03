#ifndef _SM_DB_SQLENGINEREADER
#define _SM_DB_SQLENGINEREADER
#include "DB/DBReader.h"

namespace DB
{
	namespace SQL
	{
		class SQLEngineReader : public DBReader
		{
		public:
			virtual ~SQLEngineReader() {}
		};
	}
}
#endif
