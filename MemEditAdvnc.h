#ifndef MEMEDIT_H
#define MEMEDIT_H
#include"Windows.h"
#include<cstdint>

// Little memory editing class from Uc

int filter( unsigned int code, LPEXCEPTION_POINTERS ep ) // correct parameter name for pointer to struct _EXCEPTION_POINTERS(EXCEPTION_POINTERS*)
{
	//handle only Access Violation
	// If access violation it will execute the code in handler or else execute the Execption search
	return code == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;

}

class MemEdit
{
public:
	MemEdit() = default;

	template<typename T>
	T readPtr( ptrdiff_t Address, const T& def_val = T() ) // Give a default value To check if readPtr fails e.g. like 5126
	{
		__try
		{
			return *(T*) (Address);
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			puts( "Wrong value derefrenced" );
			return def_val;
		}
	}

	template<typename T>
	bool  writePtr( ptrdiff_t Address, T Value )
	{
		__try
		{
			*(T*) (Address) = Value;
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			return false;
		}
		return true;
	}

	template<typename T>
	bool add( ptrdiff_t Address, T Value )
	{
		__try
		{
			*(T*) Address += Value;
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			return false;
		}
		return true;
	}

	template <typename T>
	bool sub( ptrdiff_t Address, T Value )
	{
		__try
		{
			*(T*) Address -= Value;
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			return false;
		}
		return true;
	}

	template <typename T>
	bool multiply( ptrdiff_t Address, T Value )
	{
		__try
		{
			*(T*) Address *= Value;
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			return false;
		}
		return true;
	}

	template <class T>
	bool divide( ptrdiff_t Address, T Value )
	{
		__try
		{
			*(T*) Address /= Value;
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			return false;
		}
		return true;
	}

	template <typename T>
	T* makePtr( ptrdiff_t addrs ) // getting  a cast to object of type T or make pointer of some address
	{
		__try
		{
			return (T*) addrs;
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			return nullptr;
		}
	}

	template <typename T>
	// T* is static/global-pointer to dynamic object structure(dT);  static-address value "addrs" is POINTER to  Base-addres of Object/ THIS pointer to Object.(found from CE)
	T* pObjectThis( ptrdiff_t addrs )
	{
		__try
		{
			return (*(T**) addrs);
		}
		__except (filter( GetExceptionCode(), GetExceptionInformation() ))
		{
			return nullptr;
		}
	}	
};

// MemEdit memedit;
//readPtring memmory:
//int hue= memedit.readPtr<int>(0x1337,1);
//where "0x1337" is the address and "1" is the default value
//Writing memory:
//memedit.writePtr<float>( 0x1337, 0.1f );
//memedit.add<int>( 0x1337, 2 );
//Setting a pointer to a game-object from  :
//memedit.pObjectThis<T>(addrs).
//Casting a pointer of type T from hexadecimal address
// memedit.makePtr<T>(addrs)
#endif