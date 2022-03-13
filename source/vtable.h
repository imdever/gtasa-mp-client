#pragma once
#include <Windows.h>

class VTableHookManager
{
public:
	VTableHookManager(void** vTable, unsigned short numFuncs) : m_vTable(vTable), m_numberFuncs(numFuncs)
	{
		m_originalFuncs = new void* [m_numberFuncs]; // allocating memory so we can save here the addresses of the original functions
		for (int i = 0; i < m_numberFuncs; i++)
		{
			m_originalFuncs[i] = GetFunctionAddyByIndex(i); // saving the address of the original functions
		}
	}

	~VTableHookManager()
	{
		delete[] m_originalFuncs; //we need to free the allocated memory
	}

	void* GetFunctionAddyByIndex(unsigned short index) // getting the address of a virtual function in the vtable by index
	{
		if (index < m_numberFuncs)
		{
			return m_vTable[index];
		}
		else
		{
			return nullptr;
		}
	}

	void* Hook(unsigned short index, void* ourFunction) // hooking the virtual function by index
	{
		uintptr_t bufferOriginalFunc = NULL;
		if (!toHook(index, true, ourFunction, &bufferOriginalFunc))
		{
			return nullptr;
		}
		return reinterpret_cast<void*>(bufferOriginalFunc);
	}

	bool Unhook(unsigned short index) // unhooking the virtual function by index
	{
		if (!toHook(index))// if not succeded
		{
			return false; // return false
		}
		return true; // else return true
	}

	void UnhookAll()
	{
		for (int index = 0; index < m_numberFuncs; index++)
		{
			if (m_vTable[index] == m_originalFuncs[index])
			{
				continue; // if not hooked skip this index
			}
			Unhook(index);
		}
	}

private:
	void** m_vTable; // the vtable of some object
	unsigned short m_numberFuncs; // number of virtual functions
	void** m_originalFuncs = nullptr; // we'll save the original address here

	bool toHook(unsigned short index, bool hook = false, void* ourFunction = nullptr, uintptr_t* bufferOriginalFunc = nullptr)
	{
		DWORD OldProtection = NULL;
		if (index < m_numberFuncs)
		{
			if (hook)
			{
				if (!ourFunction || !bufferOriginalFunc)
				{
					return false;
				}
				*bufferOriginalFunc = (uintptr_t)m_vTable[index]; // saving the original address in our buffer so we can call the function
				VirtualProtect(m_vTable + index, 0x4, PAGE_EXECUTE_READWRITE, &OldProtection);
				m_vTable[index] = ourFunction;
				VirtualProtect(m_vTable + index, 0x4, OldProtection, &OldProtection);
				return true;
			}
			else
			{
				VirtualProtect(m_vTable + index, 0x4, PAGE_EXECUTE_READWRITE, &OldProtection);
				m_vTable[index] = m_originalFuncs[index];
				VirtualProtect(m_vTable + index, 0x4, OldProtection, &OldProtection);
				return true;
			}
		}
		return false;
	}
};