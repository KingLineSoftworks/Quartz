#include "util/unit_test/UnitTest.hpp"

UT_FUNCTION(test_AmbientLight) {

}

UT_FUNCTION(test_DirectionalLight) {

}

UT_FUNCTION(test_PointLight) {

}

UT_FUNCTION(test_SpotLight) {

}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_AmbientLight);
    REGISTER_UT_FUNCTION(test_DirectionalLight);
    REGISTER_UT_FUNCTION(test_PointLight);
    REGISTER_UT_FUNCTION(test_SpotLight);
    UT_RUN_TESTS();
}
