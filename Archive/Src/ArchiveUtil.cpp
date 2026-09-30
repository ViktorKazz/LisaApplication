#include "ArchiveUtil.h"

#include <iostream>

bool LArchive::ArchiveUtil::CheckingFileName(std::vector<std::string> fileNames, const std::string& compare)
{
	for (const auto& i : fileNames)
	{
		if (i == compare)
		{
			return true;
		}
	}

	return false;
}

bool LArchive::ArchiveUtil::IsExists(
	const std::filesystem::path& p,
	std::filesystem::file_status s
)
{
	if (std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(p))
		return true;    //exists
	else
		return false;   //does not exist
}

std::filesystem::path LArchive::ArchiveUtil::CreateDirectory(
	const std::string& fileName,
	const std::string& newFolder,
	const std::filesystem::path& path
)
{
	std::filesystem::path file{ fileName };
	std::filesystem::path archiveName{ newFolder };
	std::filesystem::path archiveNameOnly{ archiveName.stem() };
	std::filesystem::file_status s = std::filesystem::file_status{};

	std::filesystem::path p;
	std::filesystem::path t{ path };

	if (t.empty())
		p = archiveNameOnly.string();

	if (!t.empty())
	{
		t.append(archiveNameOnly.string());
		p = t.string();
	}


	bool exist{};
	size_t i{};
	std::string temp;

	while (true)
	{
		temp = p.string();

		// If there is a similar folder, then add the number n+1 to the new one.
		if (exist)
		{
			temp.append(" ");
			temp.append("(");
			temp.append(std::to_string(i));
			temp.append(")");
		}

		exist = std::filesystem::status_known(s) ? std::filesystem::exists(s) : std::filesystem::exists(temp);

		if (!exist) { break; }
		i++;
	}

	std::filesystem::create_directory(temp);
	std::filesystem::path current{ std::filesystem::absolute(temp) };

	file = current.append(file.string());

	return file;
}

void LArchive::ArchiveUtil::WriteDataToFile(const std::string& data, const std::filesystem::path& file)
{
	std::fstream fout{};
	fout.open(file, std::ios::out | std::ios::binary);

	if (!fout.is_open())
	{
		std::cout << "Error opening file" << std::endl;
	}
	else
	{
		for (const auto& i : data)
		{
			fout.write(reinterpret_cast<const char*>(&i), sizeof(i));
		}
	}
	fout.close();
}
