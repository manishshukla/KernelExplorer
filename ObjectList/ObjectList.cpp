// ObjectList.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "..\KExploreHelper\KExploreHelper.h"
#include "..\KExplore\KExploreClient.h"

typedef struct _SYSTEM_OBJECTTYPE_INFORMATION {
	ULONG NextEntryOffset;
	ULONG NumberOfObjects;
	ULONG NumberOfHandles;
	ULONG TypeIndex;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccessMask;
	ULONG PoolType;
	BOOLEAN SecurityRequired;
	BOOLEAN WaitableObject;
	UNICODE_STRING TypeName;
} SYSTEM_OBJECTTYPE_INFORMATION, *PSYSTEM_OBJECTTYPE_INFORMATION;

typedef struct _OBJECT_NAME_INFORMATION {
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct _SYSTEM_OBJECT_INFORMATION {
	ULONG NextEntryOffset;
	PVOID Object;
	HANDLE CreatorUniqueProcess;
	USHORT CreatorBackTraceIndex;
	USHORT Flags;
	LONG PointerCount;
	LONG HandleCount;
	ULONG PagedPoolCharge;
	ULONG NonPagedPoolCharge;
	HANDLE ExclusiveProcessId;
	PVOID SecurityDescriptor;
	OBJECT_NAME_INFORMATION NameInfo;
} SYSTEM_OBJECT_INFORMATION, *PSYSTEM_OBJECT_INFORMATION;

int Error(const char* message) {
	printf("%s (error=%d)\n", message, ::GetLastError());
	return 1;
}

int main() {
	auto hDevice = KExploreHelper::OpenDriverHandleAllTheWay(L"kexplore");
	if (hDevice == INVALID_HANDLE_VALUE) {
		return Error("Failed tet driver handle");
	}

	int size = 0x20000;
	auto buffer = std::make_unique<BYTE[]>(size);

	DWORD returned;
	auto success = ::DeviceIoControl(hDevice, static_cast<DWORD>(KExploreIoctls::EnumObjecs), nullptr, 0, buffer.get(), size, &returned, nullptr);
	if (!success)
		return Error("Failed to enumerate objects");

	auto info = (SYSTEM_OBJECTTYPE_INFORMATION*)buffer.get();
	for (;;) {
		printf("%d: %ws\n", info->TypeIndex, info->TypeName.Buffer);
		if (info->NextEntryOffset == 0)
			break;

		info = (SYSTEM_OBJECTTYPE_INFORMATION*)((BYTE*)info + info->NextEntryOffset);
	}

	return 0;
}

