#ifndef ren_cxx_basics__error_h
#define ren_cxx_basics__error_h

#include "extrastandard.h"
#include <string>
#include <sstream>

// Exception for failed construction
struct ConstructionErrorT
{
	ConstructionErrorT(void) {}
	ConstructionErrorT(ConstructionErrorT const &Other) : Buffer(Other.Buffer.str()) {}
	template <typename Whatever> ConstructionErrorT &operator <<(Whatever const &Input) 
		{ Buffer << Input; return *this; }
	operator std::string(void) const { return Buffer.str(); }

	private:
		std::stringstream Buffer;
};

inline std::ostream& operator <<(std::ostream &Out, ConstructionErrorT const &Error)
	{ Out << static_cast<std::string>(Error); return Out; }

#endif
