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
        if (temp.timeStamp != currentTimeStamp) {
            if (medianMode == 1) {
                medianOutput();
            }
            currentTimeStamp = temp.timeStamp;
        }
        processDeployment(sith_jedi, temp);
        
        if (generalEvalMode == 1) {
            processGeneralTroops(sith_jedi, temp);
        }
        
        if (watcherMode == 1) {
            planets[temp.planetID].attack.processAttackWatch(sith_jedi, temp);
            planets[temp.planetID].ambush.processAmbushWatch(sith_jedi, temp);
        }
        
        while (checkFight()) {
            instigateFight();
        }
        
    }
    if (medianMode == 1) {
        medianOutput();
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
    
    if (planets[currentPlanet].theJedis.top().forceSensitivity <= planets[currentPlanet].theSiths.top().forceSensitivity) {
        return 1;
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
        numTroopsLost = 2 * planets[currentPlanet].theSiths.top().numTroops;
        if (verboseMode) {
            verboseOutput(currentPlanet, numTroopsLost);
        }
        planets[currentPlanet].theSiths.pop();
    }
    else if (planets[currentPlanet].theJedis.top().numTroops < planets[currentPlanet].theSiths.top().numTroops) {
        planets[currentPlanet].theSiths.top().numTroops -= planets[currentPlanet].theJedis.top().numTroops;
        numTroopsLost = 2 * planets[currentPlanet].theJedis.top().numTroops;
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
    
    if (medianMode == 1) {
        processMedian(currentPlanet, numTroopsLost);
    }
    
    ++numBattles;
}

void starWars::verboseOutput(const int currentPlanet, const int numTroopsLost) {
    cout << "General " << planets[currentPlanet].theSiths.top().generalID <<
    "'s battalion attacked General " << planets[currentPlanet].theJedis.top().generalID
    << "'s battalion on planet " << currentPlanet << ". " << numTroopsLost << " troops were lost.\n";
}

void starWars::processMedian(const int currentPlanet, int numTroopsLost) {
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

void starWars::medianOutput() {
    for (size_t i = 0; i < planets.size(); ++i) {
        // Battles have occurred
        if (planets[i].median != 0) {
            cout << "Median troops lost on planet " << i << " at time " << currentTimeStamp << " is " << planets[i].median << ".\n";
        }
    }
}

void starWars::processGeneralTroops(const char sith_jedi, const Deployment& temp) {
    if (sith_jedi == 'S') {
        generals[temp.generalID].numSith += temp.numTroops;
    }
    else {
        generals[temp.generalID].numJedi += temp.numTroops;
    }
}

void starWars::processSurvivors() {
    for (size_t i = 0; i < numPlanets; ++i) {
        while (!planets[i].theSiths.empty()) {
            generals[planets[i].theSiths.top().generalID].numSurvivors += planets[i].theSiths.top().numTroops;
            planets[i].theSiths.pop();
        }
        while (!planets[i].theJedis.empty()) {
            generals[planets[i].theJedis.top().generalID].numSurvivors += planets[i].theJedis.top().numTroops;
            planets[i].theJedis.pop();
        }
    }
}

void starWars::generalEvalOutput() {
    cout << "---General Evaluation---\n";
    for (size_t i = 0; i < numGenerals; ++i) {
        cout << "General " << i << " deployed " << generals[i].numJedi << " Jedi troops and " << generals[i].numSith << " Sith troops, and "
        << generals[i].numSurvivors << "/" << generals[i].numJedi + generals[i].numSith << " troops survived.\n";
    }
}

void starWars::Planet::attackWatch::processAttackWatch(const char sith_jedi, const Deployment& temp) {
   
    if (attackState == State::Initial) {
        if (sith_jedi == 'J') {
            bestJediForce = temp.forceSensitivity;
            bestJediTime = temp.timeStamp;
            attackState = State::SeenOne;
            
        }
    }
    
    else if (attackState == State::SeenOne) {
        if (sith_jedi == 'J') {
            if (temp.forceSensitivity < bestJediForce) {
                bestJediForce = temp.forceSensitivity;
                bestJediTime = temp.timeStamp;
            }
        }
        
        else {
            bestSithForce = temp.forceSensitivity;
            bestSithTime = temp.timeStamp;
            attackState = State::SeenBoth;
        }
    }
    
    else if (attackState == State::SeenBoth) {
        if (sith_jedi == 'S') {
            if (temp.forceSensitivity > bestSithForce) {
                bestSithForce = temp.forceSensitivity;
                bestSithTime = temp.timeStamp;
            }
        }
        
        else {
            if (temp.forceSensitivity < bestJediForce) {
                maybeBetterForce = temp.forceSensitivity;
                maybeBetterTime = temp.timeStamp;
                attackState = State::MaybeBetter;
            }
        }
    }
    
    // maybe better jedi
    else {
        if (sith_jedi == 'S') {
            if (bestSithForce - bestJediForce < temp.forceSensitivity - maybeBetterForce) {
                
                bestSithForce = temp.forceSensitivity;
                bestSithTime = temp.timeStamp;
                
                bestJediForce = maybeBetterForce;
                bestJediTime = maybeBetterTime;
                
                attackState = State::SeenBoth;
                
            }
        }
    }
    
}

void starWars::Planet::ambushWatch::processAmbushWatch(const char sith_jedi, const Deployment& temp) {
    if (ambushState == State::Initial) {
        if (sith_jedi == 'S') {
            bestSithForce = temp.forceSensitivity;
            bestSithTime = temp.timeStamp;
            ambushState = State::SeenOne;
            
        }
    }
    
    else if (ambushState == State::SeenOne) {
        if (sith_jedi == 'S') {
            if (temp.forceSensitivity > bestSithForce) {
                bestSithForce = temp.forceSensitivity;
                bestSithTime = temp.timeStamp;
            }
        }
        
        else {
            bestJediForce = temp.forceSensitivity;
            bestJediTime = temp.timeStamp;
            ambushState = State::SeenBoth;
        }
    }
    
    else if (ambushState == State::SeenBoth) {
        if (sith_jedi == 'J') {
            if (temp.forceSensitivity < bestJediForce) {
                bestJediForce = temp.forceSensitivity;
                bestJediTime = temp.timeStamp;
            }
        }
        
        else {
            if (temp.forceSensitivity > bestSithForce) {
                maybeBetterForce = temp.forceSensitivity;
                maybeBetterTime = temp.timeStamp;
                ambushState = State::MaybeBetter;
            }
        }
    }
    
    // maybe better jedi
    else {
        if (sith_jedi == 'J') {
            if (bestSithForce - bestJediForce < maybeBetterForce - temp.forceSensitivity) {
                
                bestJediForce = temp.forceSensitivity;
                bestJediTime = temp.timeStamp;
                
                bestSithForce = maybeBetterForce;
                bestSithTime = maybeBetterTime;
                
                ambushState = State::SeenBoth;
                
            }
        }
    }
}

void starWars::Planet::attackWatch::attackWatchOutput(const int planet) {
    if (attackState == State::Initial || bestJediForce > bestSithForce) {
        cout << "A movie watcher would enjoy an attack on planet " << planet << " with Jedi at time -1 << and Sith at time -1.\n";
    }
    else {
        cout << "A movie watcher would enjoy an attack on planet " << planet << " with Jedi at time " << bestJediTime <<
        " and Sith at time " << bestSithTime << ".\n";
    }
}

void starWars::Planet::ambushWatch::ambushWatchOutput(const int planet) {
    if (ambushState == State::Initial || bestJediForce > bestSithForce) {
        cout << "A movie watcher would enjoy an ambush on planet " << planet << " with Sith at time -1 << and Jedi at time -1.\n";
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
    
    cout << "---End of Day---\n" << "Battles: " << numBattles << "\n";
    
    if (generalEvalMode == 1) {
        processSurvivors();
        generalEvalOutput();
    }
    
    if (watcherMode == 1) {
        cout << "---Movie Watcher---\n";
        for (int i = 0; i < numPlanets; ++i) {
            planets[i].ambush.ambushWatchOutput(i);
            planets[i].attack.attackWatchOutput(i);
        }
    }
}
