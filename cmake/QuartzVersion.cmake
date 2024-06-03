#====================================================================
# The Major, Minor, and Patch versions of Quartz
#====================================================================

function(set_quartz_major_minor_patch_versions major minor patch)
    set(QUARTZ_MAJOR_VERSION ${major} PARENT_SCOPE)
    set(QUARTZ_MINOR_VERSION ${minor} PARENT_SCOPE)
    set(QUARTZ_PATCH_VERSION ${patch} PARENT_SCOPE)
endfunction()

# -----=====***** the versions *****=====----- #

#set_quartz_major_minor_patch_versions(0 0 0) # initialize project and get vulkan working
#set_quartz_major_minor_patch_versions(0 1 0) # load 3d models
#set_quartz_major_minor_patch_versions(0 1 1) # 3d camera movement
#set_quartz_major_minor_patch_versions(0 1 2) # load gltf models, default & indexed textures
#set_quartz_major_minor_patch_versions(0 1 3) # multiple gltf models
#set_quartz_major_minor_patch_versions(0 1 4) # default textures, materials, normal textures, emission textures, occlusion textures
#set_quartz_major_minor_patch_versions(0 1 5) # metallic roughness PBR BRDF with point lights and spot lights
set_quartz_major_minor_patch_versions(0 1 6) # skyboxes =D so preeetttttttty
