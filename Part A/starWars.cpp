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
    
    // Resize vector to number of generals if gen-eval mode is on.
    if (generalEvalMode == 1) {
        generals.resize(numGenerals);
    }
}

// Read in deployments one by one.
void starWars::readDeployment() {
    Deployment temp;
    char sith_jedi;
    string sJunk;
    char junk;
    stringstream ss;
    
    // PR input mode is on.
    if (inputMode == 'P') {
        uint32_t numSeed = 0;
        uint32_t numDeployments = 0;
        uint32_t arrivalRate = 0;

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
    
    // Read in deployment information.
    while (inputStream >> temp.timeStamp >> sith_jedi >> sJunk >> junk >> temp.generalID >> junk >>
           temp.planetID >> junk >> temp.forceSensitivity >> junk >> temp.numTroops) {
        
        // Check for errors if DL input mode is on.
        if (inputMode == 'D') {
            inputErrorCheckDL(temp);
        }
        
        // Set the currentdeployment's unique ID.
        temp.uniqueID = IDlist;
        
        // If new deployment timestamp is not the current timestamp
        if (temp.timeStamp != currentTimeStamp) {
            // Print median information if median mode is on.
            if (medianMode == 1) {
                medianOutput();
            }
            // Set the current timestamp to the new deployment timestamp.
            currentTimeStamp = temp.timeStamp;
        }
        
        // Process the read in deployment to the respective queue.
        processDeployment(sith_jedi, temp);
        
        // If general-eval mode is on, process the number of troops deployed for the general.
        if (generalEvalMode == 1) {
            processGeneralTroops(sith_jedi, temp);
        }
        
        // If watcher mode is on, process the attack and ambush information.
        if (watcherMode == 1) {
            planets[temp.planetID].attack.processAttackWatch(sith_jedi, temp);
            planets[temp.planetID].ambush.processAmbushWatch(sith_jedi, temp);
        }
        
        // While fights can occur, instigate fights after each read in deployment.
        while (checkFight(temp.planetID)) {
            instigateFight(temp.planetID);
        }
    }
    
    // Output median information again if median mode is turned on.
    if (medianMode == 1) {
        medianOutput();
    }
}

// Checks for errors when reading in using DL input mode.
void starWars::inputErrorCheckDL(const Deployment& temp) {
    if (static_cast<int>(temp.generalID) < 0 || temp.generalID >= numGenerals) {
        cerr << "Invalid general ID\n";
        exit(1);
    }
    if (static_cast<int>(temp.planetID) < 0 || temp.planetID >= numPlanets) {
        cerr << "Invalid planet ID\n";
        exit(1);
    }
    if (temp.forceSensitivity <= 0) {
        cerr << "Invalid force sensntivity level\n";
        exit(1);
    }
    if (temp.numTroops <= 0) {
        cerr << "Invalid number of troops\n";
        exit(1);
    }
    if (temp.timeStamp < currentTimeStamp) {
        cerr << "Invalid decreasing timestamp\n";
        exit(1);
    }
}

// Process the current deployment into their respective planet.
void starWars::processDeployment(const char sith_jedi, const Deployment& temp) {
    // Set current timestamp. (For error checking)
    currentTimeStamp = temp.timeStamp;
    
    // If battalion is a sith, push into sith queue.
    if (sith_jedi == 'S') {
        planets[temp.planetID].theSiths.push(temp);
    }
    // If battalion is a jedi, push into jedi queue.
    else {
        planets[temp.planetID].theJedis.push(temp);
    }
    
    // Increment uniqueID for next deployment.
    ++IDlist;
}

// Check if a fight will occur at the current planet.
bool starWars::checkFight(const uint32_t currentPlanet) {
    // No fight if there are no jedi or sith deployments on a planet.
    if (planets[currentPlanet].theJedis.size() == 0 || planets[currentPlanet].theSiths.size() == 0) {
        return 0;
    }
    
    // If there is a jedi with a force sensitivity lower or equal to that of a sith, a fight will occur.
    else if (planets[currentPlanet].theJedis.top().forceSensitivity <= planets[currentPlanet].theSiths.top().forceSensitivity) {
        return 1;
    }
    // No fight will occur since there is no compatibile jedi/sith pair.
    else {
        return 0;
    }
}

// Instigate a fight on the current planet.
void starWars::instigateFight(const uint32_t currentPlanet) {
    int numTroopsLost = 0;
    
    // If the Jedi deployment has more troops than the Sith deployment, Sith will get wiped out.
    if (planets[currentPlanet].theJedis.top().numTroops > planets[currentPlanet].theSiths.top().numTroops) {
        // The Jedi deployment will reduce in numbers by Sith troop numbers.
        planets[currentPlanet].theJedis.top().numTroops -= planets[currentPlanet].theSiths.top().numTroops;
        // Set number of troops lost to 2 times the Sith number of troops that died.
        numTroopsLost = 2 * planets[currentPlanet].theSiths.top().numTroops;
        
        // If verbose mode is on, output battle information after.
        if (verboseMode) {
            verboseOutput(currentPlanet, numTroopsLost);
        }
        
        // Sith deployment gets wiped out and removed from queue.
        planets[currentPlanet].theSiths.pop();
    }
    // If the Sith deployment has more troops than the Jedi deployment, Jedi will get wiped out.
    else if (planets[currentPlanet].theJedis.top().numTroops < planets[currentPlanet].theSiths.top().numTroops) {
        // The Sith deployment will reduce in numbers by Jedi troop numbers.
        planets[currentPlanet].theSiths.top().numTroops -= planets[currentPlanet].theJedis.top().numTroops;
        // Set number of troops lost to 2 times the Jedi number of troops that died.
        numTroopsLost = 2 * planets[currentPlanet].theJedis.top().numTroops;
        
        // If verbose mode is on, output battle information after.
        if (verboseMode) {
            verboseOutput(currentPlanet, numTroopsLost);
        }
        
        // Jedi deployment gets wiped out and removed from queue.
        planets[currentPlanet].theJedis.pop();
    }
    // If Sith deployment and Jedi deployment have equal number of troops.
    else {
        // Set number of troops lost to 2 times the number of troops.
        numTroopsLost = 2 * planets[currentPlanet].theSiths.top().numTroops;
        
        // If verbose mode is on, output battle information after.
        if (verboseMode) {
            verboseOutput(currentPlanet, numTroopsLost);
        }
        
        // Both deployments get wiped out and removed from queue.
        planets[currentPlanet].theSiths.pop();
        planets[currentPlanet].theJedis.pop();
    }
    
    // If median mode is on, process the information for median.
    if (medianMode == 1) {
        processMedian(currentPlanet, numTroopsLost);
    }
    
    // Increment the number of battles that have occurred.
    ++numBattles;
}

// Print information for a battle that has occured.
void starWars::verboseOutput(const uint32_t currentPlanet, const int numTroopsLost) {
    cout << "General " << planets[currentPlanet].theSiths.top().generalID <<
    "'s battalion attacked General " << planets[currentPlanet].theJedis.top().generalID
    << "'s battalion on planet " << currentPlanet << ". " << numTroopsLost << " troops were lost.\n";
}

// Process the median information.
void starWars::processMedian(const uint32_t currentPlanet, int numTroopsLost) {
    if (planets[currentPlanet].lowerHalf.size() > planets[currentPlanet].upperHalf.size()) {
       
        if (numTroopsLost < (planets[currentPlanet].median)) {
            planets[currentPlanet].upperHalf.push(planets[currentPlanet].lowerHalf.top());
            planets[currentPlanet].lowerHalf.pop();
            planets[currentPlanet].lowerHalf.push(numTroopsLost);
        }
        else {
            planets[currentPlanet].upperHalf.push(numTroopsLost);
        }
        
        planets[currentPlanet].median = (planets[currentPlanet].lowerHalf.top() + planets[currentPlanet].upperHalf.top()) / 2;
    }
    
    else if (planets[currentPlanet].lowerHalf.size() == planets[currentPlanet].upperHalf.size()) {
        
        if (numTroopsLost < planets[currentPlanet].median) {
            planets[currentPlanet].lowerHalf.push(numTroopsLost);
            planets[currentPlanet].median = planets[currentPlanet].lowerHalf.top();
        }
        else {
            planets[currentPlanet].upperHalf.push(numTroopsLost);
            planets[currentPlanet].median = planets[currentPlanet].upperHalf.top();
        }
    }
    
    else {
        
        if (numTroopsLost > (planets[currentPlanet].median)) {
            planets[currentPlanet].lowerHalf.push(planets[currentPlanet].upperHalf.top());
            planets[currentPlanet].upperHalf.pop();
            planets[currentPlanet].upperHalf.push(numTroopsLost);
        }
        
        else {
            planets[currentPlanet].lowerHalf.push(numTroopsLost);
        }
        
        planets[currentPlanet].median = (planets[currentPlanet].lowerHalf.top() + planets[currentPlanet].upperHalf.top()) / 2;
    }
}

// Print the median troops lost for each planet, only if battles have occurred.
void starWars::medianOutput() {
    // For each planet.
    for (size_t i = 0; i < planets.size(); ++i) {
        // Battles have occurred
        if (planets[i].median != 0) {
            cout << "Median troops lost on planet " << i << " at time " <<
            currentTimeStamp << " is " << planets[i].median << ".\n";
        }
    }
}

// Process the number of troops deployed for each general per deployment.
void starWars::processGeneralTroops(const char sith_jedi, const Deployment& temp) {
    // Record how many troops deployed for Sith and Jedi respectively.
    if (sith_jedi == 'S') {
        generals[temp.generalID].numSith += temp.numTroops;
    }
    else {
        generals[temp.generalID].numJedi += temp.numTroops;
    }
}

// Process the remaining number of troops that survived for each general.
void starWars::processSurvivors() {
    // For each planet.
    for (uint32_t i = 0; i < numPlanets; ++i) {
        
        // While there are still Sith deployments with troops left.
        while (!planets[i].theSiths.empty()) {
            // Add the remaining Sith troops that are left.
            generals[planets[i].theSiths.top().generalID].numSurvivors += planets[i].theSiths.top().numTroops;
            // Remove the deployment.
            planets[i].theSiths.pop();
        }
        // While there are still Jedi deployments with troops left.
        while (!planets[i].theJedis.empty()) {
            // Add the remaining Jedi troops left in the deployment.
            generals[planets[i].theJedis.top().generalID].numSurvivors += planets[i].theJedis.top().numTroops;
            // Remove the deployment.
            planets[i].theJedis.pop();
        }
    }
}

// Print the general evaluation information.
void starWars::generalEvalOutput() {
    cout << "---General Evaluation---\n";
    for (uint32_t i = 0; i < numGenerals; ++i) {
        cout << "General " << i << " deployed " << generals[i].numJedi <<
        " Jedi troops and " << generals[i].numSith << " Sith troops, and "
        << generals[i].numSurvivors << "/" << generals[i].numJedi + generals[i].numSith
        << " troops survived.\n";
    }
}

// For each new deployment read, keep track of information for most exciting attack.
void starWars::Planet::attackWatch::processAttackWatch(const char sith_jedi, const Deployment& temp) {
    // Initial State
    if (attackState == State::Initial) {
        // If a Jedi is seen, record its force and time and change state.
        if (sith_jedi == 'J') {
            bestJediForce = temp.forceSensitivity;
            bestJediTime = temp.timeStamp;
            attackState = State::SeenOne;
        }
    }
    // SeenOne State
    else if (attackState == State::SeenOne) {
        // If another Jedi is seen and its force is lower, record its information.
        if (sith_jedi == 'J') {
            if (temp.forceSensitivity < bestJediForce) {
                bestJediForce = temp.forceSensitivity;
                bestJediTime = temp.timeStamp;
            }
        }
        // If a Sith is seen, record it's force and time and change state.
        else {
            bestSithForce = temp.forceSensitivity;
            bestSithTime = temp.timeStamp;
            attackState = State::SeenBoth;
        }
    }
    // SeenBoth State
    else if (attackState == State::SeenBoth) {
        // If another Sith is seen and its force is higher, record its information.
        if (sith_jedi == 'S') {
            if (temp.forceSensitivity > bestSithForce) {
                bestSithForce = temp.forceSensitivity;
                bestSithTime = temp.timeStamp;
            }
        }
        // If another Jedi is seen and its force is lower, record its information for a
        // potential better fight and change state.
        else {
            if (temp.forceSensitivity < bestJediForce) {
                maybeBetterForce = temp.forceSensitivity;
                maybeBetterTime = temp.timeStamp;
                attackState = State::MaybeBetter;
            }
        }
    }
    // maybeBetter State
    else {
        // If another Sith is seen where it would result in a fight with greater force difference,
        // record its information, set the Jedi information to the potential one, and revert state.
        if (sith_jedi == 'S') {
            if (bestSithForce - bestJediForce < temp.forceSensitivity - maybeBetterForce) {
                
                bestSithForce = temp.forceSensitivity;
                bestSithTime = temp.timeStamp;
                
                bestJediForce = maybeBetterForce;
                bestJediTime = maybeBetterTime;
                
                attackState = State::SeenBoth;
            }
        }
        // If ANOTHER Jedi is seen and it's force sensitivity is lower, update the potential.
        else {
            if (temp.forceSensitivity < maybeBetterForce) {
                maybeBetterForce = temp.forceSensitivity;
                maybeBetterTime = temp.timeStamp;
            }
        }
    }
    
}

// For each new deployment read, keep track of information for most exciting ambush.
void starWars::Planet::ambushWatch::processAmbushWatch(const char sith_jedi, const Deployment& temp) {
    // Initial State
    if (ambushState == State::Initial) {
        // If a Sith is seen, record its force and time and change state.
        if (sith_jedi == 'S') {
            bestSithForce = temp.forceSensitivity;
            bestSithTime = temp.timeStamp;
            ambushState = State::SeenOne;
        }
    }
    // SeenOne State
    else if (ambushState == State::SeenOne) {
        // If another Sith is seen and its force is higher, record its information.
        if (sith_jedi == 'S') {
            if (temp.forceSensitivity > bestSithForce) {
                bestSithForce = temp.forceSensitivity;
                bestSithTime = temp.timeStamp;
            }
        }
        // If a Jedi is seen, record it's force and time and change state.
        else {
            bestJediForce = temp.forceSensitivity;
            bestJediTime = temp.timeStamp;
            ambushState = State::SeenBoth;
        }
    }
    // SeenBoth State
    else if (ambushState == State::SeenBoth) {
        // If another Jedi is seen and its force is lower, record its information.
        if (sith_jedi == 'J') {
            if (temp.forceSensitivity < bestJediForce) {
                bestJediForce = temp.forceSensitivity;
                bestJediTime = temp.timeStamp;
            }
        }
        // If another Sith is seen and its force is higher, record its information for a
        // potential better fight and change state.
        else {
            if (temp.forceSensitivity > bestSithForce) {
                maybeBetterForce = temp.forceSensitivity;
                maybeBetterTime = temp.timeStamp;
                ambushState = State::MaybeBetter;
            }
        }
    }
    // maybeBetter State
    else {
        // If another Jedi is seen where it would result in a fight with greater force difference,
        // record its information, set the Sith information to the potential one, and revert state.
        if (sith_jedi == 'J') {
            if (bestSithForce - bestJediForce < maybeBetterForce - temp.forceSensitivity) {
                
                bestJediForce = temp.forceSensitivity;
                bestJediTime = temp.timeStamp;
                
                bestSithForce = maybeBetterForce;
                bestSithTime = maybeBetterTime;
                
                ambushState = State::SeenBoth;
            }
        }
        // If ANOTHER Sith is seen and it's force sensitivity is greater, update the potential.
        else {
            if (temp.forceSensitivity > maybeBetterForce) {
                maybeBetterForce = temp.forceSensitivity;
                maybeBetterTime = temp.timeStamp;
            }
        }
    }
}

// Print the watch information for the most exciting attack.
void starWars::Planet::attackWatch::attackWatchOutput(const uint32_t planet) {
    // If there are no exciting battles, print with timestamp -1. CHERKKEKRKE HERERERAIEREAIRJEAIFJAEGLEAGEALGNEALKNBEG
    if (attackState == State::Initial || attackState == State::SeenOne || bestJediForce > bestSithForce) {
        cout << "A movie watcher would enjoy an attack on planet " << planet << " with Jedi at time -1 and Sith at time -1.\n";
    }
    else {
        cout << "A movie watcher would enjoy an attack on planet " << planet << " with Jedi at time " << bestJediTime <<
        " and Sith at time " << bestSithTime << ".\n";
    }
}

// Print the watch information for the most exciting ambush.
void starWars::Planet::ambushWatch::ambushWatchOutput(const uint32_t planet) {
    // If there are no exciting battles, print with timestamp -1.
    if (ambushState == State::Initial || ambushState == State::SeenOne || bestJediForce > bestSithForce) {
        cout << "A movie watcher would enjoy an ambush on planet " << planet << " with Sith at time -1 and Jedi at time -1.\n";
    }
    else {
        cout << "A movie watcher would enjoy an ambush on planet " << planet << " with Sith at time " << bestSithTime <<
        " and Jedi at time " << bestJediTime << ".\n";
    }
}

void starWars::simulateStarWars(int argc, char** argv) {
    getOptions(argc, argv);
    
    cout << "Deploying troops...\n";
    
    readOverviewInput();
    
    readDeployment();
    
    // Print number of battles.
    cout << "---End of Day---\n" << "Battles: " << numBattles << "\n";
    
    // If general eval mode is on, print general evaluation.
    if (generalEvalMode == 1) {
        processSurvivors();
        generalEvalOutput();
    }
    
    // If watcher mode is on, print watch information.
    if (watcherMode == 1) {
        cout << "---Movie Watcher---\n";
        for (uint32_t i = 0; i < numPlanets; ++i) {
            planets[i].ambush.ambushWatchOutput(i);
            planets[i].attack.attackWatchOutput(i);
        }
    }
}
