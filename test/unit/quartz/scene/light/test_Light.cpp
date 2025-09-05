#include "math/transform/Vec3.hpp"

#include "util/unit_test/UnitTest.hpp"

#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"
#include "quartz/scene/light/PointLight.hpp"
#include "quartz/scene/light/SpotLight.hpp"

UT_FUNCTION(test_AmbientLight) {
    quartz::scene::AmbientLight light({0, 1, 2});
    UT_CHECK_EQUAL(light.color, math::Vec3(0, 1, 2));
}

UT_FUNCTION(test_DirectionalLight) {
    quartz::scene::DirectionalLight light({0, 1, 2}, {-1, -4, -6});
    UT_CHECK_EQUAL(light.color, math::Vec3(0, 1, 2));
    UT_CHECK_EQUAL(light.direction, math::Vec3(-1, -4, -6));
}

UT_FUNCTION(test_PointLight) {
    quartz::scene::PointLight light({0, 1, 2}, {-1, -4, -6}, 4, 5.7);
    UT_CHECK_EQUAL(light.color, math::Vec3(0, 1, 2));
    UT_CHECK_EQUAL(light.position, math::Vec3(-1, -4, -6));
    UT_CHECK_EQUAL_FLOATS(light.attenuationLinearFactor, 4);
    UT_CHECK_EQUAL_FLOATS(light.attenuationQuadraticFactor, 5.7);
}

UT_FUNCTION(test_SpotLight) {
    quartz::scene::SpotLight light(
        {0, 1, 2},
        {-1, -4, -6},
        {55, 66, 77},
        32,
        42.42,
        4,
        5.7
    );
    UT_CHECK_EQUAL(light.color, math::Vec3(0, 1, 2));
    UT_CHECK_EQUAL(light.position, math::Vec3(-1, -4, -6));
    UT_CHECK_EQUAL(light.direction, math::Vec3(55, 66, 77));
    UT_CHECK_EQUAL_FLOATS(light.innerRadiusDegrees, 32);
    UT_CHECK_EQUAL_FLOATS(light.outerRadiusDegrees, 42.42);
    UT_CHECK_EQUAL_FLOATS(light.attenuationLinearFactor, 4);
    UT_CHECK_EQUAL_FLOATS(light.attenuationQuadraticFactor, 5.7);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_AmbientLight);
    REGISTER_UT_FUNCTION(test_DirectionalLight);
    REGISTER_UT_FUNCTION(test_PointLight);
    REGISTER_UT_FUNCTION(test_SpotLight);
    UT_RUN_TESTS();
}
