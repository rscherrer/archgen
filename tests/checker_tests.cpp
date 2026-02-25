#define BOOST_TEST_DYNAMIC_LINK
#define BOOST_TEST_MODULE Main

// Here we test the checking functions.

#include "testutils.hpp"
#include "../src/checker.hpp"
#include <boost/test/unit_test.hpp>

// Test the proportion checking function
BOOST_AUTO_TEST_CASE(isProportion) {

    // Known values
    BOOST_CHECK_EQUAL(chk::proportion(0.0), "");
    BOOST_CHECK_EQUAL(chk::proportion(0.5), "");
    BOOST_CHECK_EQUAL(chk::proportion(1.0), "");
    BOOST_CHECK_EQUAL(chk::proportion(-0.5), "must be between 0 and 1");
    BOOST_CHECK_EQUAL(chk::proportion(1.5), "must be between 0 and 1");

}

// Test the positive checking function
BOOST_AUTO_TEST_CASE(isPositive) {

    // Known values
    BOOST_CHECK_EQUAL(chk::positive(0.0), "");
    BOOST_CHECK_EQUAL(chk::positive(1.0), "");
    BOOST_CHECK_EQUAL(chk::positive(-1.0), "must be positive");
    BOOST_CHECK_EQUAL(chk::positive(-0.5), "must be positive");

}

// Test the strict positive checking function
BOOST_AUTO_TEST_CASE(isStrictPositive) {

    // Known values
    BOOST_CHECK_EQUAL(chk::strictpos(0.0), "must be strictly positive");
    BOOST_CHECK_EQUAL(chk::strictpos(1.0), "");
    BOOST_CHECK_EQUAL(chk::strictpos(-1.0), "must be strictly positive");
    BOOST_CHECK_EQUAL(chk::strictpos(-0.5), "must be strictly positive");
    BOOST_CHECK_EQUAL(chk::strictpos(1u), "");
    BOOST_CHECK_EQUAL(chk::strictpos(0u), "must be strictly positive");

}

// Test the zero to three checking function
BOOST_AUTO_TEST_CASE(isZeroToThree) {

    // Known values
    BOOST_CHECK_EQUAL(chk::zerotothree(0u), "");
    BOOST_CHECK_EQUAL(chk::zerotothree(1u), "");
    BOOST_CHECK_EQUAL(chk::zerotothree(2u), "");
    BOOST_CHECK_EQUAL(chk::zerotothree(3u), "");
    BOOST_CHECK_EQUAL(chk::zerotothree(4u), "must be between 0 and 3");
    BOOST_CHECK_EQUAL(chk::zerotothree(5u), "must be between 0 and 3");
    BOOST_CHECK_EQUAL(chk::zerotothree(-1), "must be between 0 and 3");

}