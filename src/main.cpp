#include "FNES.h"
//#include "DemoCpu6502.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <iostream>

int main() 
{
    std::cout << "Start" << std::endl;

    FNES fnes;
    fnes.run();

    return 0;
}