#pragma once

class FileGuard
{
public:
	explicit FileGuard(const WCHAR* fileName, const WCHAR* mode);

	~FileGuard();

	FileGuard(const FileGuard&) = delete;
	FileGuard& operator=(const FileGuard&) = delete;

	bool IsOpen() const { return nullptr != m_pFile; }
	FILE* Get() const { return m_pFile; }


private:
	errno_t m_err;
	FILE* m_pFile;
};

