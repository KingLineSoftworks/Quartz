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
                LOG_TRACE(MODEL, "Using default base color texture");
                break;
            default:
                break;
        }

        return;
    }

    LOG_TRACE(MODEL, "Loading {} attribute", attributeString);

    const uint32_t accessorIndex = gltfPrimitive.attributes.find(attributeString)->second;
    LOG_TRACE(MODEL, "Using accessor at index {}", accessorIndex);
    const tinygltf::Accessor& accessor = gltfModel.accessors[accessorIndex];

    const uint32_t bufferViewIndex = accessor.bufferView;
    LOG_TRACE(MODEL, "Using buffer view at index {}", bufferViewIndex);
    const tinygltf::BufferView& bufferView = gltfModel.bufferViews[bufferViewIndex];

    const uint32_t bufferIndex = bufferView.buffer;
    LOG_TRACE(MODEL, "Using buffer at index {}", bufferIndex);
    const tinygltf::Buffer& buffer = gltfModel.buffers[bufferIndex];

    const uint32_t accessorByteOffset = accessor.byteOffset;
    const uint32_t bufferViewByteOffset = bufferView.byteOffset;
    const std::vector<uint8_t>& bufferData = buffer.data;
    const uint8_t* bufferDataStartAddress = bufferData.data();
    const uint8_t* desiredDataStartAddress = bufferDataStartAddress + accessorByteOffset + bufferViewByteOffset;
    LOG_TRACE(MODEL, "Accessor uses byte offset of {}", accessorByteOffset);
    LOG_TRACE(MODEL, "Buffer view uses byte offset of {}", bufferViewByteOffset);
    LOG_TRACE(MODEL, "Using total byte offset of {}", accessorByteOffset + bufferViewByteOffset);
    LOG_TRACE(MODEL, "Buffer data starting at {}", reinterpret_cast<const void*>(bufferDataStartAddress));
    LOG_TRACE(MODEL, "Using data starting at {}", reinterpret_cast<const void*>(desiredDataStartAddress));
    LOG_TRACE(
        MODEL, "{} + {} = {} SHOULD EQUAL {}",
        reinterpret_cast<const void*>(bufferDataStartAddress),
        accessorByteOffset + bufferViewByteOffset,
        reinterpret_cast<const void*>(bufferDataStartAddress + accessorByteOffset + bufferViewByteOffset),
        reinterpret_cast<const void*>(desiredDataStartAddress)
    );

    const float* p_data = reinterpret_cast<const float*>(desiredDataStartAddress);

    uint32_t tinygltfVecType = TINYGLTF_TYPE_VEC3;
    if (attributeType == quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate) {
        LOG_TRACE(MODEL, "{} attribute uses vector2", attributeString);
        tinygltfVecType = TINYGLTF_TYPE_VEC2;
    }

    const int32_t byteStride =
        accessor.ByteStride(bufferView) ?
            accessor.ByteStride(bufferView) / sizeof (float) :
            tinygltf::GetNumComponentsInType(tinygltfVecType);
    LOG_TRACE(MODEL, "Using a byte stride of {}", byteStride);

    for (uint32_t i = 0; i < verticesToPopulate.size(); ++i) {
        switch (attributeType) {
            case quartz::rendering::Vertex::AttributeType::Position: {
                verticesToPopulate[i].position = glm::make_vec3(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Normal: {
                verticesToPopulate[i].normal = glm::make_vec3(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Color: {
                verticesToPopulate[i].color = glm::make_vec3(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate: {
                verticesToPopulate[i].baseColorTextureCoordinate = glm::make_vec2(&p_data[i * byteStride]);
                break;
            }
        }
    }
}

std::vector<quartz::rendering::Vertex>
quartz::rendering::Model::loadModelVertices(
    UNUSED const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<quartz::rendering::Vertex> vertices;

    return vertices;
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
            continue;
        }

        const uint32_t accessorIndex = gltfPrimitive.attributes.find("POSITION")->second;
        const uint32_t vertexCount = gltfModel.accessors[accessorIndex].count;
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

/**
 * @todo 2023/11/26 Create list of primitives so we can give it to the mesh. The
 *   primitives need to know their indices, and their indices need to be referencing
 *   the vertices that the mesh is containing. It is probably fine if we copy the
 *   indices into the primitive instead of maintaining a view of the mesh "master
 *   index buffer" for now. We can reconstruct the "master indices" list in the mesh
 *   from all of the primitives it gets
 */

std::vector<uint32_t>
quartz::rendering::Model::loadMeshIndices(
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<uint32_t> meshIndices;
    std::vector<quartz::rendering::Primitive> primitives;

    LOG_TRACE(MODEL, "Considering {} primitives", gltfMesh.primitives.size());
    for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i) {
        LOG_TRACE(MODEL, "Primitive {}", i);

        const tinygltf::Primitive& gltfPrimitive = gltfMesh.primitives[i];

        if (gltfPrimitive.indices <= -1) {
            LOG_TRACE(MODEL, "Primitive does not contain any indices");
            continue;
        }

        const uint32_t indexAccessorIndex = gltfPrimitive.indices; // will always be >= 0
        const tinygltf::Accessor& indexAccessor = gltfModel.accessors[indexAccessorIndex];
        const uint32_t indexCount = indexAccessor.count;

        const uint32_t indexBufferViewIndex = indexAccessor.bufferView;
        const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews[indexBufferViewIndex];

        const uint32_t indexBufferIndex = indexBufferView.buffer;
        const tinygltf::Buffer& indexBuffer = gltfModel.buffers[indexBufferIndex];

        const uint32_t indexAccessorByteOffset = indexAccessor.byteOffset;
        const uint32_t indexBufferViewByteOffset = indexBufferView.byteOffset;
        const std::vector<uint8_t>& indexBufferData = indexBuffer.data;
        const uint8_t* indexBufferDataStartAddress = indexBufferData.data();
        const uint8_t* desiredIndexDataStartAddress = indexBufferDataStartAddress + indexAccessorByteOffset + indexBufferViewByteOffset;

        std::vector<uint32_t> primitiveIndices(indexCount);

        switch (indexAccessor.componentType) {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                LOG_TRACE(MODEL, "  - using {} indices of type uint32_t", indexCount);
                const uint32_t* p_indices = reinterpret_cast<const uint32_t*>(desiredIndexDataStartAddress);
                for (uint32_t j = 0; j < indexCount; ++j) {
                    primitiveIndices[j] = p_indices[j];
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                LOG_TRACE(MODEL, "  - using {} indices of type uint16_t", indexCount);
                const uint16_t* p_indices = reinterpret_cast<const uint16_t*>(desiredIndexDataStartAddress);
                for (uint16_t j = 0; j < indexCount; ++j) {
                    primitiveIndices[j] = p_indices[j];
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                LOG_TRACE(MODEL, "  - using {} indices of type uint8_t", indexCount);
                const uint8_t* p_indices = reinterpret_cast<const uint8_t*>(desiredIndexDataStartAddress);
                for (uint8_t j = 0; j < indexCount; ++j) {
                    primitiveIndices[j] = p_indices[j];
                }
                break;
            }
        }

        const uint32_t startIndex = meshIndices.size();
        const uint32_t endIndex = startIndex + primitiveIndices.size();
        LOG_TRACE(MODEL, "Inserting {} indices into mesh index list", primitiveIndices.size());
        LOG_TRACE(MODEL, "Primitive cares about indices [ {} , {} ]", startIndex, endIndex);

        primitives.emplace_back(
            startIndex,
            primitiveIndices.size()
        );

        meshIndices.insert(
            meshIndices.end(),
            primitiveIndices.begin(),
            primitiveIndices.end()
        );

        LOG_TRACE(MODEL, "Mesh now contains {} indices", meshIndices.size());
    }

    return meshIndices;
}

std::vector<quartz::rendering::Primitive>
quartz::rendering::Model::loadMeshPrimitives(
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<quartz::rendering::Primitive> primitives;

    uint32_t rollingStartIndex = 0;

    LOG_TRACE(MODEL, "Considering {} primitives", gltfMesh.primitives.size());
    for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i) {
        LOG_TRACE(MODEL, "Primitive {}", i);

        const tinygltf::Primitive& gltfPrimitive = gltfMesh.primitives[i];

        if (gltfPrimitive.indices <= -1) {
            LOG_TRACE(MODEL, "Primitive does not contain any indices");
            continue;
        }

        const uint32_t indexAccessorIndex = gltfPrimitive.indices; // will always be >= 0
        const tinygltf::Accessor& indexAccessor = gltfModel.accessors[indexAccessorIndex];
        const uint32_t indexCount = indexAccessor.count;

        primitives.emplace_back(
            rollingStartIndex,
            indexCount
        );

        rollingStartIndex += indexCount;
    }

    return primitives;
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

        const std::vector<quartz::rendering::Primitive> primitives =
            quartz::rendering::Model::loadMeshPrimitives(
                gltfModel,
                gltfMesh
            );

        meshes.emplace_back(quartz::rendering::Mesh(
            renderingDevice,
            vertices,
            indices,
            primitives
        ));
    }

    return meshes;
}

std::vector<uint32_t>
quartz::rendering::Model::loadTextures(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    LOG_TRACE(MODEL, "Got {} texture samplers from gltf model", gltfModel.samplers.size());
    LOG_TRACE(MODEL, "Got {} textures from gltf model", gltfModel.textures.size());

    quartz::rendering::Texture::initializeMasterList(renderingDevice);

    std::vector<uint32_t> masterIndices;

    for (uint32_t i = 0; i < gltfModel.textures.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        const tinygltf::Texture& gltfTexture = gltfModel.textures[i];

        LOG_TRACE(MODEL, "Creating texture {} with name \"{}\"", i, gltfTexture.name);

        tinygltf::Sampler gltfSampler;
        const int32_t samplerIndex = gltfTexture.sampler;
        if (samplerIndex == -1) {
            LOG_WARNING(MODEL, "No gltf sampler specified. Using default one");
            gltfSampler.minFilter = -1;
            gltfSampler.magFilter = -1;
            gltfSampler.wrapS = -1;
            gltfSampler.wrapT = -1;
            gltfSampler.wrapT = -1;
        } else {
            gltfSampler = gltfModel.samplers[samplerIndex];
            LOG_TRACE(MODEL, "Using gltf sampler {} with name \"{}\"", samplerIndex, gltfSampler.name);
        }

        const int32_t imageIndex = gltfTexture.source;
        const tinygltf::Image& gltfImage = gltfModel.images[imageIndex];
        LOG_TRACE(MODEL, "Using gltf image {} with name \"{}\"", imageIndex, gltfImage.name);

        /**
         * @todo 2023/11/17 We might be able to determine the type of texture we are creating based
         *   on the information in gltfImage and in gltfSampler. We might not be able to store all of
         *   these textures in 1 master list due to their differences in structure.
         */

        masterIndices.emplace_back(quartz::rendering::Texture::createTexture(
            renderingDevice,
            gltfImage,
            gltfSampler
        ));
    }

    return masterIndices;
}

quartz::rendering::Material
quartz::rendering::Model::loadMaterial(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<uint32_t> masterIndices =
        quartz::rendering::Model::loadTextures(
            renderingDevice,
            gltfModel
        );

    int32_t baseColorLocalIndex = -1;
    int32_t normalLocalIndex = -1;
    int32_t emissionLocalIndex = -1;
    int32_t metallicRoughnessLocalIndex = -1;

    LOG_TRACE(TEXTURE, "Processing {} materials", gltfModel.materials.size());
    for (uint32_t i = 0; i < gltfModel.materials.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(TEXTURE);
        LOG_TRACE(TEXTURE, "Processing material {}", i);

        const tinygltf::Material& gltfMaterial = gltfModel.materials[i];

        LOG_TRACE(TEXTURE, "Looking for base color texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator baseColorIterator =
            gltfMaterial.values.find("baseColorTexture");
        if (baseColorIterator != gltfMaterial.values.end()) {
            baseColorLocalIndex = baseColorIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found base color texture local index {}", baseColorLocalIndex);
        }

        LOG_TRACE(TEXTURE, "Looking for normal texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator normalIterator =
            gltfMaterial.values.find("normalTexture");
        if (normalIterator != gltfMaterial.values.end()) {
            normalLocalIndex = normalIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found normal texture local index {}", normalLocalIndex);
        }

        LOG_TRACE(TEXTURE, "Looking for emission texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator emissionIterator =
            gltfMaterial.values.find("emissiveTexture");
        if (emissionIterator != gltfMaterial.values.end()) {
            emissionLocalIndex = emissionIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found emission texture local index {}", emissionLocalIndex);
        }

        LOG_TRACE(TEXTURE, "Looking for metallic roughness texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator metallicRoughnessIterator =
            gltfMaterial.values.find("metallicRoughnessTexture");
        if (metallicRoughnessIterator != gltfMaterial.values.end()) {
            metallicRoughnessLocalIndex = metallicRoughnessIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found metallic roughness texture local index {}", metallicRoughnessLocalIndex);
        }
    }

    const uint32_t baseColorMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getBaseColorDefaultIndex() :
        masterIndices[
            baseColorLocalIndex >= 0 ?
                baseColorLocalIndex :
                0
        ];
    const uint32_t normalMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getNormalDefaultIndex() :
        masterIndices[
            normalLocalIndex >= 0 ?
                normalLocalIndex :
                0
        ];
    const uint32_t emissionMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getEmissionDefaultIndex() :
        masterIndices[
            emissionLocalIndex >= 0 ?
                emissionLocalIndex :
                0
        ];
    const uint32_t metallicRoughnessMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getMetallicRoughnessDefaultIndex() :
        masterIndices[
            metallicRoughnessLocalIndex >= 0 ?
                metallicRoughnessLocalIndex :
                0
        ];
    LOG_TRACE(TEXTURE, "base color         texture master index = {}", baseColorMasterIndex);
    LOG_TRACE(TEXTURE, "normal             texture master index = {}", normalMasterIndex);
    LOG_TRACE(TEXTURE, "emission           texture master index = {}", emissionMasterIndex);
    LOG_TRACE(TEXTURE, "metallic roughness texture master index = {}", metallicRoughnessMasterIndex);

    return {
        baseColorMasterIndex,
        normalMasterIndex,
        emissionMasterIndex,
        metallicRoughnessMasterIndex
    };
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath
) :
    m_gltfModel(quartz::rendering::Model::loadGLTFModel(objectFilepath)),
    m_vertices(),
    m_indices(),
    m_meshes(
        quartz::rendering::Model::loadMeshes(
            renderingDevice,
            m_gltfModel
        )
    ),
    m_material(
        quartz::rendering::Model::loadMaterial(
            renderingDevice,
            m_gltfModel
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_gltfModel(other.m_gltfModel),
    m_vertices(other.m_vertices),
    m_indices(other.m_indices),
    m_meshes(std::move(other.m_meshes)),
    m_material(std::move(other.m_material))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}