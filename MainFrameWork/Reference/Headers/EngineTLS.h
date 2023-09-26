#pragma once
#include <stack>

extern thread_local unsigned __int32		LEngineThreadId;
extern thread_local std::stack<__int32>		LEngineLockStack;