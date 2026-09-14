#pragma once

#include <stdexcept>

#define MYQL_REQUIRE(condition, message) \
    if (!(condition))                    \
        throw std::runtime_error(message)
