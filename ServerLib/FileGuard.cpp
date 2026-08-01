#include "LibraryPch.h"
#include "FileGuard.h"

FileGuard::FileGuard(const WCHAR* fileName, const WCHAR* mode)
{
	FILE* file;

	m_err = _wfopen_s(&file, fileName, mode);

	if (nullptr == file || 0 != m_err)
	{
		m_pFile = nullptr;
	}
	else
	{
		m_pFile = file;
	}
}

FileGuard::~FileGuard()
{
	if (IsOpen())
		fclose(m_pFile);
}
