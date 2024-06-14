#====================================================================
# A function to get all of the desired pedantic warning flags
#====================================================================

set(
    QUARTZ_CMAKE_CXX_CUSTOM_PEDANTIC_WARNING_FLAGS

    # for nice printing :)
    -fcolor-diagnostics

    -Wc++11-extra-semi
    -Wc++11-long-long
    -Wc++14-binary-literal
    -Wc++20-designator
    -Wc11-extensions
    -Wcomplex-component-init
    -Wdollar-in-identifier-extension
    -Wembedded-directive
    -Wempty-translation-unit
    -Wfixed-enum-extension
    -Wflexible-array-extensions
    -Wgnu-anonymous-struct
    -Wgnu-auto-type
    -Wgnu-binary-literal
    -Wgnu-case-range
    -Wgnu-complex-integer
    -Wgnu-compound-literal-initializer
    -Wgnu-conditional-omitted-operand
    -Wgnu-empty-struct
    -Wgnu-flexible-array-initializer
    -Wgnu-flexible-array-union-member
    -Wgnu-imaginary-constant
    -Wgnu-include-next
    -Wgnu-label-as-value
    -Wgnu-redeclared-enum
    -Wgnu-statement-expression
    -Wgnu-union-cast
    -Wgnu-zero-line-directive
    -Wimport-preprocessor-directive-pedantic
    -Wkeyword-macro
    -Wlanguage-extension-token
    -Wlong-long
    -Wmicrosoft-charize
    -Wmicrosoft-comment-paste
    -Wmicrosoft-cpp-macro
    -Wmicrosoft-end-of-file
    -Wmicrosoft-enum-value
    -Wmicrosoft-fixed-enum
    -Wmicrosoft-flexible-array
    -Wmicrosoft-redeclare-static
    # -Wnested-anon-types # Have to disable this for something in reactphysics3d
    -Wnullability-extension
    -Woverlength-strings
    -Wretained-language-linkage
    -Wundefined-internal-type
    -Wvla-extension
    -Wzero-length-array

    # ============================================================
    # Unwanted warning flag(s)
    #       * and the reason why
    # ============================================================
    
    # -Wgnu-zero-variadic-macro-arguments
    #       * disallows __VA_OPT__ functionality in Logger.hpp
    
    # ============================================================
    # Unavailable warning flag(s)
    # and the replacement(s) (if avail) - recommended by compiler
    # ============================================================
    
    # -Wbit-int-extension
    
    # -Wc++14-attribute-extensions
    # -Wc++17-attribute-extensions
    # -Wc++20-attribute-extensions
    # -Wfuture-attribute-extensions
    -Wcxx-attribute-extension
    
    # -Wdelimited-escape-sequence-extension
    
    # -Wgeneric-type-extension
    
    # -Wgnu-line-marker
    -Wgnu-case-range
    
    # -Wgnu-null-pointer-arithmetic
    -Wnull-pointer-arithmetic
    
    # -Wgnu-offsetof-extensions
    -Wgnu-alignof-expression
    
    # -Wgnu-pointer-arith
    -Wpointer-arith
    
    # -Winvalid-utf8
    -Winvalid-iboutlet
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
endif ()
