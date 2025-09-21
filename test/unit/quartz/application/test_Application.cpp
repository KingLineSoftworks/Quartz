#include "util/unit_test/UnitTest.hpp"

#include "quartz/application/Application.hpp"

namespace quartz {
namespace unit_test {

class ApplicationUnitTestClient {
public:
    static void determineSceneDebugMode(
        quartz::Application& application,
        const bool shouldToggleSceneDebugMode,
        const bool shouldToggleWireframeDoodadMode,
        const bool shouldToggleWireframeColliderMode
    ) {
        application.determineSceneDebugMode(
            shouldToggleSceneDebugMode,
            shouldToggleWireframeDoodadMode,
            shouldToggleWireframeColliderMode
        );
    }

private:
    ApplicationUnitTestClient() = delete;
};

} // namespace unit_test
} // namespace quartz

UT_FUNCTION(test_determineSceneDebugMode) {
    quartz::Application application(
        "test_determineSceneDebugMode",
        0,
        0,
        0,
        100,
        100,
        false,
        {}
    );

    // Don't toggle anything
    quartz::unit_test::ApplicationUnitTestClient::determineSceneDebugMode(application, false, false, false);
    UT_CHECK_FALSE(application.getSceneDebugMode());
    UT_CHECK_FALSE(application.getWireframeDoodadMode());
    UT_CHECK_FALSE(application.getWireframeColliderMode());

    // Toggle debug mode
    quartz::unit_test::ApplicationUnitTestClient::determineSceneDebugMode(application, true, false, false);
    UT_CHECK_TRUE(application.getSceneDebugMode());
    UT_CHECK_FALSE(application.getWireframeDoodadMode());
    UT_CHECK_FALSE(application.getWireframeColliderMode());

    // Toggle wire frame doodad mode
    quartz::unit_test::ApplicationUnitTestClient::determineSceneDebugMode(application, false, true, false);
    UT_CHECK_TRUE(application.getSceneDebugMode());
    UT_CHECK_TRUE(application.getWireframeDoodadMode());
    UT_CHECK_FALSE(application.getWireframeColliderMode());

    // Toggle both wire frame modes - doodad should be off, collider on
    quartz::unit_test::ApplicationUnitTestClient::determineSceneDebugMode(application, false, true, true);
    UT_CHECK_TRUE(application.getSceneDebugMode());
    UT_CHECK_FALSE(application.getWireframeDoodadMode());
    UT_CHECK_TRUE(application.getWireframeColliderMode());

    // Toggle wire frame collider mode - only debug mode should be on
    quartz::unit_test::ApplicationUnitTestClient::determineSceneDebugMode(application, false, false, true);
    UT_CHECK_TRUE(application.getSceneDebugMode());
    UT_CHECK_FALSE(application.getWireframeDoodadMode());
    UT_CHECK_FALSE(application.getWireframeColliderMode());

    // Toggle both wire frame modes - everything should be on now
    quartz::unit_test::ApplicationUnitTestClient::determineSceneDebugMode(application, false, true, true);
    UT_CHECK_TRUE(application.getSceneDebugMode());
    UT_CHECK_TRUE(application.getWireframeDoodadMode());
    UT_CHECK_TRUE(application.getWireframeColliderMode());

    // Toggle debug mode - so the other two should turn off
    quartz::unit_test::ApplicationUnitTestClient::determineSceneDebugMode(application, true, false, false);
    UT_CHECK_FALSE(application.getSceneDebugMode());
    UT_CHECK_FALSE(application.getWireframeDoodadMode());
    UT_CHECK_FALSE(application.getWireframeColliderMode());
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_determineSceneDebugMode);
    UT_RUN_TESTS();
}
