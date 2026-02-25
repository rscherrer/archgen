#ifndef archgen_TESTS_TESTUTILS_HPP
#define archgen_TESTS_TESTUTILS_HPP

// This header is for the tst (test) namespace, which contains functions used in unit tests.

#include <vector>
#include <string>
#include <functional>
#include <fstream>
#include <sstream>
#include <boost/test/unit_test.hpp>

namespace tst
{

    // Functions used in unit tests
    std::vector<double> read(const std::string&);
    std::string readtext(const std::string&);
    void write(const std::string&, const std::string&);
    void checkError(const std::function<void()>&, const std::string&);
    void checkOutput(const std::function<void()>&, const std::string&);
    std::string captureOutput(const std::function<void()>&);

}

#endif