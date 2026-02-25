// Functions pertaining to the tst namespace.

#include "testutils.hpp"

// Function to read a binary data file
std::vector<double> tst::read(const std::string &filename)
{

    // filename: the name of the file to read

    // Open the input file
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

    // Check if the file is open
    if (!file.is_open())
        throw std::runtime_error("Unable to open file " + filename);

    // Prepare storage for values
    double x;
    std::vector<double> v;

    // If the file is open
    if (file.is_open()) {

        // Loop through the file until we reach the end of the file
        while(file) {

            // Read elements
            file.read((char *) &x, sizeof(size_t));

            // Exit if reaching the end of the file
            if (!file.gcount()) break;

            // Store elements
            v.push_back(x);

        }
    }

    // Close the file
    file.close();

    return v;

}

// Function to read a text file into a string
std::string tst::readtext(const std::string &filename) {

    // filename: the name of the file

    // Open the input file
    std::ifstream file(filename.c_str(), std::ios::in);

    // Check if the file is open
    if (!file.is_open())
        throw std::runtime_error("Unable to open file " + filename);

    // Create a string stream to hold the file content
    std::stringstream content;

    // Read the file content into the string stream
    content << file.rdbuf();

    // Close the file
    file.close();

    // Return the content as a string
    return content.str();

}

// Function to write a text file
void tst::write(const std::string &filename, const std::string &content)
{

    // filename: the name of the file to write
    // content: the content to write to the file

    // Open the output file
    std::ofstream file(filename);

    // Check if the file is open
    if (!file.is_open()) 
        throw std::runtime_error("Unable to open file " + filename);

    // Write the content to the file
    file << content;

    // Close the file
    file.close();

}

// Helper function to check for errors and error messages
void tst::checkError(const std::function<void()>& func, const std::string& expected) {

    // func: function to run
    // expected: expected error message

    // Prepare to catch an error
    bool error = false;
    std::string message = "";

    // Try to...
    try {

        // Run the function
        func();

    }
    catch (const std::runtime_error& err) {

        // Catch the error
        error = true;
        message = err.what();

    }
    
    // Check that an error occurred
    BOOST_CHECK(error);

    // Check that the error message is the expected one
    BOOST_CHECK_EQUAL(message, expected);

}

// Function to capture screen output
std::string tst::captureOutput(const std::function<void()>& func) {

    // func: function to run

    // Redirect output
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    // Execute function
    func();

    // Restore
    std::cout.rdbuf(old);

    // Captured output
    std::string output = oss.str();

    // Normalize line breaks
    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());

    // Return
    return output;

}

// Helper function to test screen output
void tst::checkOutput(const std::function<void()>& func, const std::string& expected) {

    // func: function to run
    // expected: expected error message

    const std::string output = tst::captureOutput([&] { func(); });

    // Is it as expected?
    BOOST_CHECK_EQUAL(output, expected);

}