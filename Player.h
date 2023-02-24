#pragma once
#include<array>
#include<vector>
#include"Vector.h"
#include"MemEditAdvnc.h"
#pragma region Address
#define clientBase  (ptrdiff_t)GetModuleHandleA("client.dll")
#define engineBase  (ptrdiff_t)GetModuleHandleA("engine.dll")
#define dwLocalPlayer (ptrdiff_t)0xDEA964
#define dwEntityInstance (ptrdiff_t)0x4DFFEF4
#define dwClientState (ptrdiff_t)0x59F19C
#define dwClientState_MaxPlayer  (ptrdiff_t)0x388
#define dwClientState_ViewAngles (ptrdiff_t) 0x4D90

#pragma endregion

MemEdit memEdit {};




/*
class OtherPlayer : public LocalPlayer
{
public:
	OtherPlayer* GetOtherPlayer(int index)
	{
		return memEdit.makePtr<OtherPlayer>(clientBase + dwEntityInstance + index * 0x10);

	}
	bool checkValidEnt(int index)
	{
		OtherPlayer* entity = GetOtherPlayer( index );

		if (!entity)
			return false;
		if (entity == getLocalPlayerPtr())
			return false;
		if (entity->iHealth <= 0)
			return false;
		if (entity->isDormant)
			return false;
		if (entity->iTeamNum == getLocalPlayerPtr()->iTeamNum)
			return false;
		return true;
	}
	static int GetMaxPlayer()
	{
		ptrdiff_t maxPlayerAdd = *(ptrdiff_t*) (engineBase + dwClientState) + dwClientState_MaxPlayer;
		return memEdit.readPtr<int>( maxPlayerAdd, 0 );
	}

};   */


class LocalPlayer
{

	// This is entity object class which represents local entity (player entity) structure
	//In game memory. the padding is being done to represent actual localplayer object
	//The required padding is to reach the class object structure members at various offsets
	// various important structure members chosen with their offsets
	// so these can be directly accessed by using as "class LocalEntity" members
	//As class Ent will be assigned actual running in-game address
		//These members when accessed at defined original padding(offset) 
	// can be made to readPtr and access the real-time values of game in runtime
public:
	LocalPlayer() = default;
	// isDormant;
	char pad1[0xED];
	bool isDormant; // 0xED(0x01) ;

	//iTeamNum
	char pad2[0x6];
	int iTeamNum; //{ 0xF4 };(0x04)

	//iHealth
	char pad3[0x8];
	int iHealth;// { 0x100 };(0x04)

	char padz[0x04];
	Vector3 m_vecViewOffset;// {0x108}; (0xc)

	//vecorigin
	char pad4[0x24];
	Vector3  vecOrigin; //{ 0x138 };(0xc)

	//b_spotted by wall
	char pad8[0x83c];
	bool m_bSpottedByMask; // {0x980}; (0x01)

	// bone_matrix
	//char pad5[0X255E];
	//Matrix34 m_dwBoneMatrix; // { 0x26A8 };(0x48)  // use Matrix34 struct here for bone-position of size 4x12(float) bytes
	// bone_matrix
	char pad5[0X1D27];
	int m_dwBoneMatrix; // { 0x26A8 };(0x4) 

	//aim_punch_angle
	char pad6[0X990];
	Vector3 m_aimPunchAngle; //{ 0x303C };(0xc)

	//armor-value
	char pad7[0xE784];
	int  m_ArmorValue;// { 0x117CC };(0X4)

	static LocalPlayer* getLocalPlayerPtr() // to get a this pointer of object or maybe call base-address of object
	{
		return memEdit.pObjectThis<LocalPlayer>( clientBase+dwLocalPlayer);

	}
	
	Vector3* getViewAnglesPtr()
	{
		ptrdiff_t viewAngleAddrs = { memEdit.readPtr<ptrdiff_t>(engineBase+dwClientState) + dwClientState_ViewAngles};
		Vector3* viewAnglesPtr = memEdit.makePtr<Vector3>( viewAngleAddrs );
		return viewAnglesPtr;
	}

	void aimAt(const Vector3& target)
	{
		static Vector3* viewAngles = this->getViewAnglesPtr();
		Vector3 originPos = this->vecOrigin;
		Vector3 viewOffset = this->m_vecViewOffset;
		Vector3 myFinalVec = originPos + viewOffset; // vector of LocalPlayer eyePosition
		Vector3 deltaVector = target - myFinalVec; // difference Vector3 between LocalPlayer and other entity
		float deltaVectorLength = deltaVector.Length(); // Length of difference vector
		float pitch = (float)-asin( deltaVector.m_z / deltaVectorLength ) * (180 / M_PI);
		float yaw = (float)atan2 ( deltaVector.m_y , deltaVector.m_x ) * (180 / M_PI);

		if (pitch >= -89 && pitch <= 89 && yaw >= -180 && yaw <= 180) // clamping angles to maximumdefined angles in game
		{
			viewAngles->m_x = pitch; // set the view angles by dereference operator
			viewAngles->m_y = yaw;
			//*(float*) ((ptrdiff_t) viewAngles) = pitch;  same as above
			//*(float*) ((ptrdiff_t) viewAngles + 0x04) = yaw;
		}

	}
	Vector3 GetBonePosition( int boneID )
	{
		Vector3 bonePos {};
		ptrdiff_t boneMatrixPtr = this->m_dwBoneMatrix;
		bonePos.m_x = *(float*) (boneMatrixPtr + 0x30 * boneID + 0x0c);
		bonePos.m_y = *(float*) (boneMatrixPtr + 0x30 * boneID + 0x1c);
		bonePos.m_z = *(float*) (boneMatrixPtr + 0x30 * boneID + 0x2C);
		return bonePos;
	}
};

class EntityInstance :public LocalPlayer
{
public:
	
	std::vector < LocalPlayer*> array_EntityPtr{};

public:
	EntityInstance() = default;
	static EntityInstance* getEntityInstancePtr()
	{
		return memEdit.makePtr<EntityInstance>( clientBase + dwEntityInstance );
	}
	static int GetMaxPlayer()
	{
		ptrdiff_t maxPlayerAdd = *(ptrdiff_t*) (engineBase + dwClientState) + dwClientState_MaxPlayer;
		return memEdit.readPtr<int>( maxPlayerAdd, 0 );
	}
	EntityInstance& resizeEntityArray()
	{
		array_EntityPtr.resize( GetMaxPlayer() );
		return *this;
	}  
	LocalPlayer* GetOtherEntity( int index )
	{
		ptrdiff_t entityPtr = (ptrdiff_t) getEntityInstancePtr() + index * 0x10;
		 ptrdiff_t localPlayerptr= memEdit.readPtr<ptrdiff_t>( entityPtr,1337 );
		 return memEdit.makePtr<LocalPlayer>( localPlayerptr );
	}
	bool checkValidEnt( int index )
	{
		LocalPlayer* entity = GetOtherEntity( index );

		if (!entity)
			return false;
		if (entity == getLocalPlayerPtr())
			return false;
		if (entity->iHealth <= 0)
			return false;
		if (entity->isDormant)
			return false;
		if (entity->iTeamNum == getLocalPlayerPtr()->iTeamNum)
			return false;
		if (entity->m_bSpottedByMask)
			return false;
		return true;
	}
	

	LocalPlayer* GetClosestEnemy(   ) // Get closest enemy to player
	{
		float closestDistance = 1000000;
		int closestDistanceIndex = -1;

		LocalPlayer* localPlayer = getLocalPlayerPtr();
		for(int i{0};i<64;++i)
		{
			if (!checkValidEnt( i ))
				continue;
		
			LocalPlayer* currentPlayer = GetOtherEntity( i );
			
			Vector3 LocalPlayerPos = (localPlayer->vecOrigin) + (localPlayer->m_vecViewOffset);
			
			Vector3 otherPlayerPos = [&]()  // Getting bone position by Lambda
			{
				Vector3 bonePos {};
				ptrdiff_t boneMatrixPtr = GetOtherEntity( i )->m_dwBoneMatrix;
				bonePos.m_x = *(float*) (boneMatrixPtr + 0x30 * 8 + 0x0c);
				bonePos.m_y = *(float*) (boneMatrixPtr + 0x30 * 8 + 0x1c);
				bonePos.m_z = *(float*) (boneMatrixPtr + 0x30 * 8 + 0x2C);
				return bonePos;
			}();

			float distanceDiff = LocalPlayerPos.DistanceTo( otherPlayerPos );
			if (distanceDiff < closestDistance)
			{
				closestDistance = distanceDiff;
				closestDistanceIndex = i;
			}
		}
		if (closestDistanceIndex == -1)
		{
			return nullptr;
		}
		return GetOtherEntity( closestDistanceIndex );
	}	
};