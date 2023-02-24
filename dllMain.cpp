#include<iostream>
#include<Windows.h>
#include"Vector.h"
#include"Player.h"
// note "clientBase + dwLocalPlayer" gives an address which contains LOcalPlayer Base address
// The LocalPlayer Base address can also be thought as PTHIS or THIS pointer to local Player struct
// This has to be dreferenced again to get a value of the offset
// double pointer casting and dereferncing once *reinterpret_cast<LocalPlayer**> to get a pointer from [pointer of pointer]
extern MemEdit memEdit;
DWORD WINAPI myThreadPtrProc( HMODULE hinstDll )
{
	LocalPlayer* playerBaseAddrs = LocalPlayer::getLocalPlayerPtr();
	AllocConsole();
	FILE* f;
	freopen_s( &f, "CONOUT$", "w", stdout );

	std::cout << "[?]LocalPlayerAddres:\t" << playerBaseAddrs << '\n';
	
	//std::cout << "[?]LocalPlayerTeam:\t" << *(intptr_t*)(*(ptrdiff_t*) playerBaseAddrs+ 0xf4)<< '\n';
	//std::cout << "[?]LocalPlayerHealth:\t" << *(intptr_t*) (*(ptrdiff_t*) playerBaseAddrs + 0x100) << '\n';

	std::cout << "[?]LocalPlayerTeam:\t" <<  ( playerBaseAddrs->iTeamNum) << '\n';
	std::cout << "[?]LocalPlayerHealth:\t" <<  ( playerBaseAddrs->iHealth) << '\n';
	std::cout << "Player view angles address: " << std::hex << playerBaseAddrs->getViewAnglesPtr() << '\n';
	// int maxPlayer = EntityInstance::GetMaxPlayer();
	EntityInstance* entityInstances = EntityInstance::getEntityInstancePtr();
	//entityInstances->array_EntityPtr.resize( 64 );
	/*for (int i{0}; i < 64; ++i)
	{
		if (!entityInstances->checkValidEnt( i ))
			continue;
		LocalPlayer* currentEntity = entityInstances->GetOtherEntity(i);

		std::cout << "Entity address:\t" << currentEntity << '\n';
		// this final pointer after dereferencing at offset m_dwbonematrix i.e whatever value the enitityClass is having at member m_dwBoneMatrix
		std::cout << "Entity boneMatrix ptr" << std::hex << currentEntity->m_dwBoneMatrix << '\n';
		// final value at offset iHealth member after dereferencing at class Member  iHealth address
		std::cout << "Entities Health:\t" << std::dec << currentEntity->iHealth << '\n';
		std::cout << " Bone position vector for entity is: " << "__X " << currentEntity->GetBonePosition( 8 ).m_x << "__Y " << currentEntity->GetBonePosition( 8 ).m_y << "__Z " << currentEntity->GetBonePosition( 8 ).m_z << '\n'; 
		
	}  */

	while (!GetAsyncKeyState( VK_INSERT ) & 1)
	{

		LocalPlayer* closestEnemy = entityInstances->GetClosestEnemy();
		Vector3 bonePos {};
		if (closestEnemy)
		{
			playerBaseAddrs->aimAt( closestEnemy->GetBonePosition(8) );

		}                                 //( closestEnemy->GetBonePosition(8) );

	Sleep( 1 );
	}


	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread( hinstDll, 0 );
	return 0;
}



BOOL APIENTRY DllMain( HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved )
{
	switch (fdwReason)
	{

		case DLL_PROCESS_ATTACH:
		{
			::DisableThreadLibraryCalls( hinstDll );
			CreateThread( nullptr, 0, LPTHREAD_START_ROUTINE( myThreadPtrProc ), hinstDll, 0, nullptr );
		}
		break;
		case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}