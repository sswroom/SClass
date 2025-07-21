#ifndef _SM_TEXT_REGEX
#define _SM_TEXT_REGEX
#include "Data/ArrayListStringNN.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class RegEx
	{
	public:
		class Expression
		{
		public:
			virtual ~Expression(){};

			virtual UOSInt Match(Text::CStringNN s, UOSInt index) = 0;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
		};

		class UnknownExpression : public Expression
		{
		public:
			UnknownExpression() {};
			virtual ~UnknownExpression() {};

			virtual UOSInt Match(Text::CStringNN s, UOSInt index);
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};

		class WSExpression : public Expression
		{
		public:
			WSExpression() {};
			virtual ~WSExpression() {};

			virtual UOSInt Match(Text::CStringNN s, UOSInt index);
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};

	private:
		NN<Expression> exp;

		struct ParseState;
		static NN<Expression> ParseExpression(NN<ParseState> state);
	public:
		RegEx(UnsafeArray<const Char> regEx);
		~RegEx();

		UOSInt Split(Text::CStringNN s, NN<Data::ArrayListStringNN> result) const;
	};
}
#endif
