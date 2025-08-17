#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"
#include "math/transform/Vec4.hpp"

#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "quartz/scene/camera/Camera.hpp"

UT_FUNCTION(test_construction) {
    const quartz::scene::Camera camera1(
        90,
        {0, 0, 0},
        {1, 2, 3}
    );
    UT_CHECK_EQUAL(camera1.getId(), 0);
    UT_CHECK_EQUAL_FLOATS(camera1.getFovDegrees(), 90);
    UT_CHECK_EQUAL(camera1.getWorldPosition(), math::Vec3(0, 0, 0));
    UT_CHECK_EQUAL(camera1.getLookDirection(), math::Vec3(1, 2, 3).normalize());

    {
        const quartz::scene::Camera camera2(
            40.444,
            {44, 55, 66},
            {-1, -2, -3}
        );
        UT_CHECK_EQUAL(camera2.getId(), 1);
        UT_CHECK_EQUAL_FLOATS(camera2.getFovDegrees(), 40.444);
        UT_CHECK_EQUAL(camera2.getWorldPosition(), math::Vec3(44, 55, 66));
        UT_CHECK_EQUAL(camera2.getLookDirection(), math::Vec3(-1, -2, -3).normalize());
    }

    const quartz::scene::Camera camera3(
        -40.444,
        {-44, 55, -66},
        {-1, -2222, -3}
    );
    UT_CHECK_EQUAL(camera3.getId(), 2);
    UT_CHECK_EQUAL_FLOATS(camera3.getFovDegrees(), -40.444);
    UT_CHECK_EQUAL(camera3.getWorldPosition(), math::Vec3(-44, 55, -66));
    UT_CHECK_EQUAL_FLOATS(camera3.getLookDirection().x, math::Vec3(-1, -2222, -3).normalize().x);
    UT_CHECK_EQUAL_FLOATS(camera3.getLookDirection().y, math::Vec3(-1, -2222, -3).normalize().y);
    UT_CHECK_EQUAL_FLOATS(camera3.getLookDirection().z, math::Vec3(-1, -2222, -3).normalize().z);
}

UT_FUNCTION(test_setPosition) {
    quartz::scene::Camera camera;
    UT_CHECK_EQUAL(camera.getWorldPosition(), math::Vec3(0, 0, 0));

    camera.setPosition({3, 3, 3});

    UT_CHECK_EQUAL(camera.getWorldPosition(), math::Vec3(3, 3, 3));

    /**
     * @todo 2025/08/07 Check view matrix were updated correctly
     */
}

UT_FUNCTION(test_setRotation) {

}

UT_FUNCTION(test_setRotationDegrees) {

}

UT_FUNCTION(test_rotateDegrees) {

}

UT_FUNCTION(test_update) {
    /**
     * @todo 2025/08/07 Check view, model, projection matrices were updated correctly
     */
}

UT_FUNCTION(test_UBO) {
    // Create UBO default construction
    const quartz::scene::Camera::UniformBufferObject ubo1;
    UT_CHECK_EQUAL(ubo1.position, math::Vec3(0));
    UT_CHECK_EQUAL(ubo1.viewMatrix, math::Mat4(0));
    UT_CHECK_EQUAL(ubo1.projectionMatrix, math::Mat4(0));

    // Create UBO from individual values
    const math::Vec3 position(2, 3, 4);
    const math::Mat4 viewMatrix(
        std::array<math::Vec4, 4>{
            math::Vec4(1),
            math::Vec4(2, 3, 4, 5),
            math::Vec4(-5),
            math::Vec4(77.99)
        }
    );
    const math::Mat4 projectionMatrix(94.22);
    const quartz::scene::Camera::UniformBufferObject ubo2(position, viewMatrix, projectionMatrix);
    UT_CHECK_EQUAL(ubo2.position, position);
    UT_CHECK_EQUAL(ubo2.viewMatrix, viewMatrix);
    UT_CHECK_EQUAL(ubo2.projectionMatrix, projectionMatrix);

    // Create UBO from camera
    const quartz::scene::Camera camera(42.42, {7, 8, 9}, {-4, -3, -2});
    const quartz::scene::Camera::UniformBufferObject ubo3(camera);
    UT_CHECK_EQUAL(ubo3.position, camera.getWorldPosition());
    UT_CHECK_EQUAL(ubo3.viewMatrix, camera.getViewMatrix());
    UT_CHECK_EQUAL(ubo3.projectionMatrix, camera.getProjectionMatrix());
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_setPosition);
    REGISTER_UT_FUNCTION(test_setRotation);
    REGISTER_UT_FUNCTION(test_setRotationDegrees);
    REGISTER_UT_FUNCTION(test_rotateDegrees);
    REGISTER_UT_FUNCTION(test_update);
    REGISTER_UT_FUNCTION(test_UBO);
    UT_RUN_TESTS();
}

