//  AD48FB4835AF347EB0CA8009E24C3B13F8519882
//  galaxy.cpp
//  p2-galaxy
//
//  Created by James Zhang on 2/21/21.
//

#include "starWars.h"
#include "xcode_redirect.hpp"

// ----------------------------------------------------------------------------
//                               Driver
// ----------------------------------------------------------------------------

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false); // Speeds up I/O.
    xcode_redirect(argc, argv); // Xcode redirect.
   
    starWars omg;
    omg.simulateStarWars(argc, argv);
    return 0;
}
