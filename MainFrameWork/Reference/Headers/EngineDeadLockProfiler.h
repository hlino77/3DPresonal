#pragma once
#include <vector>
#include <unordered_map>
#include <set>
#include "AsTypes.h"
#include <mutex>
#include "Base.h"

class CEngineDeadLockProfiler : public CBase
{
	DECLARE_SINGLETON(CEngineDeadLockProfiler)

private:
	CEngineDeadLockProfiler();
	virtual ~CEngineDeadLockProfiler() = default;

public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32>	_nameToId;
	unordered_map<int32, const char*>	_idToName;
	map<int32, set<int32>>				_lockHistory;

	std::mutex _lock;

private:
	vector<int32>	_discoveredOrder; // ��尡 �߰ߵ� ������ ����ϴ� �迭
	int32			_discoveredCount = 0; // ��尡 �߰ߵ� ����
	vector<bool>	_finished; // Dfs(i)�� ���� �Ǿ����� ����
	vector<int32>	_parent;

public:
	virtual void Free() override;
};

