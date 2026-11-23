
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
    std::cout << "Program is about to exit." << std::endl;

    return 0;
}


