#====================================================================
# The Major, Minor, and Patch versions of the application
#====================================================================

function(set_application_major_minor_patch_versions major minor patch)
    set(APPLICATION_MAJOR_VERSION ${major} PARENT_SCOPE)
    set(APPLICATION_MINOR_VERSION ${minor} PARENT_SCOPE)
    set(APPLICATION_PATCH_VERSION ${patch} PARENT_SCOPE)
endfunction()

# -----=====***** the versions *****=====----- #

set_application_major_minor_patch_versions(0 0 0) # dummy app not doing anything