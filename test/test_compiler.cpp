#include <gtest/gtest.h>
#include <cstdio>    // For popen, pclose
#include <memory>    // For std::unique_ptr
#include <array>
#include <stdexcept>
#include <string>
#include <iostream>

// Function to execute a command and get its output
std::string execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::string fullCmd = cmd + " 2>&1"; // Redirect stderr to stdout
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(fullCmd.c_str(), "r"), pclose);
    
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}


std::string runCompilerWithFile(const std::string& filePath) {
    std::string execCommandStr = "./build/microcompiler " + filePath;
    std::string execOutput = execCommand(execCommandStr);
    return execOutput;
}

TEST(MicroCompilerTests, MissingBracket) {
    std::string output = runCompilerWithFile("./test_inputs/no_brackets.micro");
    std::string expected_output = "Invalid scope\n";
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, MissingSemicolon) {
    std::string output = runCompilerWithFile("./test_inputs/no_semicolon.micro");
    std::string expected_output = "[Parser Error] Expected ';' on line 1\n";
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, MissingDeclaration) {
    std::string output = runCompilerWithFile("./test_inputs/no_declaration.micro");
    std::string expected_output = "Identifier has not been declared: y\n";
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, AdditionSubtractionChained) {
    std::string output = runCompilerWithFile("./test_inputs/test_addition_subtraction.micro");
    std::string expected_output = "Program exited with status: 12\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, MultiplicationDivisionChained) {
    std::string output = runCompilerWithFile("./test_inputs/test_multiplication_division.micro");
    std::string expected_output = "Program exited with status: 76\n";   // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, CombinedOperationsChained) {
    std::string output = runCompilerWithFile("./test_inputs/test_combined_operations.micro");
    std::string expected_output = "Program exited with status: 31\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, ConditionalLogicChained) {
    std::string output = runCompilerWithFile("./test_inputs/test_conditional_elif.micro");
    std::string expected_output = "Program exited with status: 16\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, AddSubtractExtended) {
    std::string output = runCompilerWithFile("./test_inputs/test_addition_subtraction_chained.micro");
    std::string expected_output = "Program exited with status: 27\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, ConditionalElseSimple) {
    std::string output = runCompilerWithFile("./test_inputs/test_conditional_else_simple.micro");
    std::string expected_output = "Program exited with status: 15\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, ConditionalNestedSimple) {
    std::string output = runCompilerWithFile("./test_inputs/test_conditional_nested_simple.micro");
    std::string expected_output = "Program exited with status: 6\n";   // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, ConditionalElseNestedSimple) {
    std::string output = runCompilerWithFile("./test_inputs/test_conditional_else_nested_simple.micro");
    std::string expected_output = "Program exited with status: 10\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, MultilineComment) {
    std::string output = runCompilerWithFile("./test_inputs/multiline_comment.micro");
    std::string expected_output = "Program exited with status: 21\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, VarReassignment) {
    std::string output = runCompilerWithFile("./test_inputs/variable_reassignment.micro");
    std::string expected_output = "Program exited with status: 4\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, MultilevelElif) {
    std::string output = runCompilerWithFile("./test_inputs/test_multilevel_elif.micro");
    std::string expected_output = "Program exited with status: 7\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, Pemdas) {
    std::string output = runCompilerWithFile("./test_inputs/test_complex_pemdas.micro");
    std::string expected_output = "Program exited with status: 46\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

TEST(MicroCompilerTests, Undeclared) {
    std::string output = runCompilerWithFile("./test_inputs/undeclare_var.micro");
    std::string expected_output = "Identifier has not been declared: y\n";  // Based on the calculations in the file
    EXPECT_EQ(output, expected_output);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
