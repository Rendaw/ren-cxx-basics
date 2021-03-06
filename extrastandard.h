#ifndef extrastandard_h
#define extrastandard_h

#include <memory>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

#include "function.h"

template <typename TargetT> struct ExplicitT {};

struct FinallyT
{
	function<void(void)> Callback;

	FinallyT(void) {}
	FinallyT(FinallyT const &) = delete;
	inline FinallyT(FinallyT &&Other) : Callback(std::move(Other.Callback))
		{ Other.Callback = {}; }
	inline FinallyT(function<void(void)> &&Callback) : Callback(Callback) {}
	inline FinallyT &operator =(FinallyT const &) = delete;
	inline FinallyT &operator =(FinallyT &&Other)
	{ 
		Callback = std::move(Other.Callback); 
		Other.Callback = {}; 
		return *this;
	}
	inline FinallyT &operator =(function<void(void)> &&Callback) 
	{
		this->Callback = std::move(Callback);
		return *this;
	}
	~FinallyT(void) { if (Callback) Callback(); }
};

//----------------------------------------------------------------------------------------------------------------
// Will be included in C++14 lolololol
template<typename T, typename... ArgsT> std::unique_ptr<T> make_unique(ArgsT &&... Args)
	{ return std::unique_ptr<T>(new T(std::forward<ArgsT>(Args)...)); }

// Default stuff for shared pointers and unique pointers
template <typename ValueT> void make_shared(std::shared_ptr<ValueT> &Target) { Target = std::make_shared<ValueT>(); }
template <typename ValueT> void make_shared(std::shared_ptr<ValueT> &Target, ValueT Value) { Target = std::make_shared<ValueT>(Value); }
template <typename ValueT, typename ...ArgumentsT> void make_shared(std::shared_ptr<ValueT> &Target, ArgumentsT... Arguments) { Target = make_shared<ValueT>(std::forward<ArgumentsT>(Arguments)...); }
template <typename ValueT> void make_unique(std::unique_ptr<ValueT> &Target) { Target.reset(new ValueT{}); }
template <typename ValueT> void make_unique(std::unique_ptr<ValueT> &Target, ValueT Value) { Target.reset(std::unique_ptr<ValueT>{std::move(Value)}); }
template <typename ValueT, typename ...ArgumentsT> void make_unique(std::unique_ptr<ValueT> &Target, ArgumentsT... Arguments) { Target = make_unique<ValueT>(std::forward<ArgumentsT>(Arguments)...); }

//----------------------------------------------------------------------------------------------------------------
// To identify tuples and tuple-likes in template args
// Specialize to extend to derived types
template <typename ValueT> struct IsTuply
	{ static constexpr bool Result = false; };
template <typename ...InnerT> struct IsTuply<std::tuple<InnerT...>> 
	{ static constexpr bool Result = true; };

// Streaming tuples
template <typename NextT, typename ...InnerT>
	void StreamTupleElement(std::ostream &Stream, std::tuple<InnerT...> const &Value)
{
	Stream << std::get<sizeof...(InnerT) - 1>(Value);
}

template <typename NextT, typename RemainingFirstT, typename ...RemainingT, typename ...InnerT>
	void StreamTupleElement(std::ostream &Stream, std::tuple<InnerT...> const &Value)
{
	Stream << std::get<sizeof...(InnerT) - sizeof...(RemainingT) - 2>(Value) << ", ";
	StreamTupleElement<RemainingFirstT, RemainingT...>(Stream, Value);
}

template <typename InnerFirstT, typename ...InnerRemainingT>
	std::ostream &operator <<(std::ostream &Stream, std::tuple<InnerFirstT, InnerRemainingT...> const &Value)
{
	Stream << "tuple(";
	StreamTupleElement<InnerFirstT, InnerRemainingT...>(Stream, Value);
	Stream << ")";
	return Stream;
}

inline std::ostream &operator <<(std::ostream &Stream, std::tuple<> const &Value)
	{ return Stream << "tuple()"; }

//----------------------------------------------------------------------------------------------------------------
// Remove values for vector
template <typename VectorT, typename FunctionT> void VectorRemove(VectorT &Vector, FunctionT const &Filter)
	{ Vector.erase(std::remove_if(Vector.begin(), Vector.end(), Filter), Vector.end()); }

//----------------------------------------------------------------------------------------------------------------
// Won't be included in C++14 lolololol
template <typename type> struct fixed_vector : std::vector<type>
{
	// Notes
	// std::vector defines a copy constructor even if type is not copy constructible... this fixes that
	// std::vector only uses move constructor for resizes and other operations if type has a noexcept destructor... and std::unique_ptr does not.  This also hacks around that.

	using std::vector<type>::vector;

	~fixed_vector(void) {}

	fixed_vector(void) : std::vector<type>() {}

	fixed_vector(fixed_vector &&other) noexcept : std::vector<type>(std::move(other)) {} // Hack... like everything else in this file

	template 
	<
		typename duplicate = type, 
		typename std::enable_if<std::is_copy_constructible<duplicate>::value>::type * = nullptr
	> 
	fixed_vector(fixed_vector const &) = delete;

	template 
	<
		typename duplicate = type, 
		typename std::enable_if<!std::is_copy_constructible<duplicate>::value>::type * = nullptr
	> 
	fixed_vector(fixed_vector const &other) : std::vector<type>(other) {}
	
	fixed_vector<type> &operator =(fixed_vector &&other) { std::vector<type>::operator =(std::move(other)); return *this; }
	
	template 
	<
		typename duplicate = type, 
		typename std::enable_if<std::is_copy_constructible<duplicate>::value>::type * = nullptr
	> 
	fixed_vector<type> &operator =(fixed_vector const &) = delete;

	template 
	<
		typename duplicate = type, 
		typename std::enable_if<!std::is_copy_constructible<duplicate>::value>::type * = nullptr
	> 
	fixed_vector<type> &operator =(fixed_vector const &other) { std::vector<type>::operator =(other); return *this; }
};

//----------------------------------------------------------------------------------------------------------------
// r-value string serialization and deserialization
struct StringT
{
	private:
		std::stringstream Buffer;
	public:

	StringT(void) {}
	StringT(std::string const &Initial) : Buffer(Initial) {}
	template <typename Whatever> StringT &operator <<(Whatever const &Input) { Buffer << Input; return *this; }
	template <typename Whatever> StringT &operator >>(Whatever &Output) { Buffer >> Output; return *this; }
	bool operator !(void) const { return !Buffer; }
	decltype(Buffer.str()) str(void) const { return Buffer.str(); }
	operator std::string(void) const { return Buffer.str(); }
};

inline std::ostream &operator <<(std::ostream &Stream, StringT const &Value)
	{ return Stream << (std::string)Value; }

//----------------------------------------------------------------------------------------------------------------
// Unambiguous string display
inline std::string ClarifyString(std::string const &In)
{
	StringT Out;
	Out << "[";
	for (auto &Char : In)
	{
		Out << std::string(1, Char) << "(" << (int)Char << ")";
	}
	Out << "]";
	return Out;
}

//----------------------------------------------------------------------------------------------------------------
// A more informative assert?
inline void AssertStamp(char const *File, char const *Function, int Line)
        { std::cerr << File << "/" << Function << ":" << Line << " Assertion failed" << std::endl; }
 
template <typename ArgT, typename std::enable_if<!std::is_enum<ArgT>::value>::type * = nullptr>
	inline std::string AssertString(ArgT const &Arg)
	{ return StringT() << Arg; }
	
template <typename ArgT, typename std::enable_if<std::is_enum<ArgT>::value>::type * = nullptr> 
	inline std::string AssertString(ArgT const &Arg)
	{ return StringT() << static_cast<typename std::underlying_type<ArgT>::type>(Arg); }

inline std::string AssertString(unsigned char const &Arg)
	{ return StringT() << (int)Arg; }

template <typename Type> inline bool AssertImplementation(char const *File, char const *Function, int Line, char const *ValueString, Type const &Value)
{
#ifndef NDEBUG
	if (!Value)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Value (" << ValueString << ") '" << (bool)Value << "'" << std::endl;
		throw false;
	}
#endif
	return !!Value;
}

template <typename ValueT> struct IsVector
	{ static constexpr bool Result = false; };
template <typename InnerT> struct IsVector<std::vector<InnerT>> 
	{ static constexpr bool Result = true; };

template <typename ValueT, typename std::enable_if<IsVector<ValueT>::Result>::type * = nullptr> 
	inline bool AssertImplementationE(
		char const *File, 
		char const *Function, 
		int Line, 
		char const *GotString, 
		ValueT const &Got, 
		char const *ExpectedString, 
		ValueT const &Expected)
{
	size_t Offset = 0;
	auto GotIter = Got.begin();
	auto ExpectedIter = Expected.begin();

	while ((GotIter != Got.end()) && (ExpectedIter != Expected.end()))
	{
		if (*GotIter != *ExpectedIter)
		{
#ifndef NDEBUG
			AssertStamp(File, Function, Line);
			std::cerr << 
				"Got (" << GotString << ", index " << Offset << ") '" << AssertString(*GotIter) << "'"
				" != "
				"expected (" << ExpectedString << ", index " << Offset << ") '" << AssertString(*ExpectedIter) << "'" <<
				std::endl;
			throw false;
#endif
			return false;
		}
		++Offset;
		++GotIter;
		++ExpectedIter;
	}

	if ((GotIter == Got.end()) && (ExpectedIter == Expected.end())) 
		return true;

	if (ExpectedIter != Expected.end())
	{
#ifndef NDEBUG
		AssertStamp(File, Function, Line);
		std::cerr << 
			"Got (" << GotString << ", length " << Offset << ")"
			" shorter than "
			"expected (" << ExpectedString << ")" <<
			std::endl;
		throw false;
#endif
		return false;
	}

	if (GotIter != Got.end())
	{
#ifndef NDEBUG
		AssertStamp(File, Function, Line);
		std::cerr << 
			"Got (" << GotString << ")"
			" longer than "
			"expected (" << ExpectedString << ", length " << Offset << ")" <<
			std::endl;
		throw false;
#endif
		return false;
	}

	return true;
}

template 
<
	typename GotType, 
	typename ExpectedType, 
	typename std::enable_if<!IsVector<ExpectedType>::Result>::type * = nullptr
>
	inline bool AssertImplementationE(
		char const *File, 
		char const *Function, 
		int Line, 
		char const *GotString, 
		GotType const &Got, 
		char const *ExpectedString, 
		ExpectedType const &Expected)
{
	bool Result = Got == Expected;
#ifndef NDEBUG
	if (!Result)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Got (" << GotString << ") '" << Got << "' == expected (" << ExpectedString << ") '" << AssertString(Expected) << "'" << std::endl;
		throw false;
	}
#endif
	return Result;
}

template <typename GotType, typename ExpectedType> inline bool AssertImplementationNE(char const *File, char const *Function, int Line, char const *GotString, GotType const &Got, char const *ExpectedString, ExpectedType const &Expected)
{
	bool Result = Got != Expected;
#ifndef NDEBUG
	if (!Result)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Got (" << GotString << ") '" << AssertString(Got) << "' != expected (" << ExpectedString << ") '" << AssertString(Expected) << "'" << std::endl;
		throw false;
	}
#endif
	return Result;
}

template <typename GotType, typename ExpectedType> inline bool AssertImplementationLT(char const *File, char const *Function, int Line, char const *GotString, GotType const &Got, char const *ExpectedString, ExpectedType const &Expected)
{
	bool Result = Got < Expected;
#ifndef NDEBUG
	if (!Result)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Got (" << GotString << ") '" << Got << "' < expected (" << ExpectedString << ") '" << Expected << "'" << std::endl;
		throw false;
	}
#endif
	return Result;
}

template <typename GotType, typename ExpectedType> inline bool AssertImplementationLTE(char const *File, char const *Function, int Line, char const *GotString, GotType const &Got, char const *ExpectedString, ExpectedType const &Expected)
{
	bool Result = Got <= Expected;
#ifndef NDEBUG
	if (!Result)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Got (" << GotString << ") '" << Got << "' <= expected (" << ExpectedString << ") '" << Expected << "'" << std::endl;
		throw false;
	}
#endif
	return Result;
}
template <typename GotType, typename ExpectedType> inline bool AssertImplementationGT(char const *File, char const *Function, int Line, char const *GotString, GotType const &Got, char const *ExpectedString, ExpectedType const &Expected)
{
	bool Result = Got > Expected;
#ifndef NDEBUG
	if (!Result)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Got (" << GotString << ") '" << Got << "' > expected (" << ExpectedString << ") '" << Expected << "'" << std::endl;
		throw false;
	}
#endif
	return Result;
}
template <typename GotType, typename ExpectedType> inline bool AssertImplementationGTE(char const *File, char const *Function, int Line, char const *GotString, GotType const &Got, char const *ExpectedString, ExpectedType const &Expected)
{
	bool Result = Got >= Expected;
#ifndef NDEBUG
	if (!Result)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Got (" << GotString << ") '" << Got << "' >= expected (" << ExpectedString << ") '" << Expected << "'" << std::endl;
		throw false;
	}
#endif
	return Result;
}
template <typename Got1Type, typename Got2Type> inline bool AssertImplementationOr(char const *File, char const *Function, int Line, char const *Got1String, Got1Type const &Got1, char const *Got2String, Got2Type const &Got2)
{
	bool Result = Got1 || Got2;
#ifndef NDEBUG
	if (!Result)
	{
		AssertStamp(File, Function, Line);
		std::cerr << "Got #1 (" << Got1String << ") '" << Got1 << "' || got #2 (" << Got2String << ") '" << Got2 << "'" << std::endl;
		throw false;
	}
#endif
	return Result;
}

#define Assert(Arg1) AssertImplementation(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1)
#define AssertE(Arg1, Arg2) AssertImplementationE(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1, #Arg2, Arg2)
#define AssertNE(Arg1, Arg2) AssertImplementationNE(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1, #Arg2, Arg2)
#define AssertLT(Arg1, Arg2) AssertImplementationLT(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1, #Arg2, Arg2)
#define AssertLTE(Arg1, Arg2) AssertImplementationLTE(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1, #Arg2, Arg2)
#define AssertGT(Arg1, Arg2) AssertImplementationGT(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1, #Arg2, Arg2)
#define AssertGTE(Arg1, Arg2) AssertImplementationGTE(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1, #Arg2, Arg2)
#define AssertOr(Arg1, Arg2) AssertImplementationOr(__FILE__, __FUNCTION__, __LINE__, #Arg1, Arg1, #Arg2, Arg2)

#endif
