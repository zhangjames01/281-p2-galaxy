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
        
    void simulateStarWars(int argc, char** argv);
    
private:
    // Output mode variables
    bool verboseMode = 0;
    bool medianMode = 0;
    bool generalEvalMode = 0;
    bool watcherMode = 0;
    // Input read in variables
    char inputMode = ' ';
    // State class
    enum class State : char {Initial, SeenOne, SeenBoth, MaybeBetter};
    uint32_t numGenerals = 0;
    uint32_t numPlanets = 0;
    uint32_t currentTimeStamp = 0;
    uint32_t IDlist = 0;
    // Variables used in output
    uint32_t numBattles = 0;
    
    struct Deployment {
        uint32_t generalID = 0;
        int forceSensitivity = 0;
        mutable int numTroops = 0;
        uint32_t uniqueID = 0;
    };
    class sithComparator {
    public:
        bool operator()(const Deployment& d1, const Deployment& d2) const {
            if (d1.forceSensitivity == d2.forceSensitivity) {
                return d1.uniqueID > d2.uniqueID;
            }
            else {
                return d1.forceSensitivity < d2.forceSensitivity;
            }
        }
    };
    class jediComparator {
    public:
        bool operator()(const Deployment& d1, const Deployment& d2) const {
            if (d1.forceSensitivity == d2.forceSensitivity) {
                return d1.uniqueID > d2.uniqueID;
            }
            else {
                return d1.forceSensitivity > d2.forceSensitivity;
            }
        }
    };
    struct General {
        int numJedi = 0;
        int numSith = 0;
        int numSurvivors = 0;
    };
    struct Planet {
        
        class attackWatch {
        private:
            State attackState = State::Initial;
            
            uint32_t bestSithTime = 0;
            
            int bestSithForce = 0;
            
            uint32_t bestJediTime = 0;
            
            int bestJediForce = 0;
            
            uint32_t maybeBetterTime = 0;
            
            int maybeBetterForce = 0;
            
        public:
            void processAttackWatch(const char sith_jedi, const int forceSensitivity, const uint32_t timeStamp);
            
            void attackWatchOutput(const uint32_t planet);
        };
        attackWatch attack;
        
        class ambushWatch {
        private:
            State ambushState = State::Initial;
            
            uint32_t bestSithTime = 0;
            
            int bestSithForce = 0;
            
            uint32_t bestJediTime = 0;
            
            int bestJediForce = 0;
            
            uint32_t maybeBetterTime = 0;
            
            int maybeBetterForce = 0;
            
        public:
            void processAmbushWatch(const char sith_jedi, const int forceSensitivity, const uint32_t timeStamp);
            
            void ambushWatchOutput(const uint32_t planet);
        };
        ambushWatch ambush;
        
        // Sith priority queue
        priority_queue<Deployment, vector<Deployment>, sithComparator> theSiths;
        
        //Jedi priority queue
        priority_queue<Deployment, vector<Deployment>, jediComparator> theJedis;
        
        priority_queue<int> lowerHalf;
        
        priority_queue<int, vector<int>, greater<int>> upperHalf;
        
        int median = 0;
    };
    vector<General> generals;
    vector<Planet> planets;
    
    // Read and process command line options.
    void getOptions(int argc, char** argv);
    
    // Read in the introductory input information.
    void readOverviewInput();
    
    // Read in deployments one by one.
    void readDeployment();
    
    // Checks for errors when reading in using DL input mode.
    void inputErrorCheckDL(const Deployment& temp, const uint32_t timeStamp, const uint32_t planetID);
    
    // Process the current deployment into their respective planet.
    void processDeployment(const Deployment& temp, const char sith_jedi, const uint32_t timeStamp, const uint32_t planetID);
    
    // Check if a fight will occur at the current planet.
    bool checkFight(const uint32_t currentPlanet);
    
    // Instigate a fight on the current planet.
    void instigateFight(const uint32_t currentPlanet);
    
    // Print information for a battle that has occured.
    void verboseOutput(const uint32_t currentPlanet, const int numTroopsLost);
    
    // Process the median information.
    void processMedian(const uint32_t currentPlanet, int numTroopsLost);
    // Print the median troops lost for each planet, only if battles have occurred.
    void medianOutput();
    
    // Process the number of troops deployed for each general per deployment.
    void processGeneralTroops(const char sith_jedi, const Deployment& temp);
    // Process the remaining number of troops that survived for each general.
    void processSurvivors();
    // Print the general evaluation information.
    void generalEvalOutput();
};


