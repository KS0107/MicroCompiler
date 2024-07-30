#include <iostream>
#include <fstream>
#include <sstream>
#include "arena.hpp"
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
        stringstream contents_stream;
        fstream input(argv[1], ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    } 

    Tokeniser tokeniser(std::move(contents));
    vector<Token> tokens = tokeniser.tokenise(contents);

    Parser parser(std::move(tokens));
    optional<NodeProg> prog = parser.parse_prog();

    if (!prog.has_value()) {
        cerr << "Invalid program" << endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());

    {
        fstream output_file("out.asm", ios::out);
        output_file << generator.gen_prog();
    }

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