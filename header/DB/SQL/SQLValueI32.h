#ifndef _SM_DB_SQLVALUEI32
#define _SM_DB_SQLVALUEI32
#include "DB/SQL/SQLValue.h"

namespace DB
{
	namespace SQL
	{
		class SQLValueI32 : public SQLValue
		{
		private:
			Int32 value;
		public:
			SQLValueI32(Int32 value);
			virtual ~SQLValueI32();

			virtual ValueType GetValueType() const;
			Int32 GetValue() const;
		};
	}
}
#endif
