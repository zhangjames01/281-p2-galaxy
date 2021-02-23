// AD48FB4835AF347EB0CA8009E24C3B13F8519882

/*

    A utility file containing the declaration of the class starWars.

*/

#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include "P2random.h"

using namespace std;

class starWars {
public:
    
    void getOptions(int argc, char** argv);
    
    void readOverviewInput();
    
    void readDeployment();
    
    
    bool checkFight();
    
    void instigateFight();
    
    void verboseOutput(const int currentPlanet, const int numTroopsLost);
    
    void simulateStarWars(int argc, char** argv);
    
private:
    // Output mode variables
    bool verboseMode = 0;
    bool medianMode = 0;
    bool generalEvalMode = 0;
    bool watcherMode = 0;
    
    // Input read in variables
    char inputMode = ' ';
    int numGenerals = 0;
    int numPlanets = 0;
    int currentTimeStamp = 0;
    
    // PR input variables
    int numSeed = 0;
    int numDeployments = 0;
    int arrivalRate = 0;
    
    // Battalion deployment
    struct Deployment {
        //planetNumber
        int timeStamp = 0;
        int generalID = 0;
        int planetID = 0;
        int forceSensitivity = 0;
        mutable int numTroops = 0;
        int uniqueID = 0;
    };
    
    bool currentSith = 0;
    
    int IDlist = 0;
    
    Deployment currentDeployment;
    
    // ADD TIEBREAKER INFO WITH UNIQUE ID
    class sithComparator {
    public:
        bool operator()(const Deployment& d1, const Deployment& d2) const {
            return d1.forceSensitivity < d2.forceSensitivity;
        }
    };
    
    class jediComparator {
    public:
        bool operator()(const Deployment& d1, const Deployment& d2) const {
            return d1.forceSensitivity > d2.forceSensitivity;
        }
    };
    
    
    struct Planet {
        // Sith priority queue
        priority_queue<Deployment, vector<Deployment>, sithComparator> theSiths;
        
        //Jedi priority queue
        priority_queue<Deployment, vector<Deployment>, jediComparator> theJedis;
        
    };
    
    vector<Planet> planets;
    
    void inputErrorCheckDL(const Deployment& temp);
    void processDeployment(const char sith_jedi, const Deployment& temp);
};

