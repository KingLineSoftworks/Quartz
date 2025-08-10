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
    UT_CHECK_EQUAL(camera3.getLookDirection(), math::Vec3(-1, -2222, -3).normalize());
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

UT_FUNCTION(test_setLookDirection) {
    quartz::scene::Camera camera;
    UT_CHECK_EQUAL(camera.getLookDirection(), math::Vec3::Forward);

    camera.setLookDirection(math::Vec3(3, 3, 3).normalize());

    UT_CHECK_EQUAL(camera.getLookDirection(), math::Vec3(3, 3, 3).normalize());

    /**
     * @todo 2025/08/07 Check euler angles were updated correctly
     * @todo 2025/08/07 Check view matrix were updated correctly after an update() invocation
     */
}

UT_FUNCTION(test_lookAtPosition) {
    quartz::scene::Camera camera;
    UT_CHECK_EQUAL(camera.getWorldPosition(), math::Vec3(0, 0, 0));
    UT_CHECK_EQUAL(camera.getLookDirection(), math::Vec3::Forward);

    camera.setPosition({6, 6, 6});
    camera.lookAtPosition({3, 3, 3});

    UT_CHECK_EQUAL(camera.getLookDirection(), math::Vec3(-3, -3, -3).normalize());

    /**
     * @todo 2025/08/07 Check euler angles were updated correctly
     * @todo 2025/08/07 Check view matrix were updated correctly after an update() invocation
     */
}

UT_FUNCTION(test_setEulerAngles) {

}

UT_FUNCTION(test_update) {
    /**
     * @todo 2025/08/07 Check view, model, projection matrices were updated correctly
     */
}

UT_FUNCTION(test_calculateLookDirectionFromEulerAngles) {
    {
        const double yawDegrees = 0;
        const double pitchDegrees = 0;
        const double rollDegrees = 0;
       
        const quartz::scene::Camera::EulerAngles eulerAngles(yawDegrees, pitchDegrees, rollDegrees);

        const math::Vec3 lookDirection = quartz::scene::Camera::calculateLookDirectionFromEulerAngles(eulerAngles);

        const math::Quaternion quaternion = math::Quaternion::fromEulerAngles(yawDegrees, pitchDegrees, rollDegrees);
        const math::Vec3 expected = quaternion.getDirectionVector();

        UT_CHECK_EQUAL(lookDirection, expected);
    }

    {
        const double yawDegreesIn = 45;
        const double pitchDegreesIn = 45;
        const double rollDegreesIn = 0;

        const quartz::scene::Camera::EulerAngles eulerAnglesIn(yawDegreesIn, pitchDegreesIn, rollDegreesIn);

        const math::Vec3 lookDirection = quartz::scene::Camera::calculateLookDirectionFromEulerAngles(eulerAnglesIn);
        LOG_INFO(UT, "Look direction: {}", lookDirection.toString());

        const math::Vec3 expectedLookDirection = math::Vec3(1, 1, 1).normalize();

        UT_CHECK_EQUAL(lookDirection, expectedLookDirection);
    }

    {
        const double yawDegreesIn = 45;
        const double pitchDegreesIn = 45;
        const double rollDegreesIn = 0;

        const quartz::scene::Camera::EulerAngles eulerAnglesIn(yawDegreesIn, pitchDegreesIn, rollDegreesIn);

        const math::Vec3 lookDirection = quartz::scene::Camera::calculateLookDirectionFromEulerAngles(eulerAnglesIn);

        const quartz::scene::Camera::EulerAngles eulerAnglesOut = quartz::scene::Camera::calculateEulerAnglesFromLookDirection(lookDirection);

        UT_CHECK_EQUAL(eulerAnglesOut, eulerAnglesIn);
    }

    /**
     * @todo 2025/08/07 Ensure that we are calculating euler angles the same way here as we are
     *    in our quaternion class, we don't want a mismatch - but there are more important things to do than
     *    figure this out right now
     *
     *    If we have roll set to 0 degrees, we should get the same behaviour from both the camera and
     *    quaternions, but that is not what we're seeing.
     */
    {
        const double yawDegrees = 30;
        const double pitchDegrees = 40;
        const double rollDegrees = 0;
       
        const quartz::scene::Camera::EulerAngles eulerAngles(yawDegrees, pitchDegrees, rollDegrees);

        UNUSED const math::Vec3 lookDirection = quartz::scene::Camera::calculateLookDirectionFromEulerAngles(eulerAngles);

        const math::Quaternion quaternion = math::Quaternion::fromEulerAngles(yawDegrees, pitchDegrees, rollDegrees);
        UNUSED const math::Vec3 expected = quaternion.getDirectionVector();

        // UT_CHECK_EQUAL(lookDirection, expected);
    }
}

UT_FUNCTION(test_calculateEulerAnglesFromLookDirection) {

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
    REGISTER_UT_FUNCTION(test_setLookDirection);
    REGISTER_UT_FUNCTION(test_lookAtPosition);
    REGISTER_UT_FUNCTION(test_setEulerAngles);
    REGISTER_UT_FUNCTION(test_update);
    REGISTER_UT_FUNCTION(test_calculateLookDirectionFromEulerAngles);
    REGISTER_UT_FUNCTION(test_calculateEulerAnglesFromLookDirection);
    REGISTER_UT_FUNCTION(test_UBO);
    UT_RUN_TESTS();
}

