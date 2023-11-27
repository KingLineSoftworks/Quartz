#pragma once

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/mesh/Vertex.hpp"

namespace quartz {
namespace rendering {
    class Primitive;
}
}

class quartz::rendering::Primitive {
public: // member functions
    Primitive(
        const std::vector<uint32_t>& indices
    );
    Primitive(const Primitive& other);
    Primitive(Primitive&& other);
    ~Primitive();

    USE_LOGGER(MESH);

    const std::vector<uint32_t> getIndices() const { return m_indices; }

private: // static functions

private: // member variables

    /**
     * @todo 2023/11/21 Should contain vertices and corresponding indices as well as
     *   a (weak) pointer to the material it should be using
     */

    /**
     * @todo 2023/11/21 Should contain the indices that it is using. The actual vertices
     *   should be contained within the model itself
     */

    std::vector<uint32_t> m_indices;
};