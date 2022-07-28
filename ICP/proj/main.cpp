// Project: ICP - Editor a interpret hierarchicky strukturovaných funkčních bloků
// Authors: Michal Cibák - xcibak00, FIT VUT
//          Tomáš Hrúz   - xhruzt00, FIT VUT
// Date: 15.4.2021


#include "funblocks.hpp"

#include <iostream>


int main()
{
    Library lib;
    Composite application{lib.applications["APP"]};
    try
    {
        application.generate();
    }
    catch(Composite::GenerationError& e)
    {
        std::cerr << e.error << '\n';
    }


    return 0;
}
