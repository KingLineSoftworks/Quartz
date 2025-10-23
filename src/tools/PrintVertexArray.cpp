#include <array>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <vector>

using vertex_t = std::array<float, 3>;

std::optional<std::vector<vertex_t>> getVertices(
    const std::string& binaryFilePath,
    const uint32_t dataSizeBytes,
    const uint32_t dataStartOffsetBytes
);
void printVertices(
    const std::vector<vertex_t>& vertices
);

int main(
    int argc,
    char* argv[]
) {
    if (argc != 4) {
        std::cerr << "PrintVertexArray requires 3 command line arguments:" << std::endl;
        std::cerr << "  - the file containing the binary buffer of data (.glb file)" << std::endl;
        std::cerr << "  - the data size (in bytes)" << std::endl;
        std::cerr << "  - the data segment's start offset (in bytes)" << std::endl;
        return 1;
    }

    const std::string binaryFilepath = argv[1];
    const uint32_t dataSizeBytes = std::stoul(argv[2]);
    const uint32_t dataStartOffsetBytes = std::stoul(argv[3]);

    const std::optional<std::vector<vertex_t>> o_vertices = getVertices(binaryFilepath, dataSizeBytes, dataStartOffsetBytes);
    if (!o_vertices) {
        std::cerr << "Unable to read vertices from " << binaryFilepath << " with data size of " << dataSizeBytes << " and a start offset of " << dataStartOffsetBytes << " bytes" << std::endl;
        return 1;
    }
    printVertices(*o_vertices);

    return 0;
}

std::optional<std::vector<vertex_t>>
getVertices(
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

    const uint32_t bytesPerVertex = 4 * 3; // 4 bytes per float, 3 floats per vertex
    if (dataSizeBytes % bytesPerVertex) {
        std::cerr << "Specified data size (" << dataSizeBytes << " bytes) does not evenly fit into 3 float vertices" << std::endl;
        return {};
    }

    const uint32_t vertexCount = dataSizeBytes / bytesPerVertex;
    std::vector<vertex_t> vertices(vertexCount);

    inputFile.seekg(dataStartOffsetBytes, std::ios::beg);
    for (uint32_t iVertex = 0; iVertex < vertexCount; ++iVertex) {
        inputFile.read(reinterpret_cast<char*>(&(vertices[iVertex][0])), 4);
        inputFile.read(reinterpret_cast<char*>(&(vertices[iVertex][1])), 4);
        inputFile.read(reinterpret_cast<char*>(&(vertices[iVertex][2])), 4);
    }
    inputFile.close();

    return vertices;
}

void
printVertices(
    const std::vector<vertex_t>& vertices 
) {
    std::cout << "{\n";

    for (uint32_t iVertex = 0; iVertex < vertices.size(); ++iVertex) {
        const vertex_t& vertex = vertices[iVertex];

        std::cout << "    { ";
        std::cout << std::showpoint << vertex[0] << std::noshowpoint << "f , ";
        std::cout << std::showpoint << vertex[1] << std::noshowpoint << "f , ";
        std::cout << std::showpoint << vertex[2] << std::noshowpoint << "f ";
        std::cout << "}";

        if (iVertex < vertices.size() - 1) {
            std::cout << ",";
        }

        std::cout << "\n";
    }

    std::cout << "}\n";
}
