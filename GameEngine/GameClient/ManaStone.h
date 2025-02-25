#pragma once
#include "ClientComponent.h"
#include "MeshFilterSetting.h"
#include <vector>
class MeshFilter;
class Transform;
class GameObject;
class AnimationController;
class MonsterA;

class Collider;
class Rigidbody;
class ParticleController;

class ManaStone :public ClientComponent
{
public:
	ManaStone();
	~ManaStone();
	void Awake() override;
	void SetUp() override;
	void Start() override;
	void Update() override;
	void Debug() override;

	void OnTriggerStay(GameObject* Obj);
	
	void SetMonsterCount(int MonsterA, int MonsterB);
	Vector3 GetPoint(int CreateRangeIndex, int MovePointIndex);
	void CreateMonster(int MonsterACount, int MonsterBCount);
	void Delete();
private:
	MeshFilter* mMeshFilter;
	Transform*	mTransform;
	AnimationController* mAnimation;
	Collider*	mCollider = nullptr;;
	Rigidbody*	mRigidbody = nullptr;
private:
	void CreateMonsterA(int index);
	void CreateMonsterB(int index);

	Vector3 GetMonsterPoint(const Vector3& MyPosition, int MonsterPointIndex , float Range);
	void CreateMonsterRangePoint(int MonsterCount);

	MeshFilterSetting mSetting;

	static std::vector<Vector3> MonsterMovePointDefault;
	static int MaxManaCount;

	Vector3 MovePoint[5];
	std::vector<MonsterA*> MonsterA_List;
	std::vector<MonsterB*> MonsterB_List;


	bool Boss_Start = false;
	bool HitStart = false;
	bool DeadStart = false;
	float Range = 12;
	float HP  = 60;
	int MonsterACount = 0;
	int MonsterBCount = 0;

	// �����ʴ� �ھ�� ȹ�� ����
	int CoreManaCount = 1;

	ParticleController* mRangeParticle;
};

