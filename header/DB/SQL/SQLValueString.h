#ifndef _SM_DB_SQLVALUESTRING
#define _SM_DB_SQLVALUESTRING
#include "DB/SQL/SQLValue.h"
#include "Text/String.h"

namespace DB
{
	namespace SQL
	{
		class SQLValueString : public SQLValue
		{
		private:
			NN<Text::String> value;
		public:
			SQLValueString(Text::CStringNN value);
			virtual ~SQLValueString();

			virtual ValueType GetValueType() const;
			NN<Text::String> GetValue() const;
		};
	}
}
#endif
