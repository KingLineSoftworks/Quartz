#include <string>
#include <queue>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/mesh/Vertex.hpp"
#include "quartz/rendering/model/Model.hpp"

tinygltf::Model
quartz::rendering::Model::loadGLTFModel(
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "{}", filepath);

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model gltfModel;

    std::string warningString;
    std::string errorString;

    const bool isBinaryFile =
        util::FileSystem::getFileExtension(filepath) ==
        "glb";

    LOG_TRACE(MODEL, "Using {} gltf file at {}", isBinaryFile ? "binary" : "ascii", filepath);

    const bool fileLoadedSuccessfully =
        isBinaryFile ?
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

    return gltfModel;
}

const tinygltf::Scene&
quartz::rendering::Model::loadDefaultScene(
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    int32_t defaultSceneIndex = gltfModel.defaultScene;

    LOG_TRACE(MODEL, "Default scene index is {}", defaultSceneIndex);
    if (defaultSceneIndex <= -1) {
        LOG_TRACE(MODEL, "Default scene index is less than -1");
        LOG_TRACE(MODEL, "Using default scene index of 0 instead");
        defaultSceneIndex = 0;
    }

    const tinygltf::Scene& gltfScene = gltfModel.scenes[defaultSceneIndex];

    return gltfScene;
}

std::queue<const tinygltf::Node*>
quartz::rendering::Model::loadNodePtrQueue(
    const tinygltf::Model& gltfModel,
    const tinygltf::Scene& gltfScene
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::queue<const tinygltf::Node*> workingQueue;
    std::queue<const tinygltf::Node*> masterQueue;

    LOG_TRACE(MODEL, "Initializing queue of nodes from default scene");

    for (uint32_t i = 0; i < gltfScene.nodes.size(); ++i) {
        const uint32_t currentNodeIndex = gltfScene.nodes[i];
        const tinygltf::Node* p_currentNode = &gltfModel.nodes[currentNodeIndex];

        workingQueue.push(p_currentNode);
        masterQueue.push(p_currentNode);
    }

    LOG_TRACE(MODEL, "Using {} root nodes", workingQueue.size());

    while (!workingQueue.empty()) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);

        const tinygltf::Node* p_gltfNode = workingQueue.front();
        workingQueue.pop();
        LOG_TRACE(MODEL, "Got node at {}", static_cast<const void*>(p_gltfNode));

        LOG_TRACE(MODEL, "Adding {} more nodes to the queue", p_gltfNode->children.size());

        for (uint32_t i = 0; i < p_gltfNode->children.size(); ++i) {
            const uint32_t currentNodeIndex = p_gltfNode->children[i];
            const tinygltf::Node* p_currentNode = &gltfModel.nodes[currentNodeIndex];

            workingQueue.push(p_currentNode);
            masterQueue.push(p_currentNode);
        }
    }

    return masterQueue;
}

void
quartz::rendering::Model::populateVerticesWithAttribute(
    std::vector<quartz::rendering::Vertex>& verticesToPopulate,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const quartz::rendering::Vertex::AttributeType attributeType
) {
    const std::string attributeString =
        quartz::rendering::Vertex::getAttributeGLTFString(attributeType);
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "{}", attributeString);

    if (gltfPrimitive.attributes.find(attributeString) == gltfPrimitive.attributes.end()) {
        LOG_TRACE(MODEL, "Primitive does not contain a {} attribute", attributeString);

        switch (attributeType) {
            case quartz::rendering::Vertex::AttributeType::Position:
            case quartz::rendering::Vertex::AttributeType::Normal:
                LOG_CRITICAL(MODEL, "Primitive must contain a {} attribute", attributeString);
                throw std::runtime_error("");
            case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
                LOG_CRITICAL(MODEL, "Using default base color texture");
                break;
            default:
                break;
        }

        return;
    }

    LOG_TRACE(MODEL, "Loading {} attribute", attributeString);

    const tinygltf::Accessor& accessor = gltfModel.accessors[gltfPrimitive.attributes.find(attributeString)->second];
    const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];

    const float* p_data = reinterpret_cast<const float*>(&(
        gltfModel.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]
    ));

    uint32_t tinygltfVecType = TINYGLTF_TYPE_VEC3;
    if (attributeType == quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate) {
        LOG_TRACE(MODEL, "{} attribute uses vector2");
        tinygltfVecType = TINYGLTF_TYPE_VEC2;
    }

    const int32_t byteStride =
        accessor.ByteStride(bufferView) ?
            accessor.ByteStride(bufferView) / sizeof (float) :
            tinygltf::GetNumComponentsInType(tinygltfVecType);

    for (uint32_t j = 0; j < verticesToPopulate.size(); ++j) {
        switch (attributeType) {
            case quartz::rendering::Vertex::AttributeType::Position: {
                verticesToPopulate[j].position = glm::make_vec3(&p_data[j * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Normal: {
                verticesToPopulate[j].normal = glm::make_vec3(&p_data[j * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Color: {
                verticesToPopulate[j].color = glm::make_vec3(&p_data[j * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate: {
                verticesToPopulate[j].baseColorTextureCoordinate = glm::make_vec2(&p_data[j * byteStride]);
                break;
            }
        }
    }
}

std::vector<quartz::rendering::Vertex>
quartz::rendering::Model::loadMeshVertices(
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

        std::vector<quartz::rendering::Vertex> meshVertices;

        LOG_TRACE(MODEL, "Considering {} primitives", gltfMesh.primitives.size());
        for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i) {
            LOG_TRACE(MODEL, "Primitive {}", i);

            const tinygltf::Primitive& gltfPrimitive = gltfMesh.primitives[i];

            if (gltfPrimitive.attributes.find("POSITION") == gltfPrimitive.attributes.end()) {
                LOG_CRITICAL(MODEL, "Primitive {} does not contain a position attribute", i);
                throw std::runtime_error("");
            }

            const uint32_t vertexCount = gltfModel.accessors[gltfPrimitive.attributes.find("POSITION")->second].count;
            std::vector<quartz::rendering::Vertex> primitiveVertices(vertexCount);

            std::vector<quartz::rendering::Vertex::AttributeType> attributeTypes = {
                quartz::rendering::Vertex::AttributeType::Position,
                quartz::rendering::Vertex::AttributeType::Normal,
                quartz::rendering::Vertex::AttributeType::Color,
                quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate,
            };

            for (const quartz::rendering::Vertex::AttributeType attributeType : attributeTypes) {
                quartz::rendering::Model::populateVerticesWithAttribute(
                    primitiveVertices,
                    gltfModel,
                    gltfPrimitive,
                    attributeType
                );
            }

//            quartz::rendering::Model::populateVerticesWithAttribute(
//                primitiveVertices,
//                gltfModel,
//                gltfPrimitive,
//                quartz::rendering::Vertex::AttributeType::Position
//            );
//
//            quartz::rendering::Model::populateVerticesWithAttribute(
//                primitiveVertices,
//                gltfModel,
//                gltfPrimitive,
//                quartz::rendering::Vertex::AttributeType::Normal
//            );
//
//            quartz::rendering::Model::populateVerticesWithAttribute(
//                primitiveVertices,
//                gltfModel,
//                gltfPrimitive,
//                quartz::rendering::Vertex::AttributeType::Color
//            );
//
//            quartz::rendering::Model::populateVerticesWithAttribute(
//                primitiveVertices,
//                gltfModel,
//                gltfPrimitive,
//                quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate
//            );

            LOG_TRACE(MODEL, "Primitive loaded {} vertices", primitiveVertices.size());
            meshVertices.insert(
                meshVertices.end(),
                primitiveVertices.begin(),
                primitiveVertices.end()
            );
            LOG_TRACE(MODEL, "Mesh now contains {} vertices", meshVertices.size());
        }

    return meshVertices;
}

std::vector<uint32_t>
quartz::rendering::Model::loadMeshIndices(
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<uint32_t> meshIndices;

    LOG_TRACE(MODEL, "Considering {} primitives", gltfMesh.primitives.size());
    for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i) {
        LOG_TRACE(MODEL, "Primitive {}", i);

        const tinygltf::Primitive& gltfPrimitive = gltfMesh.primitives[i];

        if (gltfPrimitive.indices <= -1) {
            LOG_TRACE(MODEL, "Primitive does not contain any indices");
            continue;
        }

        const uint32_t indexCount = gltfModel.accessors[gltfPrimitive.indices].count;
        std::vector<uint32_t> primitiveIndices(indexCount);

        const tinygltf::Accessor& indexAccessor = gltfModel.accessors[
            gltfPrimitive.indices > -1 ?
                gltfPrimitive.indices :
                0
        ];

        const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer = gltfModel.buffers[indexBufferView.buffer];

        switch (indexAccessor.componentType) {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                LOG_TRACE(MODEL, "  - using indices of type uint32_t");
                const uint32_t* p_indices = reinterpret_cast<const uint32_t*>(&(
                    indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]
                ));
                for (uint32_t j = 0; j < indexAccessor.count; ++j) {
                    primitiveIndices[j] = p_indices[j];
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                LOG_TRACE(MODEL, "  - using indices of type uint16_t");
                const uint16_t* p_indices = reinterpret_cast<const uint16_t*>(&(
                    indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]
                ));
                for (uint16_t j = 0; j < indexAccessor.count; ++j) {
                    primitiveIndices[j] = p_indices[j];
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                LOG_TRACE(MODEL, "  - using indices of type uint8_t");
                const uint8_t* p_indices = reinterpret_cast<const uint8_t*>(&(
                    indexBuffer.data[indexAccessor.byteOffset + indexBufferView.byteOffset]
                ));
                for (uint8_t j = 0; j < indexAccessor.count; ++j) {
                    primitiveIndices[j] = p_indices[j];
                }
                break;
            }
        }

        LOG_TRACE(MODEL, "Primitive loaded {} indices", primitiveIndices.size());
        meshIndices.insert(
            meshIndices.end(),
            primitiveIndices.begin(),
            primitiveIndices.end()
        );
        LOG_TRACE(MODEL, "Mesh now contains {} indices", meshIndices.size());
    }

    return meshIndices;
}

std::vector<quartz::rendering::Mesh>
quartz::rendering::Model::loadMeshes(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    const tinygltf::Scene& gltfScene = quartz::rendering::Model::loadDefaultScene(gltfModel);

    std::queue<const tinygltf::Node*> gltfNodePtrs =
        quartz::rendering::Model::loadNodePtrQueue(
            gltfModel,
            gltfScene
        );

    std::vector<quartz::rendering::Mesh> meshes;

    LOG_TRACE(MODEL, "Loading vertex and index information");
    while (!gltfNodePtrs.empty()) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);

        const tinygltf::Node* p_gltfNode = gltfNodePtrs.front();
        gltfNodePtrs.pop();

        if (p_gltfNode->mesh <= -1) {
            LOG_TRACE(MODEL, "Not considering this node for mesh information");
            continue;
        }

        LOG_TRACE(MODEL, "Using mesh {}", p_gltfNode->mesh);
        const tinygltf::Mesh& gltfMesh = gltfModel.meshes[p_gltfNode->mesh];

        const std::vector<quartz::rendering::Vertex> vertices =
            quartz::rendering::Model::loadMeshVertices(
                gltfModel,
                gltfMesh
            );

        const std::vector<uint32_t> indices =
            quartz::rendering::Model::loadMeshIndices(
                gltfModel,
                gltfMesh
            );

        meshes.emplace_back(quartz::rendering::Mesh(
            renderingDevice,
            vertices,
            indices
        ));
    }

    return meshes;
}

std::vector<quartz::rendering::Texture>
quartz::rendering::Model::loadTextures(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    LOG_TRACE(MODEL, "Got {} texture samplers from gltf model", gltfModel.samplers.size());
    LOG_TRACE(MODEL, "Got {} textures from gltf model", gltfModel.textures.size());

    std::vector<quartz::rendering::Texture> textures;

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

        textures.emplace_back(quartz::rendering::Texture(
            renderingDevice,
            gltfImage,
            gltfSampler
        ));
    }

    return textures;
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath
) :
    m_gltfModel(quartz::rendering::Model::loadGLTFModel(objectFilepath)),
    m_meshes(
        quartz::rendering::Model::loadMeshes(
            renderingDevice,
            m_gltfModel
        )
    ),
    m_textures(
        quartz::rendering::Model::loadTextures(
            renderingDevice,
            m_gltfModel
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_gltfModel(other.m_gltfModel),
    m_meshes(std::move(other.m_meshes)),
    m_textures(std::move(other.m_textures))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}