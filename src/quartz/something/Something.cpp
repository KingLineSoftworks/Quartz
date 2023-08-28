#include "quartz/something/Something.hpp"

quartz::Something::Something(
    const uint32_t a,
    const double b
) :
    m_a(a),
    m_b(b)
{}

void quartz::Something::doSomething() const {
    LOG_FUNCTION_SCOPE_ERRORthis("");
    LOG_FUNCTION_CALL_TRACE(getLoggerRegistrationInfo(), "");
    LOG_FUNCTION_CALL_TRACE(getLoggerRegistrationInfo(), "value 1 : {} , value 2 : {}", m_a, m_b);
    LOG_FUNCTION_CALL_TRACEthis("empty");
    LOG_FUNCTION_CALL_TRACEthis("value 1 : {} , value 2 : {}", m_a, m_b);
    LOG_SCOPE_CHANGE_CRITICALthis();

    LOG_TRACEthis("Yo yo yo, we loggin info");

    LOG_TRACEthis("");

    LOG_TRACEthis("{}", "some burger bitch");

    LOG_TRACEthis("dire dire docks son {} {}", m_a, m_b);
}
