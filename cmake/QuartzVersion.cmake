#====================================================================
# The Major, Minor, and Patch versions of Quartz
#====================================================================

function(set_quartz_major_minor_patch_versions major minor patch)
    set(QUARTZ_MAJOR_VERSION ${major} PARENT_SCOPE)
    set(QUARTZ_MINOR_VERSION ${minor} PARENT_SCOPE)
    set(QUARTZ_PATCH_VERSION ${patch} PARENT_SCOPE)
endfunction()

# -----=====***** the versions *****=====----- #

# ----- Major Version 0 ----- #

# Minor Version 1 - Fundamental Rendering

#set_quartz_major_minor_patch_versions(0 0 0) # initialize project and get vulkan working
#set_quartz_major_minor_patch_versions(0 1 0) # load 3d models
#set_quartz_major_minor_patch_versions(0 1 1) # 3d camera movement
#set_quartz_major_minor_patch_versions(0 1 2) # load gltf models, default & indexed textures
#set_quartz_major_minor_patch_versions(0 1 3) # multiple gltf models
#set_quartz_major_minor_patch_versions(0 1 4) # default textures, materials, normal textures, emission textures, occlusion textures
#set_quartz_major_minor_patch_versions(0 1 5) # metallic roughness PBR BRDF with point lights and spot lights
#set_quartz_major_minor_patch_versions(0 1 6) # skyboxes =D so preeetttttttty (and ability to create multiple pipelines)
#set_quartz_major_minor_patch_versions(0 1 7) # release mode compilation fix for unused variables
#set_quartz_major_minor_patch_versions(0 1 8) # port to linux, clean up compile definitions to be target based

# Minor Version 2 - Fundamental Physics

#set_quartz_major_minor_patch_versions(0 2 0) # allow for physics in our scenes with reactphysics3d
#set_quartz_major_minor_patch_versions(0 2 1) # give physics properties to the scene, so each scene has custom physics
#set_quartz_major_minor_patch_versions(0 2 2) # invoke Doodad callback functions within update and fixedUpdate functions
#set_quartz_major_minor_patch_versions(0 2 3) # allow client to own the camera and update it from within update functions
set_quartz_major_minor_patch_versions(0 2 4) # implement collider layer masks

