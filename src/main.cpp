#include <iostream>
#include <fstream>
#include <sstream>
#include "tokenisation.hpp"
#include "parser.hpp"
#include "generation.hpp"
using namespace std;



int main(int argc, char *argv[]) {

    if (argc != 2) {
        cerr << "Incorrect number of arguments" << endl;
        exit(EXIT_FAILURE);
    }
    
    string contents ;
    {
        // Get file contents into a string
        stringstream contents_stream;
        fstream input(argv[1], ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }

    Tokeniser tokeniser(std::move(contents));
    vector<Token> tokens = tokeniser.tokenise(contents);

    Parser parser(std::move(tokens));
    optional<NodeExit> tree = parser.parse();

    if (!tree.has_value()) {
        cerr << "No exit statement found" << endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());

    {
        fstream output_file("out.asm", ios::out);
        output_file << generator.generate();
    }

    // Tests to ensure that the file is read and converted to assembly correctly
    int ret = system("as -o out.o out.asm");
    if (WIFEXITED(ret) && WEXITSTATUS(ret) != 0) {
        cerr << "Assembly failed with exit status: " << WEXITSTATUS(ret) << endl;
        exit(EXIT_FAILURE);
    }

    ret = system("ld -macos_version_min 14.0 -e _start -o out out.o");
    if (WIFEXITED(ret) && WEXITSTATUS(ret) != 0) {
        cerr << "Linking failed with exit status: " << WEXITSTATUS(ret) << endl;
        exit(EXIT_FAILURE);
    }

    ret = system("./out");
    if (WIFEXITED(ret)) {
        cout << "Program exited with status: " << WEXITSTATUS(ret) << endl;
    } else {
        cerr << "Program did not exit normally" << endl;
    }
    exit(EXIT_SUCCESS);
}