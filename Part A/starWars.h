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
    
    // Variables used in output
    int numBattles = 0;
    
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
    
    
    
    
    
    enum class State : char {Initial, SeenOne, SeenBoth, MaybeBetter};
    
    
    
    
    
    
    
    bool currentSith = 0;
    
    int IDlist = 0;
    
    Deployment currentDeployment;
    
    // ADD TIEBREAKER INFO WITH UNIQUE ID
    class sithComparator {
    public:
        bool operator()(const Deployment& d1, const Deployment& d2) const {
            if (d1.forceSensitivity == d2.forceSensitivity) {
                return d1.uniqueID < d2.uniqueID;
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
    
    
    struct Planet {
        // Sith priority queue
        priority_queue<Deployment, vector<Deployment>, sithComparator> theSiths;
        
        //Jedi priority queue
        priority_queue<Deployment, vector<Deployment>, jediComparator> theJedis;
        
        priority_queue<int> lowerHalf;
        
        priority_queue<int, vector<int>, greater<int> > upperHalf;

        int median = 0;
        
        class attackWatch {
        private:
            State attackState = State::Initial;
            
            int bestSithTime = 0;
            
            int bestSithForce = 0;
            
            int bestJediTime = 0;
            
            int bestJediForce = 0;
            
            int maybeBetterTime = 0;
            
            int maybeBetterForce = 0;
            
        public:
            
            void processAttackWatch(const char sith_jedi, const Deployment& temp);
            
            void attackWatchOutput(const int planet);
        };
        
        attackWatch attack;
        
        class ambushWatch {
        private:
            State ambushState = State::Initial;
            
            int bestSithTime = 0;
            
            int bestSithForce = 0;
            
            int bestJediTime = 0;
            
            int bestJediForce = 0;
            
            int maybeBetterTime = 0;
            
            int maybeBetterForce = 0;
            
        public:
            
            void processAmbushWatch(const char sith_jedi, const Deployment& temp);
            
            void ambushWatchOutput(const int planet);
        };
        
        ambushWatch ambush;
        
    };
    
    vector<Planet> planets;
    
    struct General {
        int numJedi = 0;
        int numSith = 0;
        int numSurvivors = 0;
    };
    
    vector<General> generals;
    
    void inputErrorCheckDL(const Deployment& temp);
    void processDeployment(const char sith_jedi, const Deployment& temp);
    void processMedian(const int currentPlanet, int numTroopsLost);
    void medianOutput();
    
    void processGeneralTroops(const char sith_jedi, const Deployment& temp);
    
    void processSurvivors();
    
    void generalEvalOutput();
    
    void processMovieWatcher(const char sith_jedi, const Deployment& temp);
};


