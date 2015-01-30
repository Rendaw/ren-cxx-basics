#ifndef ren_cxx_basics__stricttype_h
#define ren_cxx_basics__stricttype_h

#include "extrastandard.h"

// Strict types (no implicit casting)
/* 
Usage:

typedef StrictType(int) MyInt;
MyInt x(4);

typedef StrictType(float) MyFloat;
MyFloat y(5.0f);
MyFloat z((float)5);

y = StrictCast(x, MyFloat);

*/

template <size_t Uniqueness, typename ValueT, typename Enabled = void> 
	struct ExplicitCastableT;

template <size_t Uniqueness, typename ValueT> 
	struct ExplicitCastableT
	<
		Uniqueness, 
		ValueT, 
		typename std::enable_if<!std::is_integral<ValueT>::value>::type
	>
{
	typedef ExplicitCastableT<Uniqueness, ValueT> ThisT;
	
	ExplicitCastableT(void) {}
	ExplicitCastableT(ThisT const &That) : Value(*That) {}
	explicit ExplicitCastableT(ValueT const &That) : Value(That) {}
	template <typename ThatT> ExplicitCastableT(ThatT const &That) : Value(static_cast<ValueT>(That)) {}
	
	ValueT &operator *(void) { return Value; }
	ValueT const &operator *(void) const { return Value; }
	
	//template <typename ...ArgumentsT> constexpr decltype(reinterpret_cast<ValueT *>(nullptr)->operator ()) operator()(ArgumentsT... Arguments)
	//	{ return Value(Arguments...); }
	template <typename ...ArgumentsT> auto operator()(ArgumentsT... Arguments)
		-> typename std::result_of<ValueT(ArgumentsT...)>::type
		{ return Value(std::forward<ArgumentsT>(Arguments)...); }
	template <typename ...ArgumentsT> auto operator()(ArgumentsT... Arguments) const
		-> typename std::result_of<ValueT(ArgumentsT...)>::type
		{ return Value(std::forward<ArgumentsT>(Arguments)...); }

	constexpr static size_t Size = sizeof(ValueT);
	typedef ValueT Type;
	ValueT Value;
};
template <size_t Uniqueness, typename ValueT> 
	constexpr size_t ExplicitCastableT
	<
		Uniqueness, 
		ValueT, 
		typename std::enable_if<!std::is_integral<ValueT>::value>::type
	>::Size;

template <size_t Uniqueness, typename ValueT> 
	struct ExplicitCastableT
	<
		Uniqueness, 
		ValueT, 
		typename std::enable_if<std::is_integral<ValueT>::value>::type
	>
{
	typedef ExplicitCastableT<Uniqueness, ValueT> ThisT;

	constexpr ExplicitCastableT(void) {}
	constexpr ExplicitCastableT(ThisT const &That) : Value(*That) {}
	explicit constexpr ExplicitCastableT(ValueT const &That) : Value(That) {}
	template <typename ThatT> 
		explicit constexpr ExplicitCastableT(ThatT const &That) : Value(static_cast<ValueT>(That)) {}

	ValueT &operator *(void) { return Value; }
	constexpr ValueT const &operator *(void) const { return Value; }
	constexpr static size_t Size = sizeof(ValueT);

	constexpr ThisT operator +(ThisT const &That) const { return ThisT(Value + *That); }
	constexpr ThisT operator +(ValueT const &That) const { return ThisT(Value + That); }
	template <typename ThatT> ThisT operator +(ThatT const &) const = delete;

	constexpr ThisT operator -(ThisT const &That) const { return ThisT(Value - *That); }
	constexpr ThisT operator -(ValueT const &That) const { return ThisT(Value - That); }
	template <typename ThatT> ThisT operator -(ThatT const &) const = delete;

	constexpr ThisT operator *(ThisT const &That) const { return ThisT(Value * *That); }
	constexpr ThisT operator *(ValueT const &That) const { return ThisT(Value * That); }
	template <typename ThatT> ThisT operator *(ThatT const &) const = delete;

	constexpr ThisT operator /(ThisT const &That) const { return ThisT(Value / *That); }
	constexpr ThisT operator /(ValueT const &That) const { return ThisT(Value / That); }
	template <typename ThatT> ThisT operator /(ThatT const &) const = delete;

	template <typename ThatT> ThisT operator +=(ThatT const &) = delete;
	ThisT operator +=(ThisT const &That) { return ThisT(Value += *That); }
	ThisT operator +=(ValueT const &That) { return ThisT(Value += That); }

	ThisT operator ++(void) { return ThisT(++**this); }
	ThisT operator ++(int) { auto Out = ThisT(**this); ThisT(++**this); return Out; }

	ThisT operator -=(ThisT const &That) { return ThisT(Value -= *That); }
	ThisT operator -=(ValueT const &That) { return ThisT(Value -= That); }
	template <typename ThatT> ThisT operator -=(ThatT const &) = delete;

	ThisT operator --(void) { return --**this; }
	ThisT operator --(int) { auto Out = ThisT(**this); --**this; return Out; }

	ThisT operator *=(ThisT const &That) { return ThisT(Value *= *That); }
	ThisT operator *=(ValueT const &That) { return ThisT(Value *= That); }
	template <typename ThatT> ThisT operator *=(ThatT const &) = delete;

	ThisT operator /=(ThisT const &That) { return ThisT(Value /= *That); }
	ThisT operator /=(ValueT const &That) { return ThisT(Value /= That); }
	template <typename ThatT> ThisT operator /=(ThatT const &) = delete;

	constexpr bool operator ==(ThisT const &That) const { return Value == *That; }
	constexpr bool operator ==(ValueT const &That) const { return Value == That; }
	template <typename ThatT> constexpr bool operator ==(ThatT const &) const = delete;

	constexpr bool operator !=(ThisT const &That) const { return Value != *That; }
	constexpr bool operator !=(ValueT const &That) const { return Value != That; }
	template <typename ThatT> constexpr bool operator !=(ThatT const &) const = delete;

	constexpr bool operator <(ThisT const &That) const { return Value < *That; }
	constexpr bool operator <(ValueT const &That) const { return Value < That; }
	template <typename ThatT> constexpr bool operator <(ThatT const &) const = delete;

	constexpr bool operator >(ThisT const &That) const { return Value > *That; }
	constexpr bool operator >(ValueT const &That) const { return Value > That; }
	template <typename ThatT> constexpr bool operator >(ThatT const &) const = delete;

	constexpr bool operator <=(ThisT const &That) const { return Value <= *That; }
	constexpr bool operator <=(ValueT const &That) const { return Value <= That; }
	template <typename ThatT> constexpr bool operator <=(ThatT const &) const = delete;

	constexpr bool operator >=(ThisT const &That) const { return Value >= *That; }
	constexpr bool operator >=(ValueT const &That) const { return Value >= That; }
	template <typename ThatT> constexpr bool operator >=(ThatT const &) const = delete;
	
	typedef ValueT Type;
	ValueT Value;
};
template <size_t Uniqueness, typename ValueT> 
	constexpr size_t ExplicitCastableT
	<
		Uniqueness, 
		ValueT, 
		typename std::enable_if<std::is_integral<ValueT>::value>::type
	>::Size;

template <size_t Uniqueness, typename ExplicitT> 
	std::ostream &operator <<(std::ostream &Stream, ExplicitCastableT<Uniqueness, ExplicitT> const &Value)
	{ Stream << *Value; return Stream; }

#define StrictType(Type) ::ExplicitCastableT<__COUNTER__, Type>
//#define StrictCast(Value, ToType) (Value)(ExplicitT<ToType>())
#define StrictCast(StrictValue, ToType) static_cast<ToType>((StrictValue).Value)

#endif
