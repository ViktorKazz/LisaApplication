#ifndef ARCHIVE_CLASS_H
#define ARCHIVE_CLASS_H

#include "ArchiveImpl.h"

namespace LisaApp
{
	class Archive : LArchive::Impl
	{
	public:
		Archive() = default;
		~Archive() = default;

		auto GetFlags() const noexcept { return m_flags; };
		auto GetArchiveInfo() const noexcept { return m_archiveInfo; };

		void PrintArchiveInfo(const std::filesystem::path& archive);

		void Write(
			const std::string& archiveName,
			std::vector<std::string> fileToWrite
		);

		void Read(
			const std::string& archiveName,
			const std::string& firstFlags,
			std::filesystem::path path,
			const std::string& secondFlags,
			std::vector<std::string> fileToRead
		);

	protected:
		LArchive::Flags m_flags;
		std::vector<std::string> m_archiveInfo;
	};

}

#endif // !ARCHIVE_CLASS_H
