#ifndef ARCHIVE_IMPL_CLASS_H
#define ARCHIVE_IMPL_CLASS_H

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <queue>
#include <unordered_map>
#include <vector>
#include <functional>

#include "ArchiveUtil.h"

namespace LArchive
{
	struct Flags
	{
		std::string CreatePath = "-cp";
		std::string AllFiles = "-af";
	};

	// A Huffman tree node
	struct NodePtr {

		NodePtr(char ch, std::uint32_t frequency) :
			m_ch{ ch },
			m_frequency{ frequency },
			m_left{ nullptr },
			m_right{ nullptr }
		{
		}
		NodePtr(
			char ch,
			std::uint32_t freq,
			std::shared_ptr<NodePtr> left,
			std::shared_ptr<NodePtr> right
		) :
			m_ch(ch),
			m_frequency(freq),
			m_left(left),
			m_right(right)
		{
		}

		char m_ch;
		std::uint32_t m_frequency;
		std::shared_ptr<NodePtr> m_left;
		std::shared_ptr<NodePtr> m_right;
	};

	class Impl : public ArchiveUtil
	{
	public:
		Impl() = default;
		~Impl() = default;

		// Define a comparator for priority_queue
		struct Compare
		{
			bool operator()(const std::shared_ptr<NodePtr>& a, const std::shared_ptr<NodePtr>& b) const
			{
				if (a == nullptr || b == nullptr)
				{
					throw std::runtime_error("Null pointer in priority queue");
				}

				return a->m_frequency > b->m_frequency;
			}
		};

		void GetData(const std::string& fileToWrite);
		void Frequency();
		void BuildHuffmanTree();
		void HuffmanTreeTraversal(const std::shared_ptr<NodePtr>& root, const std::string& str);
		std::string StringOfBinaryCharacters();

		std::string DecimalToBinary(size_t decimal);
		std::pair<std::vector<std::string>, std::uint8_t> EightBitChunks(const std::string& stringBinary);

		void Serialize(std::ofstream& out);
		void RecordingInTheArchive(
			const std::string& stringBinary,
			const std::string& archiveName,
			const std::string& fileToWrite
		);

		void ReadingDataFromArchive(
			const std::string& archiveName,
			std::vector<std::string> fileToRead,
			std::filesystem::path inPath,
			const std::string& firstFlag,
			const std::string& secondFlags
		);
		void Deserialize(const std::string& stringBinary);

		std::string BinaryString(
			const std::vector<std::uint32_t>& binaryBuffer,
			std::uint8_t zero
		);
		std::string Decode(const std::shared_ptr<NodePtr>& root, const std::string& stringBinary);

	private:
		std::unordered_map<char, std::uint8_t> m_frequency;
		std::unordered_map<char, std::string> m_huffmanCode;

		std::string m_inputText;

		std::shared_ptr<NodePtr> m_root{};
	};

}

#endif // !ARCHIVE_IMPL_CLASS_H