#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <vector>

using index_t = uint16_t;

std::optional<std::vector<index_t>> getIndices(
    const std::string& binaryFilePath,
    const uint32_t dataSizeBytes,
    const uint32_t dataStartOffsetBytes
);
void printIndices(
    const std::vector<index_t>& indices 
);

int main(
    int argc,
    char* argv[]
) {
    if (argc != 4) {
        std::cerr << "PrintIndexArray requires 3 command line arguments:" << std::endl;
        std::cerr << "  - the file containing the binary buffer of data (.glb file)" << std::endl;
        std::cerr << "  - the data size (in bytes)" << std::endl;
        std::cerr << "  - the data segment's start offset (in bytes)" << std::endl;
        return 1;
    }

    const std::string binaryFilepath = argv[1];
    const uint32_t dataSizeBytes = std::stoul(argv[2]);
    const uint32_t dataStartOffsetBytes = std::stoul(argv[3]);

    std::optional<std::vector<index_t>> o_indices = getIndices(binaryFilepath, dataSizeBytes, dataStartOffsetBytes);
    if (!o_indices) {
        std::cerr << "Unable to read indices from " << binaryFilepath << " with data size of " << dataSizeBytes << " and a start offset of " << dataStartOffsetBytes << " bytes" << std::endl;
        return 1;
    }
    printIndices(*o_indices);

    return 0;
}

std::optional<std::vector<index_t>>
getIndices(
    const std::string& binaryFilePath,
    const uint32_t dataSizeBytes,
    const uint32_t dataStartOffsetBytes
) {
    std::ifstream inputFile(binaryFilePath, std::ios::ate | std::ios::binary);
    if (!inputFile) {
        std::cerr << "Unable to open file at " << binaryFilePath << std::endl;
        return {};
    }

    const uint32_t fileSizeBytes = inputFile.tellg();
    if (fileSizeBytes < dataSizeBytes + dataStartOffsetBytes) {
        std::cerr << binaryFilePath << " only contains " << fileSizeBytes << ", which is insufficient for data of size " << dataSizeBytes << " bytes with a start offset of " << dataStartOffsetBytes << " bytes" << std::endl;
        return {};
    }

    const uint32_t bytesPerFace = sizeof(index_t) * 3; // N bytes per uintXX_t, 3 uintXX_t per face's indices
    if (dataSizeBytes % bytesPerFace) {
        std::cerr << "Specified data size (" << dataSizeBytes << " bytes) does not evenly fit into 3 uint32_t indices" << std::endl;
        return {};
    }

    const uint32_t indexCount = dataSizeBytes / sizeof(index_t);
    std::vector<index_t> indices(indexCount);

    inputFile.seekg(dataStartOffsetBytes, std::ios::beg);
    for (uint32_t iIndex = 0; iIndex < indexCount; ++iIndex) {
        inputFile.read(reinterpret_cast<char*>(&(indices[iIndex])), sizeof(index_t));
    }
    inputFile.close();

    return indices;
}

void
printIndices(
    const std::vector<index_t>& indices 
) {
    std::cout << "{\n";

    for (uint32_t iStartIndex = 0; iStartIndex < indices.size() - 2; iStartIndex += 3) {
        const uint32_t& index0 = indices[iStartIndex+0];
        const uint32_t& index1 = indices[iStartIndex+1];
        const uint32_t& index2 = indices[iStartIndex+2];

        std::cout << "    " << index0 << ", " << index1 << ", " << index2;

        if (iStartIndex < indices.size() - 3) {
            std::cout << ",";
        }

        std::cout << "\n";
    }

    std::cout << "}\n";
}

