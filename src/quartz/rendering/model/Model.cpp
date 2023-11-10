#include <string>
#include <queue>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>

#define TINY0BJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/mesh/Vertex.hpp"
#include "quartz/rendering/model/Model.hpp"

void
quartz::rendering::Model::loadGLTFModel(
    const quartz::rendering::Device& renderingDevice
){
    const std::string filepath = "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/Box/glTF/Box.gltf";
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "{}", filepath);

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model gltfModel;

    std::string warningString;
    std::string errorString;

    const bool isBinaryFile =
        util::FileSystem::getFileExtension(filepath) ==
        "glb";

    LOG_TRACE(MODEL, "Using {} gltf file at {}", isBinaryFile ? "binary" : "ascii", filepath);

    const bool fileLoadedSuccessfully = isBinaryFile ?
        gltfContext.LoadBinaryFromFile(
            &gltfModel,
            &errorString,
            &warningString,
            filepath.c_str()
        ) :
        gltfContext.LoadASCIIFromFile(
            &gltfModel,
            &errorString,
            &warningString,
            filepath.c_str()
        );

    if (!fileLoadedSuccessfully) {
        LOG_CRITICAL(MODEL, "Failed to load model at {}", filepath);
        LOG_CRITICAL(MODEL, "  - Warning : {}", warningString);
        LOG_CRITICAL(MODEL, "  - Error   : {}", errorString);
        throw std::runtime_error("");
    }

    if (!warningString.empty()) {
        LOG_WARNING(
            MODEL, "TinyGLTF::Load{}FromFile warning : {}",
            isBinaryFile ? "Binary" : "ASCII", warningString
        );
    }

    if (!errorString.empty()) {
        LOG_ERROR(
            MODEL, "TinyGLTF::Load{}FromFile error : {}",
            isBinaryFile ? "Binary" : "ASCII", errorString
        );
    }

    // -----===== textures and their samplers =====----- //

    LOG_TRACE(MODEL, "Got {} texture samplers from gltf model", gltfModel.samplers.size());
    LOG_TRACE(MODEL, "Got {} textures from gltf model", gltfModel.textures.size());

    for (uint32_t i = 0; i < gltfModel.textures.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        const tinygltf::Texture& gltfTexture = gltfModel.textures[i];

        LOG_TRACE(MODEL, "Creating texture {} with name \"{}\"", i, gltfTexture.name);

        tinygltf::Sampler gltfSampler;
        const int32_t samplerIndex = gltfTexture.sampler;
        if (samplerIndex == -1) {
            LOG_WARNING(MODEL, "No sampler specified. Using default one");
            gltfSampler.minFilter = -1;
            gltfSampler.magFilter = -1;
            gltfSampler.wrapS = -1;
            gltfSampler.wrapT = -1;
            gltfSampler.wrapT = -1;
        } else {
            gltfSampler = gltfModel.samplers[samplerIndex];
            LOG_TRACE(MODEL, "Using sampler {} with name \"{}\"", samplerIndex, gltfSampler.name);
        }

        const int32_t imageIndex = gltfTexture.source;
        const tinygltf::Image& gltfImage = gltfModel.images[imageIndex];
        LOG_TRACE(MODEL, "Using image {} with name \"{}\"", imageIndex, gltfImage.name);

        UNUSED quartz::rendering::Texture texture(
            renderingDevice,
            gltfImage,
            gltfSampler
        );
    }

    // -----===== materials =====----- //

    /// @todo

    // -----===== meshes =====----- //

    const uint32_t defaultSceneIndex = gltfModel.defaultScene;
    const tinygltf::Scene& gltfScene = gltfModel.scenes[
        defaultSceneIndex > -1 ?
            defaultSceneIndex :
            0
    ];

    // --- Get the vertex and index count from all nodes in the scene --- //

    std::queue<const tinygltf::Node*> gltfNodePtrs;

    LOG_TRACE(MODEL, "Initializing queue of nodes from default scene {}", defaultSceneIndex);
    for (uint32_t i = 0; i < gltfScene.nodes.size(); ++i) {
        const uint32_t currentNodeIndex = gltfScene.nodes[i];
        gltfNodePtrs.push(&gltfModel.nodes[currentNodeIndex]);
    }
    LOG_TRACE(MODEL, "Using {} root nodes", gltfNodePtrs.size());

    uint32_t totalVertexCount = 0;
    uint32_t totalIndexCount = 0;

    LOG_TRACE(MODEL, "Getting vertex and index count");
    while (!gltfNodePtrs.empty()) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);

        const tinygltf::Node* p_gltfNode = gltfNodePtrs.front();
        gltfNodePtrs.pop();
        LOG_TRACE(MODEL, "Got node at {}", static_cast<const void*>(p_gltfNode));

        LOG_TRACE(MODEL, "Adding {} more nodes to the queue", p_gltfNode->children.size());
        for (uint32_t i = 0; i < p_gltfNode->children.size(); ++i) {
            const uint32_t currentNodeIndex = p_gltfNode->children[i];
            gltfNodePtrs.push(&gltfModel.nodes[currentNodeIndex]);
            LOG_TRACE(MODEL, "  - Adding node at {}", static_cast<void*>(&gltfModel.nodes[gltfScene.nodes[currentNodeIndex]]));
        }

        if (p_gltfNode->mesh <= -1) {
            LOG_TRACE(MODEL, "Not considering this node for mesh information");
            continue;
        }

        LOG_TRACE(MODEL, "Using mesh {}", p_gltfNode->mesh);
        const tinygltf::Mesh& gltfMesh = gltfModel.meshes[p_gltfNode->mesh];

        LOG_TRACE(MODEL, "Considering {} primitives", gltfMesh.primitives.size());
        for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i) {
            LOG_TRACE(MODEL, "Primitive {}", i);

            const tinygltf::Primitive& gltfPrimitive = gltfMesh.primitives[i];

            if (gltfPrimitive.attributes.find("POSITION") == gltfPrimitive.attributes.end()) {
                LOG_CRITICAL(MODEL, "Primitive {} does not contain a position attribute", i);
                throw std::runtime_error("");
            }

            const uint32_t vertexCount = gltfModel.accessors[gltfPrimitive.attributes.find("POSITION")->second].count;
            std::vector<quartz::rendering::Vertex> vertices(vertexCount);

            LOG_TRACE(
                MODEL, "  - {} vertices, bumping total vertex count from {} to {}",
                vertexCount, totalVertexCount, totalVertexCount + vertexCount
            );
            totalVertexCount += vertexCount;

            // position

            const tinygltf::Accessor& positionAccessor = gltfModel.accessors[gltfPrimitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView& positionBufferView = gltfModel.bufferViews[positionAccessor.bufferView];
            const float* p_positions = reinterpret_cast<const float*>(&(
                gltfModel.buffers[positionBufferView.buffer].data[positionAccessor.byteOffset + positionBufferView.byteOffset]
            ));
            const int32_t positionByteStride =
                positionAccessor.ByteStride(positionBufferView) ?
                    (positionAccessor.ByteStride(positionBufferView) / sizeof(float)) :
                    tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

            for (uint32_t j = 0; j < vertexCount; ++j) {
                vertices[j].worldPosition = glm::make_vec3(&p_positions[j * positionByteStride]);
            }

            // color

            if (gltfPrimitive.attributes.find("COLOR_0") != gltfPrimitive.attributes.end()) {
                LOG_TRACE(MODEL, "Primitive {} does contain a color attribute", i);

                const tinygltf::Accessor& accessor = gltfModel.accessors[gltfPrimitive.attributes.find("COLOR_0")->second];
                const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
                const float* p_data = reinterpret_cast<const float*>(&(
                    gltfModel.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]
                ));
                const int32_t byteStride =
                    accessor.ByteStride(bufferView) ?
                        accessor.ByteStride(bufferView) / sizeof (float) :
                        tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

                for (uint32_t j = 0; j < vertexCount; ++j) {
                    vertices[j].color = glm::make_vec3(&p_data[j * byteStride]);
                }
            }

            // texture

            if (gltfPrimitive.attributes.find("TEXCOORD_0") != gltfPrimitive.attributes.end()) {
                LOG_TRACE(MODEL, "Primitive {} does contain a texture UV attribute", i);

                const tinygltf::Accessor& accessor = gltfModel.accessors[gltfPrimitive.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
                const float* p_data = reinterpret_cast<const float*>(&(
                    gltfModel.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]
                ));
                const int32_t byteStride =
                    accessor.ByteStride(bufferView) ?
                        accessor.ByteStride(bufferView) / sizeof (float) :
                        tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);

                for (uint32_t j = 0; j < vertexCount; ++j) {
                    vertices[j].textureCoordinate = glm::make_vec2(&p_data[j * byteStride]);
                }
            }

            if (gltfPrimitive.indices <= -1) {
                LOG_TRACE(MODEL, "  - no indices. Not considering for total count");
                continue;
            }

            const uint32_t indexCount = gltfModel.accessors[gltfPrimitive.indices].count;
            std::vector<uint32_t> indices(indexCount);

            LOG_TRACE(
                MODEL, "  - {} indices, bumping total index count from {} to {}",
                indexCount, totalIndexCount, totalIndexCount + indexCount
            );
            totalIndexCount += indexCount;

            const tinygltf::Accessor& indexAccessor = gltfModel.accessors[
                gltfPrimitive.indices > -1 ?
                    gltfPrimitive.indices :
                    0
            ];
            const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& indexBuffer = gltfModel.buffers[indexBufferView.buffer];

            switch (indexAccessor.componentType) {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                    LOG_TRACE(MODEL, "Using indices of type uint32_t");
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                    LOG_TRACE(MODEL, "Using indices of type uint16_t");
                    const uint16_t* p_indices = reinterpret_cast<const uint16_t*>(&(
                        indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]
                    ));
                    for (uint16_t j = 0; j < indexAccessor.count; ++j) {
                        indices[j] = p_indices[j];
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                    LOG_TRACE(MODEL, "Using indices of type uint8_t");
                    break;
                }
            }

            LOG_TRACE(MODEL, "Loaded {} indices", indices.size());
            for (uint32_t j = 0; j < indices.size(); j++) {
                LOG_TRACE(MODEL, "  - {} : {}", j, indices[j]);
            }

            UNUSED quartz::rendering::Mesh mesh(
                renderingDevice,
                vertices,
                indices
            );
        }
    }
}

bool
quartz::rendering::Model::loadModel(
    const std::string& filepath,
    tinyobj::attrib_t& tinyobjAttribute,
    std::vector<tinyobj::shape_t>& tinyobjShapes,
    std::vector<tinyobj::material_t>& tinyobjMaterials
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "{}", filepath);
    // Attributes
    //   contains:
    //    - positions
    //    - normals
    //    - texture coordinates
    //
    // Shapes
    //   contains:
    //    - seperate objects and their faces
    //      - Each face consists of an array of vertices
    //        - Each vertex contains indices of position, normal,
    //          and texture coord attributes
    // Face
    //   contains:
    //    - array of vertices
    //
    // Vertex:
    //   contains:
    //    - position index
    //    - normal index
    //    - texture coordinate index

    std::string warningString;
    std::string errorString;

    // Triangulation feature enabled by default, so we can rest assured
    // that every face is going to be a triangle
    if(!tinyobj::LoadObj(
        &tinyobjAttribute,
        &tinyobjShapes,
        &tinyobjMaterials,
        &warningString,
        &errorString,
        filepath.c_str()
    )) {
        LOG_CRITICAL(MODEL, "Failed to load model at {}", filepath);
        LOG_CRITICAL(MODEL, "  - Warning : {}", warningString);
        LOG_CRITICAL(MODEL, "  - Error   : {}", errorString);
        throw std::runtime_error("");
    }

    if (!warningString.empty()) {
        LOG_WARNING(MODEL, "tinyobj::load warning : {}", warningString);
    }

    if (!errorString.empty()) {
        LOG_ERROR(MODEL, "tinyobj::load error : {}", errorString);
    }

    uint32_t totalShapeIndices = 0;
    for (const tinyobj::shape_t& shape : tinyobjShapes) {
        totalShapeIndices += shape.mesh.indices.size();
    }

    LOG_DEBUG(MODEL, "Loaded model at {}", filepath);
    LOG_DEBUG(MODEL, "  - {} attribute vertices", tinyobjAttribute.vertices.size());
    LOG_DEBUG(MODEL, "  - {} attribute normals", tinyobjAttribute.normals.size());
    LOG_DEBUG(MODEL, "  - {} attribute texture coordinates", tinyobjAttribute.texcoords.size());
    LOG_DEBUG(MODEL, "  - {} shapes", tinyobjShapes.size());
    LOG_DEBUG(MODEL, "    - {} total indices", totalShapeIndices);
    LOG_DEBUG(MODEL, "  - {} materials", tinyobjMaterials.size());

    return true;
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    UNUSED const std::string& objectFilepath,
    const std::string& textureFilepath
) :
    m_tinyobjAttribute(),
    m_tinyobjShapes(),
    m_tinyobjMaterials(),
    m_loadedSuccessfully(
        quartz::rendering::Model::loadModel(
            objectFilepath,
            m_tinyobjAttribute,
            m_tinyobjShapes,
            m_tinyobjMaterials
        )
    ),
    m_mesh(
        renderingDevice,
        m_tinyobjAttribute,
        m_tinyobjShapes
    ),
    m_texture(renderingDevice, textureFilepath)
{
    LOG_FUNCTION_CALL_TRACEthis("");
    quartz::rendering::Model::loadGLTFModel(renderingDevice);
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_tinyobjAttribute(other.m_tinyobjAttribute),
    m_tinyobjShapes(other.m_tinyobjShapes),
    m_tinyobjMaterials(other.m_tinyobjMaterials),
    m_loadedSuccessfully(other.m_loadedSuccessfully),
    m_mesh(std::move(other.m_mesh)),
    m_texture(std::move(other.m_texture))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}