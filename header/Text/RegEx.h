#ifndef _SM_TEXT_REGEX
#define _SM_TEXT_REGEX
#include "Data/ArrayListNative.hpp"
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
			AnyChar,
			Bracket,
			Or,
			CharMatch
		};

		class Expression
		{
		public:
			virtual ~Expression(){};

			virtual ExpressionType GetType() const = 0;
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const = 0;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const = 0;
		};

		class UnknownExpression : public Expression
		{
		public:
			UnknownExpression() {};
			virtual ~UnknownExpression() {};

			virtual ExpressionType GetType() const { return ExpressionType::Unknown; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const;
		};

		class WSExpression : public Expression
		{
		public:
			WSExpression() {};
			virtual ~WSExpression() {};

			virtual ExpressionType GetType() const { return ExpressionType::WS; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const;
		};

		class SeqExpression : public Expression
		{
		protected:
			Data::ArrayListNN<Expression> expList;
		public:
			SeqExpression(NN<Data::ArrayListNN<Expression>> expList) { this->expList.AddAll(expList); };
			virtual ~SeqExpression() { this->expList.DeleteAll();};

			virtual ExpressionType GetType() const { return ExpressionType::Seq; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const;

		private:
			UIntOS MatchInner(Text::CStringNN s, UIntOS sIndex, UIntOS expIndex) const;
		};

		class SubExpression : public SeqExpression
		{
		public:
			SubExpression(NN<Data::ArrayListNN<Expression>> expList) : SeqExpression(expList) { };
			virtual ~SubExpression() {};

			virtual ExpressionType GetType() const { return ExpressionType::Sub; };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const;
		};

		class ZeroOrOneExpression : public Expression
		{
		private:
			NN<Expression> innerExp;
		public:
			ZeroOrOneExpression(NN<Expression> exp) { this->innerExp = exp; }
			virtual ~ZeroOrOneExpression() { this->innerExp.Delete(); }

			virtual ExpressionType GetType() const { return ExpressionType::ZeroOrOne; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const { return this->innerExp->Match(s, index); };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { this->innerExp->ToString(sb); sb->AppendUTF8Char('?'); };
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const {sb->AppendChar('\t', level)->Append(CSTR("ZeroOrOne: ")); this->ToString(sb); }
		};

		class ZeroOrManyExpression : public Expression
		{
		private:
			NN<Expression> innerExp;
		public:
			ZeroOrManyExpression(NN<Expression> exp) { this->innerExp = exp; }
			virtual ~ZeroOrManyExpression() { this->innerExp.Delete(); }

			virtual ExpressionType GetType() const { return ExpressionType::ZeroOrMany; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const { return this->innerExp->Match(s, index); };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { this->innerExp->ToString(sb); sb->AppendUTF8Char('*'); };
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const {sb->AppendChar('\t', level)->Append(CSTR("ZeroOrMany: ")); this->ToString(sb); }
		};

		class OneOrManyExpression : public Expression
		{
		private:
			NN<Expression> innerExp;
		public:
			OneOrManyExpression(NN<Expression> exp) { this->innerExp = exp; }
			virtual ~OneOrManyExpression() { this->innerExp.Delete(); }

			virtual ExpressionType GetType() const { return ExpressionType::OneOrMany; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const { return this->innerExp->Match(s, index); };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { this->innerExp->ToString(sb); sb->AppendUTF8Char('+'); };
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const {sb->AppendChar('\t', level)->Append(CSTR("OneOrMany: ")); this->ToString(sb); }
		};

		class AnyCharExpression : public Expression
		{
		public:
			AnyCharExpression() {}
			virtual ~AnyCharExpression() {}

			virtual ExpressionType GetType() const { return ExpressionType::AnyChar; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const { if (s.leng >= index + 1) return 1; return INVALID_INDEX; };
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { sb->AppendUTF8Char('.'); };
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const {sb->AppendChar('\t', level)->Append(CSTR("AnyChar: .")); }
		};

		class BracketExpression : public Expression
		{
		private:
			Bool notMatch;
			Data::ArrayListNative<UTF32Char> charList;
		public:
			BracketExpression(Bool notMatch) { this->notMatch = notMatch; }
			virtual ~BracketExpression() {}

			virtual ExpressionType GetType() const { return ExpressionType::Bracket; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const;
			void AddChar(UTF32Char c) {this->charList.Add(c); }
		};

		class OrExpression : public Expression
		{
		private:
			Data::ArrayListNN<Expression> expList;
		public:
			OrExpression() {}
			virtual ~OrExpression() { this->expList.DeleteAll(); }

			virtual ExpressionType GetType() const { return ExpressionType::Or; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const;
			void AddExpList(NN<Data::ArrayListNN<Expression>> expList);
		};

		class CharMatchExpression : public Expression
		{
		private:
			UTF32Char c;
		public:
			CharMatchExpression(UTF32Char c) { this->c = c; }
			virtual ~CharMatchExpression() { }

			virtual ExpressionType GetType() const { return ExpressionType::CharMatch; };
			virtual UIntOS Match(Text::CStringNN s, UIntOS index) const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const { sb->AppendChar(c, 1); }
			virtual void Summary(NN<Text::StringBuilderUTF8> sb, UIntOS level) const {sb->AppendChar('\t', level)->Append(CSTR("CharMatch: "))->AppendChar(c, 1); }
		};

	private:
		NN<Expression> exp;

		struct ParseState;
		static NN<Expression> ParseExpression(NN<ParseState> state);
	public:
		RegEx(UnsafeArray<const Char> regEx);
		~RegEx();

		UIntOS Split(Text::CStringNN s, NN<Data::ArrayListStringNN> result) const;
		void Summary(NN<Text::StringBuilderUTF8> sb) const;
		void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif
