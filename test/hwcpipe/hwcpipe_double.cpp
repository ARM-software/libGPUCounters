/*
 * Copyright (c) 2023 Arm Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#include "hwcpipe/detail/internal_types.hpp"

#include <catch2/catch.hpp>

namespace hwcpipe {
namespace detail {

TEST_CASE("Test hwcpipe_double zero division") {

    SECTION("test 0/0 division") {
        detail::hwcpipe_double a{0.0};
        detail::hwcpipe_double b{0.0};

        double result = a / b;

        REQUIRE(result == 0.0);
    }

    SECTION("test >0/0 division") {
        detail::hwcpipe_double a{99.99};
        detail::hwcpipe_double b{0.0};

        double result = a / b;

        REQUIRE(std::isinf(result));
    }

    SECTION("test 0/>0 division") {
        detail::hwcpipe_double a{0.0};
        detail::hwcpipe_double b{99.99};

        double result = a / b;

        REQUIRE(result == 0.0);
    }
}

TEST_CASE("Test hwcpipe_double min and max") {

    SECTION("test max") {
        detail::hwcpipe_double max = std::max({5.4, 17.8, 23.6, 999999.99, 46.9, 32.1});
        detail::hwcpipe_double expected = 999999.99;

        REQUIRE(max == expected);
    }

    SECTION("test min") {
        detail::hwcpipe_double min = std::min({5.4, 17.8, 23.6, 999999.99, 46.9, 32.1});
        detail::hwcpipe_double expected = 5.4;

        REQUIRE(min == expected);
    }
}

TEST_CASE("Test hwcpipe_double addition") {

    SECTION("test add hwcpipe_double") {
        detail::hwcpipe_double a{5.3};
        detail::hwcpipe_double b{10.4};

        double result = a + b;

        REQUIRE(result == 15.7);
    }

    SECTION("test add double") {
        detail::hwcpipe_double a{9.9};
        double b{1.2};

        double result = a + b;

        REQUIRE(result == 11.1);
    }

    SECTION("test add int") {
        detail::hwcpipe_double a{99.99};
        int b{100};

        double result = a + b;

        REQUIRE(result == 199.99);
    }
}

TEST_CASE("Test hwcpipe_double subtraction") {

    SECTION("test sub hwcpipe_double") {
        detail::hwcpipe_double a{12.3};
        detail::hwcpipe_double b{2.2};

        double result = a - b;

        REQUIRE(result == Approx(10.1));
    }

    SECTION("test sub double") {
        detail::hwcpipe_double a{9.9};
        double b{1.2};

        double result = a - b;

        REQUIRE(result == Approx(8.7));
    }

    SECTION("test sub int") {
        detail::hwcpipe_double a{99.99};
        int b{10};

        double result = a - b;

        REQUIRE(result == 89.99);
    }
}

TEST_CASE("Test hwcpipe_double multiplication") {

    SECTION("test mult hwcpipe_double") {
        detail::hwcpipe_double a{12.5};
        detail::hwcpipe_double b{2.5};

        double result = a * b;

        REQUIRE(result == 31.25);
    }

    SECTION("test mult double") {
        detail::hwcpipe_double a{10.5};
        double b{4.5};

        double result = a * b;

        REQUIRE(result == 47.25);
    }

    SECTION("test mult int") {
        detail::hwcpipe_double a{99.99};
        int b{10};

        double result = a * b;

        REQUIRE(result == 999.9);
    }
}

TEST_CASE("Test hwcpipe_double division") {

    SECTION("test div hwcpipe_double") {
        detail::hwcpipe_double a{12.5};
        detail::hwcpipe_double b{2.5};

        double result = a / b;

        REQUIRE(result == 5.0);
    }

    SECTION("test div double") {
        detail::hwcpipe_double a{10.5};
        double b{4.5};

        double result = a / b;

        REQUIRE(result == Approx(2.33333333333));
    }

    SECTION("test div int") {
        detail::hwcpipe_double a{99.99};
        int b{10};

        double result = a / b;

        REQUIRE(result == Approx(9.999));
    }
}

} // namespace detail
} // namespace hwcpipe