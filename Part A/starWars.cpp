// AD48FB4835AF347EB0CA8009E24C3B13F8519882

/*

    A utility file containing the member function definitions of the class starWars.

*/

#include "starWars.h"
#include <getopt.h>

// Read and process command line options.
void starWars::getOptions(int argc, char** argv) {

    int option_index = 0, option = 0;
    
    // Don't display getopt error messages about options.
    opterr = false;

    // Use getopt to find command line options.
    struct option longOpts[] = {{"verbose",      no_argument, nullptr, 'v' },
                                {"median",       no_argument, nullptr, 'm' },
                                {"general-eval", no_argument, nullptr, 'g' },
                                {"watcher",      no_argument, nullptr, 'w' },
                                {nullptr,        0,           nullptr, '\0'}};
    

    
    while ((option = getopt_long(argc, argv, "vmgw", longOpts, &option_index)) != -1) {
        switch (option) {
            case 'v':
                // Turns on verbose output mode.
                verboseMode = 1;
                break;

            case 'm':
                // Turns on median output mode.
                medianMode = 1;
                break;
                
            case 'g':
                // Turns on general-evaluation output mode.
                generalEvalMode = 1;
                break;
                
            case 'w':
                // Turns on movie-watcher output mode.
                watcherMode = 1;
                break;
        }
    }
    
}

// Read in the introductory input information.
void starWars::readOverviewInput() {
    string junk;
    
    getline(cin, junk);
    cin >> junk >> inputMode; // Read in input mode.
    getline(cin, junk);
    cin >> junk >> numGenerals; // Read in number of generals.
    getline(cin, junk);
    cin >> junk >> numPlanets; // Read in number of planets.
    
    planets.resize(numPlanets); // Resize vector to number of planets.
}

// Read in battalion using DL mode.
void starWars::readDeployment() {
    Deployment temp;
    char sith_jedi;
    string sJunk;
    char junk;
    stringstream ss; // THIS SHOULD BE PASSED IN
    
    // PR input mode is on.
    if (inputMode == 'P') {
        cin >> sJunk;
        cin >> numSeed;
        getline(cin, sJunk);
        cin >> sJunk;
        cin >> numDeployments;
        getline(cin, sJunk);
        cin >> sJunk;
        cin >> arrivalRate;
        
        // Generate deployments.
        P2random::PR_init(ss, numSeed, numGenerals, numPlanets, numDeployments, arrivalRate);
    }
    
    // Refer to ss if PR mode is on, refer to cin if DL mode is on.
    istream &inputStream = inputMode == 'P' ? ss : cin;
    
    // Read in battalion information.
    while (inputStream >> temp.timeStamp >> sith_jedi >> sJunk >> junk >> temp.generalID >> junk >>
           temp.planetID >> junk >> temp.forceSensitivity >> junk >> temp.numTroops) {
        
        // Check for errors if DL input mode is on.
        if (inputMode == 'D') {
            inputErrorCheckDL(temp);
        }
        
        temp.uniqueID = IDlist;
        processDeployment(sith_jedi, temp);
        while (checkFight()) {
            instigateFight();
        }
        
    }
    
}

void starWars::processDeployment(const char sith_jedi, const Deployment& temp) {
    // Set current timestamp. (For error checking)
    currentTimeStamp = temp.timeStamp;
    
    // If battalion is a sith, push into sith queue and note that current deployment is sith.
    if (sith_jedi == 'S') {
        planets[temp.planetID].theSiths.push(temp);
        currentSith = 1;
    }
    // If battalion is a jedi, push into jedi queue and note that current deployment is jedi.
    else {
        planets[temp.planetID].theJedis.push(temp);
        currentSith = 0;
    }
    
    // Set current battalion.
    currentDeployment = temp;
    ++IDlist;
}


void starWars::inputErrorCheckDL(const Deployment& temp) {
    if (temp.generalID < 0 || temp.generalID >= numGenerals) {
        cerr << "Invalid general ID\n";
        exit(1);
    }
    if (temp.planetID < 0 || temp.planetID >= numPlanets) {
        cerr << "Invalid planet ID\n";
        exit(1);
    }
    if (temp.forceSensitivity < 0) {
        cerr << "Invalid force sensntivity level\n";
        exit(1);
    }
    if (temp.numTroops < 0) {
        cerr << "Invalid number of troops\n";
        exit(1);
    }
    if (temp.timeStamp < currentTimeStamp) {
        cerr << "Invalid decreasing timestamp\n";
        exit(1);
    }
}


bool starWars::checkFight() {
    int currentPlanet = currentDeployment.planetID;
    
    // No fight if there are no jedi or sith deployments on a planet.
    if (planets[currentPlanet].theJedis.size() == 0 || planets[currentPlanet].theSiths.size() == 0) {
        return 0;
    }
    
    //is sith
    if (currentSith == 1) {
        if (planets[currentPlanet].theJedis.top().forceSensitivity <= currentDeployment.forceSensitivity) {
            return 1;
        }
    }
    //is jedi
    else {
        if (planets[currentPlanet].theSiths.top().forceSensitivity >= currentDeployment.forceSensitivity) {
            return 1;
        }
    }
    //this will cause problems for sure
    return 0;
}

void starWars::instigateFight() {
    int currentPlanet = currentDeployment.planetID;
    int numTroopsLost = 0;
    // MAKE SURE TO RESET NUM TROOPS LOST
    if (planets[currentPlanet].theJedis.top().numTroops > planets[currentPlanet].theSiths.top().numTroops) {
        planets[currentPlanet].theJedis.top().numTroops -= planets[currentPlanet].theSiths.top().numTroops;
        numTroopsLost = 2 * planets[currentPlanet].theJedis.top().numTroops;
        if (verboseMode) {
            verboseOutput(currentPlanet, numTroopsLost);
        }
        planets[currentPlanet].theSiths.pop();
    }
    else if (planets[currentPlanet].theJedis.top().numTroops < planets[currentPlanet].theSiths.top().numTroops) {
        planets[currentPlanet].theSiths.top().numTroops -= planets[currentPlanet].theJedis.top().numTroops;
        numTroopsLost = 2 * planets[currentPlanet].theSiths.top().numTroops;
        if (verboseMode) {
            verboseOutput(currentPlanet, numTroopsLost);
        }
        planets[currentPlanet].theJedis.pop();
    }
    else {
        numTroopsLost = 2 * planets[currentPlanet].theSiths.top().numTroops;
        if (verboseMode) {
            verboseOutput(currentPlanet, numTroopsLost);
        }
        planets[currentPlanet].theSiths.pop();
        planets[currentPlanet].theJedis.pop();
    }
    
}

void starWars::verboseOutput(const int currentPlanet, const int numTroopsLost) {
    cout << "General " << planets[currentPlanet].theSiths.top().generalID <<
    "'s battalion attacked General " << planets[currentPlanet].theJedis.top().generalID
    << "'s battalion on planet " << currentPlanet << ". " << numTroopsLost << " troops were lost.\n";
}

void starWars::simulateStarWars(int argc, char** argv) {
    getOptions(argc, argv);
    readOverviewInput();
    
    readDeployment();
    
}
