#include <array>

#include "util/unit_test/UnitTest.hpp"

#include "math/Loggers.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"
#include "math/transform/Transform.hpp"

math::Mat4
calculateTranslationMatrix(
    const math::Transform& t
) {
    // Col major instantiation
    const math::Mat4 translationMatrix(
        std::array<math::Vec4, 4>{
            math::Vec4(1, 0, 0, 0), // col 0
            math::Vec4(0, 1, 0, 0), // col 1
            math::Vec4(0, 0, 1, 0), // col 2
            math::Vec4(t.position.x, t.position.y, t.position.z, 1) // col 3
        }
    );

    return translationMatrix;
}

math::Mat4
calcualteRotationMatrix(
    const math::Transform& t
) {
    const math::Mat4 rotationMatrix(t.rotation);

    return rotationMatrix;
}

math::Mat4
calculateScaleMatrix(
    const math::Transform& t
) {
    const math::Mat4 scaleMatrix(
        std::array<math::Vec4, 4>{
            math::Vec4(t.scale.x, 0, 0, 0),
            math::Vec4(0, t.scale.y, 0, 0),
            math::Vec4(0, 0, t.scale.z, 0),
            math::Vec4(0, 0, 0, 1)
        }
    );

    return scaleMatrix;
}

math::Mat4
calculateExpectedMatrix(
    const math::Transform& t
) {
    LOG_FUNCTION_SCOPE_INFO(UT, "");

    const math::Mat4 translation = calculateTranslationMatrix(t);
    LOG_INFO(UT, "Translation:");
    LOG_INFO(UT, "  {}", translation.toString());

    const math::Mat4 rotation = calcualteRotationMatrix(t);
    LOG_INFO(UT, "Rotation:");
    LOG_INFO(UT, "  {}", rotation.toString());

    const math::Mat4 scale = calculateScaleMatrix(t);
    LOG_INFO(UT, "Scale:");
    LOG_INFO(UT, "  {}", scale.toString());

    const math::Mat4 expected = translation * rotation * scale;
    LOG_INFO(UT, "Full:");
    LOG_INFO(UT, "  {}", expected.toString());

    return expected;
}

UT_FUNCTION(test_calculateTransformationMatrix_0) {
    const math::Transform t(
        {0, 0, 0},
        math::Quaternion::fromDirectionVector(math::Vec3::Forward),
        {1, 1, 1}
    );

    const math::Mat4 actual = t.calculateTransformationMatrix();
    const math::Mat4 expected = calculateExpectedMatrix(t);

    UT_CHECK_EQUAL(actual, expected);
}

UT_FUNCTION(test_calculateTransformationMatrix_1) {
    const math::Transform t(
        {4, 2, 3},
        math::Quaternion::fromDirectionVector(math::Vec3::Left),
        {3, 2, 4}
    );

    const math::Mat4 actual = t.calculateTransformationMatrix();
    const math::Mat4 expected = calculateExpectedMatrix(t);

    UT_CHECK_EQUAL(actual, expected);
}

UT_FUNCTION(test_calculateTransformationMatrix_2) {
    const math::Transform t(
        {44.11, 200, -3.33},
        math::Quaternion::fromEulerAngles(23, 255, 987),
        {-3, 20, 0.4}
    );

    const math::Mat4 actual = t.calculateTransformationMatrix();
    const math::Mat4 expected = calculateExpectedMatrix(t);

    UT_CHECK_EQUAL(actual, expected);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_calculateTransformationMatrix_0);
    REGISTER_UT_FUNCTION(test_calculateTransformationMatrix_1);
    REGISTER_UT_FUNCTION(test_calculateTransformationMatrix_2);
    UT_RUN_TESTS();
}
