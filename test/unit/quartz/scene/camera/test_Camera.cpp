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
        math::Quaternion::fromDirectionVector(math::Vec3(1, 2, 3).normalize())
    );
    UT_CHECK_EQUAL(camera1.getId(), 0);
    UT_CHECK_EQUAL_FLOATS(camera1.getFovDegrees(), 90);
    UT_CHECK_EQUAL(camera1.getWorldPosition(), math::Vec3(0, 0, 0));
    UT_CHECK_EQUAL(camera1.getLookDirection(), math::Vec3(1, 2, 3).normalize());
    /**
     * @todo 2025/08/16 Ensure horizontal, vertical, and clockwise rotations are correct
     */

    {
        const quartz::scene::Camera camera2(
            40.444,
            {44, 55, 66},
            math::Quaternion::fromDirectionVector(math::Vec3(-1, -2, -3).normalize())
        );
        UT_CHECK_EQUAL(camera2.getId(), 1);
        UT_CHECK_EQUAL_FLOATS(camera2.getFovDegrees(), 40.444);
        UT_CHECK_EQUAL(camera2.getWorldPosition(), math::Vec3(44, 55, 66));
        UT_CHECK_EQUAL(camera2.getLookDirection(), math::Vec3(-1, -2, -3).normalize());

        /**
         * @todo 2025/08/16 Ensure horizontal, vertical, and clockwise rotations are correct
         */
    }

    const quartz::scene::Camera camera3(
        -40.444,
        {-44, 55, -66},
        math::Quaternion::fromDirectionVector(math::Vec3(-1, -2222, -3).normalize())
    );
    UT_CHECK_EQUAL(camera3.getId(), 2);
    UT_CHECK_EQUAL_FLOATS(camera3.getFovDegrees(), -40.444);
    UT_CHECK_EQUAL(camera3.getWorldPosition(), math::Vec3(-44, 55, -66));
    UT_CHECK_EQUAL_FLOATS(camera3.getLookDirection().x, math::Vec3(-1, -2222, -3).normalize().x);
    UT_CHECK_EQUAL_FLOATS(camera3.getLookDirection().y, math::Vec3(-1, -2222, -3).normalize().y);
    UT_CHECK_EQUAL_FLOATS(camera3.getLookDirection().z, math::Vec3(-1, -2222, -3).normalize().z);
    /**
     * @todo 2025/08/16 Ensure horizontal, vertical, and clockwise rotations are correct
     */
}

UT_FUNCTION(test_setPosition) {
    quartz::scene::Camera camera;
    UT_CHECK_EQUAL(camera.getWorldPosition(), math::Vec3(0, 0, 0));

    camera.setPosition({3, 3, 3});

    UT_CHECK_EQUAL(camera.getWorldPosition(), math::Vec3(3, 3, 3));
}

UT_FUNCTION(test_setRotation) {
    // Create a rotation quaternion
    // Set the camera's rotation
    // Ensure that the horizontal, vertical, and clockwise degrees are correct
 
    // Rotate 30 degrees horizontally
    {
        const math::Quaternion rotation = math::Quaternion::fromEulerAngles(30, 0, 0);

        const quartz::scene::Camera camera(70, math::Vec3(0,0,0), rotation);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 30);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);

        UT_CHECK_EQUAL(camera.getRotation().getDirectionVector(), rotation.getDirectionVector());
    }

    // Rotate 60 degrees up
    {
        const math::Quaternion rotation = math::Quaternion::fromEulerAngles(0, 60, 0);

        const quartz::scene::Camera camera(70, math::Vec3(0,0,0), rotation);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 60);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);

        UT_CHECK_EQUAL(camera.getRotation().getDirectionVector(), rotation.getDirectionVector());
    }

    // Rotate horizontally and up
    {
        const math::Quaternion rotation = math::Quaternion::fromEulerAngles(25, 50, 0);

        const quartz::scene::Camera camera(70, math::Vec3(0,0,0), rotation);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 25);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 50);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);

        UT_CHECK_EQUAL(camera.getRotation().getDirectionVector(), rotation.getDirectionVector());
    }
}

UT_FUNCTION(test_setRotationDegrees) {
    // Set the rotation
    // Ensure the horizontal, vertical, and clockwise rotations are correct
    // Ensure the look direction is correct

    // Rotate to the left 
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3::Forward));

        camera.setRotationDegrees(45, 0, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::slerp(
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            math::Quaternion::fromDirectionVector(math::Vec3::Left),
            0.5f
        );

        UT_CHECK_EQUAL(actualRotation, expectedRotation);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 45);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    // Rotate down
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3::Forward));

        camera.setRotationDegrees(0, 45, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::slerp(
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            math::Quaternion::fromDirectionVector(math::Vec3::Down),
            0.5f
        );

        UT_CHECK_EQUAL(actualRotation, expectedRotation);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 45);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    // Rotate to the right
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3::Forward));

        camera.setRotationDegrees(-45, 0, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::slerp(
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            math::Quaternion::fromDirectionVector(math::Vec3::Right),
            0.5f
        );

        UT_CHECK_EQUAL(actualRotation, expectedRotation);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), -45);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    // Rotate up 
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3::Forward));

        camera.setRotationDegrees(0, -45, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::slerp(
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            math::Quaternion::fromDirectionVector(math::Vec3::Up),
            0.5f
        );

        UT_CHECK_EQUAL(actualRotation, expectedRotation);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), -45);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    /**
     * @todo 2025/08/16 Create test for corner case when we try to set the rotation straight up or straight down
     */
}

UT_FUNCTION(test_rotateDegrees) {
    // Set the rotation
    // Ensure the horizontal, vertical, and clockwise rotations are correct
    // Ensure the look direction is correct

    /**
     * @todo 2025/08/16 Should a positive horizontal rotation really amount to a leftwards rotation????
     */

    // Rotate to the left, starting facing forward 
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3::Forward));

        camera.rotateDegrees(35, 0, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::slerp(
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            math::Quaternion::fromDirectionVector(math::Vec3::Left),
            35.0f / 90.0f
        );

        UT_CHECK_EQUAL(actualRotation, expectedRotation);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().x, expectedRotation.getDirectionVector().x);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().y, expectedRotation.getDirectionVector().y);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().z, expectedRotation.getDirectionVector().z);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 35);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    // Rotate to the right, starting facing forward and right
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3(1, 0, 1).normalize()));

        camera.rotateDegrees(-45, 0, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);

        UT_CHECK_EQUAL(actualRotation, expectedRotation);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().x, expectedRotation.getDirectionVector().x);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().y, expectedRotation.getDirectionVector().y);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().z, expectedRotation.getDirectionVector().z);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), -90);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    // Rotate down, starting facing forward
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3::Forward));

        camera.rotateDegrees(0, 30, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::slerp(
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            math::Quaternion::fromDirectionVector(math::Vec3::Down),
            30.0f / 90.0f
        );

        UT_CHECK_EQUAL(actualRotation, expectedRotation);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().x, expectedRotation.getDirectionVector().x);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().y, expectedRotation.getDirectionVector().y);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().z, expectedRotation.getDirectionVector().z);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), 30);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    // Rotate up
    {
        quartz::scene::Camera camera(65, {0, 8, 0}, math::Quaternion::fromDirectionVector(math::Vec3::Forward));

        camera.rotateDegrees(0, -40, 0);

        const math::Quaternion actualRotation = camera.getRotation();
        const math::Quaternion expectedRotation = math::Quaternion::slerp(
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            math::Quaternion::fromDirectionVector(math::Vec3::Up),
            40.0f / 90.0f
        );

        UT_CHECK_EQUAL(actualRotation, expectedRotation);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().x, expectedRotation.getDirectionVector().x);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().y, expectedRotation.getDirectionVector().y);
        UT_CHECK_EQUAL_FLOATS(actualRotation.getDirectionVector().z, expectedRotation.getDirectionVector().z);

        UT_CHECK_EQUAL_FLOATS(camera.getHorizontalRotationDegrees(), 0);
        UT_CHECK_EQUAL_FLOATS(camera.getVerticalRotationDegrees(), -40);
        UT_CHECK_EQUAL_FLOATS(camera.getClockwiseRotationDegrees(), 0);
    }

    /**
     * @todo 2025/08/16 Create test for complex cases (facing non-forwards before rotation, rotating on two axes at once)
     * @todo 2025/08/16 Create test for corner case when we try to set the rotation straight up or straight down
     */
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
    const quartz::scene::Camera camera(42.42, {7, 8, 9}, math::Quaternion::fromDirectionVector(math::Vec3(-4, -3, -2).normalize()));
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

