// ReadPars: Simple C++ library to read parameter text files

// Version 1.2

// Copyright (c) 2025-2026, Raphaël Scherrer
// This code is shared under the MIT License. You are free
// to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of it, subject to the terms of the license.
// You must retain the copyright notice and license text in copies 
// or substantial portions of the code. See LICENSE file for details.

// URL: https://github.com/rscherrer/readpars

// Source code of the ReadPars class.

#include "readpars.hpp"

// Constructor
ReadPars::ReadPars(const std::string &filename) : 
    filename(filename),
    file(std::ifstream()),
    count(0u),
    empty(false),
    comment(false),
    line(std::istringstream()),
    name("")
{

    // filename: name of the file to read

}

// Error messages
std::string ReadPars::errorOpenFile() const { return "Unable to open file " + filename; }
std::string ReadPars::errorEmptyFile() const { return "File " + filename + " is empty"; }
std::string ReadPars::errorReadName() const { return "Could not read parameter name in line " + std::to_string(count) + " of file " + filename; }
std::string ReadPars::errorNoValue() const { return "No value for parameter " + name + " in line " + std::to_string(count) + " of file " + filename; }
std::string ReadPars::errorReadValue() const { return "Could not read value for parameter " + name + " in line " + std::to_string(count) + " of file " + filename; }
std::string ReadPars::errorParseValue() const { return "Invalid value type for parameter " + name + " in line " + std::to_string(count) + " of file " + filename; }
std::string ReadPars::errorTooManyValues() const { return "Too many values for parameter " + name + " in line " + std::to_string(count) + " of file " + filename; }
std::string ReadPars::errorTooFewValues() const { return "Too few values for parameter " + name + " in line " + std::to_string(count) + " of file " + filename; }
std::string ReadPars::errorInvalidParameter() const { return "Invalid parameter: " + name + " in line " + std::to_string(count) + " of file " + filename; }

// Function to error on invalid parameter
void ReadPars::readerror() const {

    // Throw error
    throw std::runtime_error(errorInvalidParameter());

}

// Function to format error message
void ReadPars::checkerror(const std::string &error) const {

    // error: error message to format

    // Check if error is empty
    if (error.empty()) return;

    // Or format the error message
    std::string message = "Parameter " + name + " " + error + " in line " + std::to_string(count) + " of file " + filename;

    // And throw exception
    throw std::runtime_error(message);

}

// Function to open the file
void ReadPars::open() {

    // Open the file
    file.open(filename.c_str());

    // Check if the file is open
    if (!isopen())
        throw std::runtime_error(errorOpenFile());

    // Check if the file is empty
    if (iseof())
        throw std::runtime_error(errorEmptyFile());

    // Check
    assert(isopen());
    assert(!iseof());
    assert(count == 0u);

}

// Function to reset a line
void ReadPars::reset() {

    // Reset
    empty = false;
    comment = false;
    line.clear();
    line.str("");
    line.seekg(0, std::ios::beg);
    name.clear();

}

// Function to make sure the next thing can be read
bool ReadPars::readnext(std::istringstream &line, std::string &input) {

    // line: line to read from
    // input: string to read into

    // Read the parameter name
    line >> input;

    // Check if error
    bool error = line.fail();

    // If not...
    if (!error) {

        // For each character...
        for (char c : input) {

            // Make sure it is alphanumeric or a dot or a minus
            if (!std::isalnum(c) && c != '.' && c != '-') error = true;

            // Exit if needed
            if (error) break;

        }
    }

    // Return error code
    return !error;

}

// Function to read a line from the file
void ReadPars::readline() {

    // Check
    assert(isopen());
    assert(!iseof());

    // Reset
    reset();

    // Temporary container
    std::string temp;

    // Read the line
    std::getline(file, temp);

    // Check if the line is empty
    empty = temp.empty();

    // Check if the line is a comment
    comment = temp[0] == '#';

    // Convert the line into a stream
    line.str(temp);

    // Increment line count
    ++count;

    // If needed...
    if (empty || comment) return;

    // Error if needed
    if (!readnext(line, name))
        throw std::runtime_error(errorReadName());

    // Check that we are not at the end of the line
    if (iseol())
        throw std::runtime_error(errorNoValue());

}

// Function to close the input file
void ReadPars::close() {

    // Close
    file.close();

}
