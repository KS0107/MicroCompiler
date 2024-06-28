#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

enum class TokenType {
    _exit,
    int_lit,
    semi,
};

struct Token {
    TokenType type;
    optional<string> value;
};

vector<Token> tokenise(const string& str) {
    vector<Token> tokens{};

    string buffer;

    for (int i = 0; i < str.length(); i++) {
        char c = str.at(i);

        if (isspace(c)) {
            continue;
        }
        if (isalpha(c)) {
            buffer.push_back(c);
            i++;
            while (isalnum(str.at(i))) {
                buffer.push_back(str.at(i));
                i++;
            }
            i--;
            if (buffer == "exit") {
                tokens.push_back(Token{TokenType::_exit, nullopt});
                buffer.clear();
                continue;
            } else {
                cerr << "Unknown token: " << buffer << endl;
                exit(EXIT_FAILURE);
            }
        } else if (isdigit(c)) {
            buffer.push_back(c);
            i++;
            while ((isdigit(str.at(i)))) {
                buffer.push_back(str.at(i));
                i++;  
            }
            i--;
            tokens.push_back(Token{TokenType::int_lit, buffer});
            buffer.clear();
        } else if (c == ';') {
            tokens.push_back(Token{TokenType::semi, nullopt});
        } 
        else {
            cerr << "Unknown token: " << c << endl;
            exit(EXIT_FAILURE);
        }


    }
    return tokens;
    
}

string tokens_to_asm(vector<Token>& tokens) {
    stringstream out;
    out << ".global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::_exit) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    out << "   mov x0, #" << tokens.at(i + 1).value.value() << "\n";
                    out << "   mov x16, #1\n";
                    out << "   svc #0";

                }
            }
        }
    }

    return out.str();

}

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

    vector<Token> tokens = tokenise(contents);

    {
        fstream output_file("out.asm", ios::out);
        output_file << tokens_to_asm(tokens);
        cout << "Output written to out.asm" << endl;
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