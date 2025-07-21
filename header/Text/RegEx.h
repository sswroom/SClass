#ifndef _SM_TEXT_REGEX
#define _SM_TEXT_REGEX
#include "Data/ArrayListStringNN.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class RegEx
	{
	public:
		enum class ExpressionType
		{
			Unknown,
			WS,
			Seq,
			Sub,
			ZeroOrOne,
			OneOrMany,
			ZeroOrMany,
			AnyChar
		};

		class Expression
		{
		public:
			virtual ~Expression(){};

			virtual ExpressionType GetType() const = 0;
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const = 0;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
		};

		class UnknownExpression : public Expression
		{
		public:
			UnknownExpression() {};
			virtual ~UnknownExpression() {};

			virtual ExpressionType GetType() const { return ExpressionType::Unknown; };
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};

		class WSExpression : public Expression
		{
		public:
			WSExpression() {};
			virtual ~WSExpression() {};

			virtual ExpressionType GetType() const { return ExpressionType::WS; };
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};

		class SeqExpression : public Expression
		{
		private:
			Data::ArrayListNN<Expression> expList;
		public:
			SeqExpression(NN<Data::ArrayListNN<Expression>> expList) { this->expList.AddAll(expList); };
			virtual ~SeqExpression() { this->expList.DeleteAll();};

			virtual ExpressionType GetType() const { return ExpressionType::Seq; };
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		private:
			UOSInt MatchInner(Text::CStringNN s, UOSInt sIndex, UOSInt expIndex) const;
		};

		class SubExpression : public SeqExpression
		{
		public:
			SubExpression(NN<Data::ArrayListNN<Expression>> expList) : SeqExpression(expList) { };
			virtual ~SubExpression() {};

			virtual ExpressionType GetType() const { return ExpressionType::Sub; };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};

		class ZeroOrOneExpression : public Expression
		{
		private:
			NN<Expression> innerExp;
		public:
			ZeroOrOneExpression(NN<Expression> exp) { this->innerExp = exp; }
			virtual ~ZeroOrOneExpression() { this->innerExp.Delete(); }

			virtual ExpressionType GetType() const { return ExpressionType::ZeroOrOne; };
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const { return this->innerExp->Match(s, index); };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { this->innerExp->ToString(sb); sb->AppendUTF8Char('?'); };
		};

		class ZeroOrManyExpression : public Expression
		{
		private:
			NN<Expression> innerExp;
		public:
			ZeroOrManyExpression(NN<Expression> exp) { this->innerExp = exp; }
			virtual ~ZeroOrManyExpression() { this->innerExp.Delete(); }

			virtual ExpressionType GetType() const { return ExpressionType::ZeroOrMany; };
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const { return this->innerExp->Match(s, index); };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { this->innerExp->ToString(sb); sb->AppendUTF8Char('*'); };
		};

		class OneOrManyExpression : public Expression
		{
		private:
			NN<Expression> innerExp;
		public:
			OneOrManyExpression(NN<Expression> exp) { this->innerExp = exp; }
			virtual ~OneOrManyExpression() { this->innerExp.Delete(); }

			virtual ExpressionType GetType() const { return ExpressionType::OneOrMany; };
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const { return this->innerExp->Match(s, index); };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { this->innerExp->ToString(sb); sb->AppendUTF8Char('+'); };
		};

		class AnyCharExpression : public Expression
		{
		public:
			AnyCharExpression() {}
			virtual ~AnyCharExpression() {}

			virtual ExpressionType GetType() const { return ExpressionType::AnyChar; };
			virtual UOSInt Match(Text::CStringNN s, UOSInt index) const { if (s.leng >= index + 1) return 1; return INVALID_INDEX; };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { sb->AppendUTF8Char('.'); };
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
