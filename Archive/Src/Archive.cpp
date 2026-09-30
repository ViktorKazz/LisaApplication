#include "Archive.h"

void LisaApp::Archive::PrintArchiveInfo(const std::filesystem::path& archive)
{
	std::filesystem::path path{ archive };
	std::filesystem::path current{ std::filesystem::absolute(path) };

	if (!IsExists(current))
	{
		std::cout << "File " << current.filename() << " not found." << std::endl;
		std::cout << "The file name or file path is invalid or does not exist.." << std::endl;

		return;
	}
	std::uint64_t fileSize{ std::filesystem::file_size(path) };


	std::ifstream fin{ archive, std::ios::in | std::ios::binary };

	if (!fin.is_open())
	{
		std::cout << "Error opening file" << std::endl;
	}
	else
	{
		std::uint64_t transition{};

		// Reading information about the file.
		while (true)
		{
			// I'll leave this here. 
			// If for some reason the condition for exiting the loop below doesn't work :).
			// 
			// If the carriage has reached the end of the file.
			if (static_cast<std::uint64_t>(fin.tellg()) == fileSize)
				break;

			std::string fileName{};
			std::uint32_t codedSize{};
			std::uint8_t nZeroCode{};

			std::uint32_t treeSize{};
			std::uint8_t nZeroTree{};

			// Read the file name.
			while (true)
			{
				char t{};

				std::uint8_t n{};
				fin.read(reinterpret_cast<char*>(&n), sizeof(n));

				if (n == '\0')
					break;
				// Will restore the symbols that were hidden.
				// 255 is the maximum value for 1 byte.
				if (n < 255)
					n -= '\1';

				t = n;
				fileName += t;
			}

			// Place the file names in a container.
			m_archiveInfo.push_back(fileName);

			// Read encoded size
			fin.read(reinterpret_cast<char*>(&codedSize), sizeof(codedSize));
			fin.read(reinterpret_cast<char*>(&nZeroCode), sizeof(nZeroCode));

			// Read tree size
			fin.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));
			fin.read(reinterpret_cast<char*>(&nZeroTree), sizeof(nZeroTree));

			// Move on to the next file in the archive.
			transition += fileName.size()
				+ sizeof('\0')
				+ sizeof(codedSize)
				+ sizeof(nZeroCode)
				+ sizeof(treeSize)
				+ sizeof(nZeroTree)
				+ codedSize
				+ treeSize;

			fin.clear();
			fin.seekg(transition, fin.beg);

			// Display information about the file in the console.
			std::cout
				<< fileName
				<< " | file size in archive: "
				<< codedSize
				<< " bytes | "
				<< "size of additional data: "
				<< treeSize
				<< " bytes"
				<< std::endl;

			// Don't wait until the carriage reaches the end of the file.
			if (transition == fileSize)
				break;
		}
	}
	fin.close();
}

void LisaApp::Archive::Write(const std::string& archiveName, std::vector<std::string> fileToWrite)
{
	for (const auto& i : fileToWrite)
	{
		std::filesystem::path current{ std::filesystem::absolute(i) };

		if (!IsExists(current))
		{
			std::cout << "File " << current.filename() << " not found." << std::endl;
			std::cout << "The file name or file path is invalid or does not exist.." << std::endl;
		}
		else if (IsExists(current))
		{
			GetData(i);

			Frequency();

			BuildHuffmanTree();

			std::string stringBinary = StringOfBinaryCharacters();

			RecordingInTheArchive(stringBinary, archiveName, i);
		}
	}
}

void LisaApp::Archive::Read(
	const std::string& archiveName,
	const std::string& firstFlags,
	std::filesystem::path path,
	const std::string& secondFlags,
	std::vector<std::string> fileToRead
)
{
	if (!IsExists(archiveName))
	{
		std::cout << "Archive with this name : " << archiveName << " not found!" << std::endl;
		return;
	}

	if (!path.empty() && !IsExists(path))
	{
		std::cout << "The specified path does not exist!" << std::endl;
		return;
	}

	ReadingDataFromArchive(archiveName, fileToRead, path, firstFlags, secondFlags);
}