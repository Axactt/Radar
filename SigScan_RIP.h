#ifndef SIGSCAN_H
#define SIGSCAN_H
#include <Windows.h>
#include<Psapi.h>
#include <iostream>
#include <string>



class MemoryScanner
{
private:
	HMODULE hModule {};
	ptrdiff_t baseAddress {};
	ptrdiff_t moduleLength {};
	ptrdiff_t m_address_found {};

public:
	MemoryScanner( const char* moduleName )
	{

		// Get the handle to the target process's module
		hModule = ::GetModuleHandleA( moduleName );
		if (hModule == NULL)
		{
			std::cout << "Could not retrieve the handle to the target process's module." << std::endl;
			return;
		}

		// Get information about the target process's module
		MODULEINFO moduleInfo;
		GetModuleInformation( GetCurrentProcess(), hModule, &moduleInfo, sizeof( moduleInfo ) );

		// Get the base m_address_found and size of the target process's module
		baseAddress = (ptrdiff_t) moduleInfo.lpBaseOfDll;
		moduleLength = moduleInfo.SizeOfImage;

		// Check the searchable reason of the target process's memory
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery( (LPCVOID) baseAddress, &mbi, sizeof( mbi ) );
		if (mbi.State != MEM_COMMIT)
		{
			std::cout << "Could not retrieve the base_address of the target process's memory." << std::endl;
		}

		// Change the memory protection of the target process's memory to allow read and write
		//VirtualProtect( (LPVOID) baseAddress, moduleLength, PAGE_EXECUTE_READWRITE, &oldProtect );
	}
	// destructor to reset class variables and de-allocate any resources used
	~MemoryScanner()
	{
		hModule = 0;
		baseAddress = 0;
		moduleLength = 0;
		m_address_found = 0;
	}

	bool DataCompare( const byte* pData, const byte* bMask, const char* szMask )
	{
		for (; *szMask; ++szMask, ++pData, ++bMask)
		{
			if (*szMask == 'x' && *pData != *bMask)
			{
				return false;
			}
		}
		return (*szMask) == 0;
	}

	MemoryScanner& FindPattern( byte* bMask, char* szMask )
	{
		for (ptrdiff_t i = 0; i < moduleLength; i++)
		{
			if (DataCompare( (byte*) (baseAddress + i), bMask, szMask ))
			{
				m_address_found = (ptrdiff_t) (baseAddress + i);
				return *this;
			}
		}
		std::cout << "Pattern not found." << std::endl;
		return *this;
	}

	MemoryScanner& add( ptrdiff_t offset )
	{
		m_address_found += offset;
		return *this;
	}
	MemoryScanner& sub( ptrdiff_t offset )
	{
		m_address_found -= offset;
		return *this;
	}

	// Patching byttes with user provided payload
	//At the desired address found write user provided payload
	//Provide address, payload and payload length to the function. get payload length using sizeof(paLoad)
	void patchByte( ptrdiff_t address, const char* payLoad, unsigned int payLoadLen )
	{
		DWORD oldProtect {};
		VirtualProtect( (LPVOID) address, payLoadLen, PAGE_EXECUTE_READWRITE, &oldProtect );
		RtlMoveMemory( (byte*) address, payLoad, payLoadLen );
		VirtualProtect( (LPVOID) address, payLoadLen, oldProtect, &oldProtect );
	}

	ptrdiff_t GetAddress() // This gives the address of the first opcode byte from where pattern was copied
	{
		return m_address_found;
	}

	ptrdiff_t GetAbsoluteAddress( int whole_inst_size, int ofst_firstInstByt_firstAddrByt )
	{
		ptrdiff_t  rip_address = m_address_found + whole_inst_size; // this gives address of RIP which is address of next instruction start
		//This instruction adds the offset no. of bytes between between the first byte of instruction and first byte of address to the address found
		// Then it is derefernced to read value of SIGNED INTEGER 32 bit to get the value of relative offset in __int32 hence *(INT32*)
		//This is then added to rip_address(ADDRESS NEXT INSTTUCTION) calculated above to get the concerned absolute address value contained in form of [rel_int32].
		//note use of *(INT32*) signed relative offset to go up/down from curent position
		ptrdiff_t absolute_address = rip_address + *(INT32*) (m_address_found + ofst_firstInstByt_firstAddrByt);

		return absolute_address;
	}
};
/*
int main()
{
	// Create an instance of the MemoryScanner class, passing the name of the module you want to scan as the parameter
	MemoryScanner ms( "example.exe" );

	// Search for a pattern of bytes in the target process's memory
	byte bMask[] = { 0x12, 0x34, 0x56, 0x78 };
	char szMask[] = "xxxx";
	ms.FindPattern( bMask, szMask );

	// Add an offset to the found m_address_found
	ms.add( 0x4 );

	// Write new bytes to the desired location in memory
	byte bWrite[] = { 0x11, 0x22, 0x33, 0x44 };
	ms.WriteBytes( bWrite, sizeof( bWrite ) );

	return 0;
}  */

#endif