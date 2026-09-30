#include "ArchiveImpl.h"
#include <cassert>

void LArchive::Impl::GetData(const std::string& fileToWrite)
{
	std::fstream input;
	m_inputText.clear();

	input.open(fileToWrite, std::ios::in);
	while (!input.eof())
	{
		std::string temp;
		std::getline(input, temp);
		if (!input.eof())
		{
			temp += '\n';
		}
		m_inputText += temp;
	}
	input.close();
}

// Getting an array of character usage frequencies
void LArchive::Impl::Frequency()
{
	m_frequency.clear();

	for (const auto& i : m_inputText)
		m_frequency[i]++;
}

// Function to build the Huffman Tree and generate Huffman Codes
void LArchive::Impl::BuildHuffmanTree()
{
	// Create a priority queue to store live nodes of the
	// Huffman tree.
	std::priority_queue<std::shared_ptr<NodePtr>, std::vector<std::shared_ptr<NodePtr>>, Compare> minHeap;

	if (m_frequency.empty())
		throw std::runtime_error("Frequency map is empty");

	// Creating nodes using unique_ptr
	for (const auto& pair : m_frequency)
		minHeap.emplace(std::make_shared<NodePtr>(pair.first, pair.second));

	// Building a tree.
	while (minHeap.size() > 1)
	{
		std::shared_ptr<NodePtr> left = minHeap.top();
		minHeap.pop();

		std::shared_ptr<NodePtr> right = minHeap.top();
		minHeap.pop();

		// Create a parent node.
		std::uint32_t sum = left->m_frequency + right->m_frequency;
		minHeap.emplace(std::make_shared<NodePtr>('\0', sum, left, right));
	}

	m_root = minHeap.top();
}

// Traverse the Huffman Tree and store Huffman Codes in a map
void LArchive::Impl::HuffmanTreeTraversal(const std::shared_ptr<NodePtr>& root, const std::string& str)
{
	if (root == nullptr)
		return;

	// Found a leaf node
	if (!root->m_left && !root->m_right)
	{
		m_huffmanCode[root->m_ch] = str;
	}

	HuffmanTreeTraversal(root->m_left, str + "0");
	HuffmanTreeTraversal(root->m_right, str + "1");
}

// Create a single string from binary characters.
std::string LArchive::Impl::StringOfBinaryCharacters()
{
	std::string str{};
	m_huffmanCode.clear();

	HuffmanTreeTraversal(m_root, "");

	// Binary code merger
	for (char ch : m_inputText)
	{
		str += m_huffmanCode[ch];
	}

	return str;
}

std::string LArchive::Impl::DecimalToBinary(size_t decimal)
{
	std::string binary = "";

	if (decimal == 0)
		binary = "0";

	while (decimal > 0)
	{
		binary = (decimal % 2 == 0 ? "0" : "1") + binary;
		decimal = decimal / 2;
	}

	return binary;
}

// Splitting a string of binary characters into eight-bit chunks.
std::pair<std::vector<std::string>, std::uint8_t> LArchive::Impl::EightBitChunks(const std::string& stringBinary)
{
	// Preparing to write coded text.
	std::string buffer{};
	std::vector<std::string> storingBinary;

	// Split a binary string into eight-character chunks.
	// Eight characters is the number of bits in one byte.

	size_t eightBit{ 8 };

	for (const auto& i : stringBinary)
	{
		if (buffer.size() < eightBit)
		{
			buffer += i;

			if (buffer.size() == eightBit)
			{
				storingBinary.push_back(buffer);
				buffer.clear();
			}
		}
	}

	// If the remainder is not equal to eight.
	if (!buffer.empty())
	{
		storingBinary.push_back(buffer);
		buffer.clear();
	}


	std::uint8_t numberOfZeros{ 0 };

	// Count the number of zeros at the beginning of the last row of bits.
	// 001001101 -> two zeros
	for (const auto& c : storingBinary.back())
	{
		if (c == '0')
		{
			numberOfZeros++;
		}
		else if (c == '1')
		{
			break;
		}
	}

	return { storingBinary, numberOfZeros };
}

void LArchive::Impl::Serialize(std::ofstream& out)
{
	std::string stringBinary;
	std::function<void(const std::shared_ptr<NodePtr>&)> l;

	l = [&](const std::shared_ptr<NodePtr>& r)
		{
			if (r == nullptr)
				return;

			std::string bits = DecimalToBinary(r->m_frequency);
			std::string nBits = DecimalToBinary(bits.size());

			// The first block is six bits. 
			// It is allocated to indicate the frequency size in bits.
			for (size_t i = 0; i < 6 - nBits.size(); i++)
				stringBinary += "0";

			stringBinary += nBits;


			if (r->m_ch == '\0')
			{
				// The second block is 1 bit in size. If there is no symbol then 0.
				stringBinary += DecimalToBinary(0);
			}
			else if (r->m_ch != '\0')
			{
				// If there is a symbol then 1.
				stringBinary += DecimalToBinary(1);
				// How many bits does a symbol have?
				nBits = DecimalToBinary(r->m_ch);

				// Allocate 8 bits for the symbol.
				for (size_t i = 0; i < 8 - nBits.size(); i++)
					stringBinary += "0";

				stringBinary += nBits;
			}
			// Write the frequency in bits.
			stringBinary += DecimalToBinary(r->m_frequency);

			// Internal node, recursively serialize children
			l(r->m_left);
			l(r->m_right);
		};

	l(m_root);

	std::pair<std::vector<std::string>, std::uint8_t> sb = EightBitChunks(stringBinary);

	// The container size is essentially the number of bytes occupied by the tree.
	std::uint32_t bytes = static_cast<std::uint32_t>(sb.first.size());

	// Write it down in the archive.
	out.write(reinterpret_cast<char*>(&bytes), sizeof(bytes));


	// Divide the string into pieces of eight. 
	// The last piece may contain zeros at the beginning of the string. 
	// When converting to a decimal value and writing to a file, 
	// these zeros will disappear. 
	// In order to correctly recreate the string when reading, 
	// you need to write down the number of zeros.
	out.write(reinterpret_cast<char*>(&sb.second), sizeof(sb.second));

	// Convert from strings with binary value to decimal and write to file.
	for (const auto& i : sb.first)
	{
		std::uint32_t toDecimal = std::stoi(i, nullptr, 2);

		std::byte byte = { static_cast<std::byte>(toDecimal) };
		out.write(reinterpret_cast<char*>(&byte), sizeof(byte));
	}
}

// Recording coded data into an archive.
void LArchive::Impl::RecordingInTheArchive(
	const std::string& stringBinary,
	const std::string& archiveName,
	const std::string& fileToWrite
)
{
	// Check if an archive exists when we want to add files to it.
	bool isEmpty{ true };
	if (IsExists(archiveName))
		isEmpty = (std::filesystem::file_size(archiveName) == 0);


	std::ofstream fout{};

	if (isEmpty)
		fout.open(archiveName, std::ios::out | std::ios::binary);
	else
		fout.open(archiveName, std::ofstream::app | std::ios::out | std::ios::binary);


	if (!fout.is_open())
	{
		std::cout << "Error opening file" << std::endl;
	}
	else
	{
		// Recording data about files located in the archive.

		// Get the file name with the extension.
		std::filesystem::path file{ fileToWrite };

		// Write down the file name.
		for (const auto& i : file.filename().string())
		{
			std::uint8_t u{};
			// Hide the file name a little.
			// 255 is the maximum value for 1 byte.
			if (i < 255)
				u = i + '\1';
			else
				u = i;

			fout.write(reinterpret_cast<char*>(&u), sizeof(u));
		}
		fout << '\0';


		std::pair<std::vector<std::string>, std::uint8_t> sb = EightBitChunks(stringBinary);

		// The container size is essentially the number of bytes occupied by the encode data.
		std::uint32_t bytes = static_cast<std::uint32_t>(sb.first.size());

		// Write it down in the archive.
		fout.write(reinterpret_cast<char*>(&bytes), sizeof(bytes));


		// Divide the string into pieces of eight. 
		// The last piece may contain zeros at the beginning of the string. 
		// When converting to a decimal value and writing to a file, 
		// these zeros will disappear. 
		// In order to correctly recreate the string when reading, 
		// you need to write down the number of zeros.
		fout.write(reinterpret_cast<char*>(&sb.second), sizeof(sb.second));

		// Write the Huffman tree to a file.
		Serialize(fout);

		// Write encoded data.

		// Convert from strings with binary value to decimal and write to file.
		for (const auto& i : sb.first)
		{
			std::uint32_t toDecimal = std::stoi(i, nullptr, 2);

			std::byte byte = { static_cast<std::byte>(toDecimal) };
			fout.write(reinterpret_cast<char*>(&byte), sizeof(byte));
		}
	}
	fout.close();
}

void LArchive::Impl::Deserialize(const std::string& stringBinary)
{
	std::uint32_t end{};
	std::uint32_t iter{};

	std::function<std::shared_ptr<NodePtr>(std::string)> l;

	// Bit order for Huffman tree.
	// |_6bit__Node frequency in bits__|_0bit or 1bit_Is there a symbol?_|_if 1_8bit_Symbol in bits__|_Frequency_|
	//              |                                   |______________________________^                   ^
	//              |                                                                                      |
	//              |______________________________________________________________________________________|
	//
	// 1. 6 bit. Here we specify the frequency size in bits. For example, 
	// frequency 7 is 111 in bits, so we write 3 that in bits is 11. The last block will be 3 bits.
	// 2. 1 bit. 0 - no symbol, 1 - there is a symbol. Also the last node or not.
	// 3. 8 bit. If there is a symbol, 8 bits are allocated for it.
	// 4. As many bits as specified in the first block.

	l = [&](std::string sb) -> std::shared_ptr<NodePtr>
		{
			if (end > 0)
			{
				end--;
				return nullptr;
			}

			std::string temp;

			// The first six bits indicate how many bits the node frequency has.
			for (size_t i = 0; i < 6; i++)
				temp += sb[iter + i];

			iter += 6;

			std::uint32_t lengthBits = std::stoi(temp, nullptr, 2);


			temp = sb[iter];
			iter++;

			char ch{};
			bool isSymbol = std::stoi(temp, nullptr, 2);

			if (isSymbol)
			{
				temp = "";

				// Eight bits are allocated for the symbol.
				for (size_t i = 0; i < 8; i++)
					temp += sb[iter + i];

				ch = static_cast<char>(std::stoi(temp, nullptr, 2));

				iter += 8;
				// The node with the symbol is usually the last one.
				// Let's assign the variable a value of two, 
				// since the last node has empty left and right nodes.
				end = 2;
			}
			else
			{
				ch = '\0';
			}

			temp = "";

			// Determine the frequency of the node.
			for (size_t i = 0; i < lengthBits; i++)
				temp += sb[iter + i];

			iter += lengthBits;


			std::uint32_t fr = std::stoi(temp, nullptr, 2);

			std::shared_ptr<NodePtr> node = std::make_shared<NodePtr>(ch, fr);

			node->m_left = l(sb);
			node->m_right = l(sb);

			return node;
		};

	m_root = l(stringBinary);
}

// Reading data from archive
void LArchive::Impl::ReadingDataFromArchive(
	const std::string& archiveName,
	std::vector<std::string> fileToRead,
	std::filesystem::path inPath,
	const std::string& firstFlag,
	const std::string& secondFlags
)
{
	std::ifstream fin{ archiveName, std::ios::in | std::ios::binary };

	std::filesystem::path path = archiveName;
	std::uint64_t fileSize{ std::filesystem::file_size(path) };

	if (!fin.is_open())
	{
		std::cout << "Error opening file" << std::endl;
	}
	else
	{
		Flags fl;
		std::uint64_t transition{};
		bool available{};
		std::filesystem::path file;

		if (fileToRead.size() == 0 && secondFlags == fl.AllFiles)
			available = true;

		// Reading information about the file.
		size_t i{};
		while (true)
		{
			// If the carriage has reached the end of the file.
			if (static_cast<std::uint64_t>(fin.tellg()) == fileSize)
				break;

			if (fileToRead.size() != 0)
				if (i == fileToRead.size())
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

			// Read encoded size

			fin.read(reinterpret_cast<char*>(&codedSize), sizeof(codedSize));
			fin.read(reinterpret_cast<char*>(&nZeroCode), sizeof(nZeroCode));

			// Read tree size

			fin.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));
			fin.read(reinterpret_cast<char*>(&nZeroTree), sizeof(nZeroTree));


			if (CheckingFileName(fileToRead, fileName) || available)
			{
				std::vector<std::uint32_t> binaryBuffer;
				std::byte bt{};
				size_t n{};

				// Read tree

				while (n < treeSize)
				{
					fin.read(reinterpret_cast<char*>(&bt), sizeof(bt));
					binaryBuffer.push_back(std::to_integer<std::uint32_t>(bt));

					n++;
				}

				// Recreate a string of binary characters.
				std::string stringBinary = BinaryString(binaryBuffer, nZeroTree);

				// Reconstructing the Huffman tree.
				Deserialize(stringBinary);


				// Reading encoded data.
				binaryBuffer.clear();
				n = 0;
				while (n < codedSize)
				{
					fin.read(reinterpret_cast<char*>(&bt), sizeof(bt));
					binaryBuffer.push_back(std::to_integer<std::uint32_t>(bt));

					n++;
				}

				// Recreate a string of binary characters.
				stringBinary = BinaryString(binaryBuffer, nZeroCode);

				std::string dec = Decode(m_root, stringBinary);

				if (firstFlag == fl.CreatePath)
				{
					// Create the directory only once when unpacking several files.
					if (i == 0)
					{
						file = CreateDirectory(fileName, archiveName, inPath);
					}
					else
					{
						std::filesystem::path current{ file.parent_path() };
						file = current.append(fileName);
					}
				}
				else
				{
					bool exist{};
					size_t j{};
					std::string temp;

					file = fileName;
					// Returns the stem path component (filename without the final extension)
					std::filesystem::path fileNameOnly{ file.stem() };
					std::filesystem::path extensionOnly{ file.extension() };

					while (true)
					{
						temp = fileNameOnly.string();

						// If there is a similar file, then the new number is n+1.
						if (exist)
						{
							temp.append(" ");
							temp.append("(");
							temp.append(std::to_string(j));
							temp.append(")");
							temp.append(extensionOnly.string());
							file = temp;
						}

						exist = IsExists(file);

						if (!exist) { break; }
						j++;
					}
				}

				WriteDataToFile(dec, file);

				transition += fileName.size()
					+ sizeof('\0')
					+ sizeof(codedSize)
					+ sizeof(nZeroCode)
					+ sizeof(treeSize)
					+ sizeof(nZeroTree)
					+ codedSize
					+ treeSize;

				i++;
			}
			else
			{
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
			}
		}

		if (fileToRead.size() != 0 && i < fileToRead.size())
		{
			std::cout << "One or more files were not found in the archive!" << std::endl;
		}
	}
	fin.close();
}

// Reconstructing a string from binary characters.
std::string LArchive::Impl::BinaryString(
	const std::vector<std::uint32_t>& binaryBuffer,
	std::uint8_t zero
)
{
	std::string str{};
	size_t eightBit{ 8 };

	for (size_t i{ 0 }; const auto& col : binaryBuffer)
	{
		std::string d{};
		std::string dtb{};

		dtb = DecimalToBinary(col);

		if (i == binaryBuffer.size() - 1)
		{
			for (size_t j = 0; j < zero; j++)
			{
				d += "0";
			}

			d += dtb;
		}
		else if (i < binaryBuffer.size() - 1)
		{
			size_t z = eightBit - dtb.size();

			for (size_t k = 0; k < z; k++)
			{
				d += "0";
			}

			d += dtb;
		}
		i++;
		str += d;
	}

	return str;
}

// Function to decode a given Huffman encoded string
std::string LArchive::Impl::Decode(const std::shared_ptr<NodePtr>& root, const std::string& stringBinary)
{
	std::string str;

	std::shared_ptr<NodePtr> curr = root;
	for (char bit : stringBinary)
	{
		if (bit == '0')
		{
			curr = curr->m_left;
		}
		else
		{
			curr = curr->m_right;
		}

		// Reached a leaf node
		if (curr->m_left != nullptr && curr->m_right != nullptr)
		{

		}
		else
		{
			str += curr->m_ch;
			curr = root;
		}
	}

	return str;
};