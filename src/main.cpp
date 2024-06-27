#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

enum class TokenType {
    _return,
    int_lit,
    semi,
};

struct Token {
    TokenType type;
    optional<string> value;
};

vector<Token> tokenise(const string& str) {
    for (int i = 0; i < str.length(); i++) {
        // TODO: WRite function to tokenise the input string
        
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        cerr << "Incorrect number of arguments" << endl;
        return EXIT_FAILURE;
    }
    
    string contents ;
    {
        // Get file contents into a string
        stringstream contents_stream;
        fstream input(argv[1], ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }
    cout << contents << endl;
    return EXIT_SUCCESS;
}