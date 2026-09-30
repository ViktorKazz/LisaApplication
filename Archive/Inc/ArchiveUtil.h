#ifndef ARCHIVE_UTIL_CLASS_H
#define ARCHIVE_UTIL_CLASS_H

#include <vector>
#include <filesystem>
#include <fstream>

namespace LArchive
{
	class ArchiveUtil
	{
	public:
		ArchiveUtil() = default;
		~ArchiveUtil() = default;

		bool CheckingFileName(std::vector<std::string> fileNames, const std::string& compare);

		bool IsExists(
			const std::filesystem::path& p,
			std::filesystem::file_status s = std::filesystem::file_status{});

		std::filesystem::path CreateDirectory(
			const std::string& fileName,
			const std::string& newFolder,
			const std::filesystem::path& path
		);

		void WriteDataToFile(const std::string& data, const std::filesystem::path& file);

	private:

	};
}

#endif // !ARCHIVE_UTIL_CLASS_H
