#ifndef TOKENGENERATOR_H
#define TOKENGENERATOR_H

#include <string>

using namespace std;

class TokenGenerator {
private:
    static int tokenCounter;
    
public:
    static string generateTokenId();
    static string generateRequestId();
};

#endif