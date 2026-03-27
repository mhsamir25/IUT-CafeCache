#ifndef TOKENGENERATOR_H
#define TOKENGENERATOR_H

#include <string>

using namespace std;

class TokenGenerator {
private:
    static int tokenCounter;
    static void saveCounter();
    
public:
    static void initializeCounter();
    static string generateTokenId();
    static string generateRequestId();
};

#endif