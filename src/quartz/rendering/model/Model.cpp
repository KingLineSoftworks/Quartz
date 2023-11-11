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

        if (attributeType == quartz::rendering::Vertex::AttributeType::Position) {
            LOG_CRITICAL(MODEL, "Primitive does not have a position attribute");
            throw std::runtime_error("");
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
    if (attributeType == quartz::rendering::Vertex::AttributeType::TextureCoordinate) {
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
                verticesToPopulate[j].worldPosition = glm::make_vec3(&p_data[j * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Color: {
                verticesToPopulate[j].color = glm::make_vec3(&p_data[j * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::TextureCoordinate: {
                verticesToPopulate[j].textureCoordinate = glm::make_vec2(&p_data[j * byteStride]);
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

            quartz::rendering::Model::populateVerticesWithAttribute(
                primitiveVertices,
                gltfModel,
                gltfPrimitive,
                quartz::rendering::Vertex::AttributeType::Position
            );

            quartz::rendering::Model::populateVerticesWithAttribute(
                primitiveVertices,
                gltfModel,
                gltfPrimitive,
                quartz::rendering::Vertex::AttributeType::Color
            );

            quartz::rendering::Model::populateVerticesWithAttribute(
                primitiveVertices,
                gltfModel,
                gltfPrimitive,
                quartz::rendering::Vertex::AttributeType::TextureCoordinate
            );

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
quartz::rendering::Model::loadMeshIndices() {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");
    return {};
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

    uint32_t totalIndexCount = 0;

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

        LOG_TRACE(MODEL, "Considering {} primitives", gltfMesh.primitives.size());
        for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i) {
            LOG_TRACE(MODEL, "Primitive {}", i);

            const tinygltf::Primitive& gltfPrimitive = gltfMesh.primitives[i];

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

            meshes.emplace_back(quartz::rendering::Mesh(
                renderingDevice,
                vertices,
                indices
            ));
        }
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

void
quartz::rendering::Model::loadGLTFModelFull(
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
    const std::string& objectFilepath,
    UNUSED const std::string& textureFilepath
) :
//    m_tinyobjAttribute(),
//    m_tinyobjShapes(),
//    m_tinyobjMaterials(),
//    m_loadedSuccessfully(
//        quartz::rendering::Model::loadModel(
//            objectFilepath,
//            m_tinyobjAttribute,
//            m_tinyobjShapes,
//            m_tinyobjMaterials
//        )
//    ),
//    m_mesh(
//        renderingDevice,
//        m_tinyobjAttribute,
//        m_tinyobjShapes
//    ),
//    m_texture(renderingDevice, textureFilepath)
    m_gltfModel(quartz::rendering::Model::loadGLTFModel(objectFilepath)),
    m_mesh(std::move(
        quartz::rendering::Model::loadMeshes(
            renderingDevice,
            m_gltfModel
        )[0]
    )),
    m_texture(std::move(
        quartz::rendering::Model::loadTextures(
            renderingDevice,
            m_gltfModel
        )[0]
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
//    quartz::rendering::Model::loadGLTFModelFull(renderingDevice);
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
//    m_tinyobjAttribute(other.m_tinyobjAttribute),
//    m_tinyobjShapes(other.m_tinyobjShapes),
//    m_tinyobjMaterials(other.m_tinyobjMaterials),
//    m_loadedSuccessfully(other.m_loadedSuccessfully),
    m_gltfModel(other.m_gltfModel),
    m_mesh(std::move(other.m_mesh)),
    m_texture(std::move(other.m_texture))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}