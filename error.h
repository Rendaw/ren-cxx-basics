#ifndef ren_cxx_basics__error_h
#define ren_cxx_basics__error_h

#include "extrastandard.h"
#include <string>
#include <sstream>

#define DEFINE_ERROR(Name) \
	struct Name \
	{ \
		Name(char const *TraceFile, char const *TraceFunction, int TraceLine) \
		{ \
			Buffer << TraceFile << " " << TraceFunction << ":" << TraceLine << " "; \
		} \
		\
		Name(Name const &Other) : Buffer(Other.Buffer.str()) {} \
		\
		template <typename Whatever> Name &operator <<(Whatever const &Input) \
			{ Buffer << Input; return *this; } \
		\
		operator std::string(void) const { return Buffer.str(); } \
		\
		private: \
			std::stringstream Buffer; \
	}; \
	\
	inline std::ostream& operator <<(std::ostream &Out, Name const &Error) \
		{ Out << static_cast<std::string>(Error); return Out; }

// Exception for failed construction
DEFINE_ERROR(ConstructionErrorT)
#define CONSTRUCTION_ERROR ConstructionErrorT(__FILE__, __FUNCTION__, __LINE__)

// Exception for an error outside the program's control, may not be user error
DEFINE_ERROR(SystemErrorT)
#define SYSTEM_ERROR SystemErrorT(__FILE__, __FUNCTION__, __LINE__)

// Exception for user caused issue
DEFINE_ERROR(UserErrorT)
#define USER_ERROR UserErrorT(__FILE__, __FUNCTION__, __LINE__)

// Exception for supposedly dead code
DEFINE_ERROR(AssertionErrorT)
#define ASSERTION_ERROR AssertionErrorT(__FILE__, __FUNCTION__, __LINE__)

#endif
