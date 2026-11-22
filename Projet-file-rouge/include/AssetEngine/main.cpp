#include <algorithm>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include <limits>
#include <iostream>
#include "Roundtrip.h"


typedef std::vector<std::byte> Bytes;

int main()
{

    Roundtrip roundtrip;
    roundtrip.verifyAll();
    std::cout << "Roundtrip verification completed successfully." << std::endl;

    roundtrip.testAssetFileWriter();
    roundtrip.testAssetFileRoundtrip();
    
    return 0;

}


