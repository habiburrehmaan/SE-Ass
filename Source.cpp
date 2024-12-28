#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <sstream>

using namespace std;

/****************************
 * Ant Base Class *
 ****************************/
class Ant {
public:
    string role;
    Ant(string role) : role(role) {}
    virtual ~Ant() = default;
};

class WorkerAnt : public Ant {
public:
    WorkerAnt() : Ant("Worker") {}
};

class WarriorAnt : public Ant {
public:
    WarriorAnt() : Ant("Warrior") {}
};

/***************************
 * AntFarm Class *
 ***************************/
class AntFarm {
private:
    int id;
    string species;
    vector<shared_ptr<Ant>> ants;
    int workers = 0;
    int warriors = 0;
    int antKills = 0;
    map<string, int> colonyKills;
    int ticksAlive = 0;
    bool alive = true;

public:
    AntFarm(int id, string species) : id(id), species(species) {}

    int getId() const { return id; }
    string getSpecies() const { return species; }
    int getTicksAlive() const { return ticksAlive; }
    bool isAlive() const { return alive; }

    void addAnt(shared_ptr<Ant> ant) {
        ants.push_back(ant);
        if (ant->role == "Worker") workers++;
        else if (ant->role == "Warrior") warriors++;
    }

    void recordKill(string species) {
        antKills++;
        colonyKills[species]++;
    }

    void removeAnts(int count) {
        while (count > 0 && !ants.empty()) {
            if (ants.back()->role == "Worker") workers--;
            else if (ants.back()->role == "Warrior") warriors--;
            ants.pop_back();
            count--;
        }
        if (ants.empty()) alive = false;
    }

    void tick() { ticksAlive++; }

    void displaySummary() {
        cout << "Species: " << species << "\n";
        cout << "Workers: " << workers << "\n";
        cout << "Warriors: " << warriors << "\n";
        cout << "Ant Kills: " << antKills << "\n";
        cout << "Colony Kills: " << colonyKills.size() << " (";
        for (const auto& kill : colonyKills) {
            cout << kill.first << ":" << kill.second << " ";
        }
        cout << ")\n";
        cout << "Ticks Alive: " << ticksAlive << "\n";
        cout << "Status: " << (alive ? "Alive" : "Dead") << "\n";
    }
};

/****************************
 * Singleton Meadow Class *
 ****************************/
class Meadow {
private:
    static Meadow* instance;
    vector<shared_ptr<AntFarm>> antFarms;

    Meadow() {} // Private constructor

public:
    static Meadow* getInstance() {
        if (!instance)
            instance = new Meadow();
        return instance;
    }

    void addAntFarm(shared_ptr<AntFarm> antFarm) { antFarms.push_back(antFarm); }

    vector<shared_ptr<AntFarm>>& getAntFarms() { return antFarms; }

    void removeAntFarm(int id) {
        antFarms.erase(
            remove_if(antFarms.begin(), antFarms.end(),
                [id](const shared_ptr<AntFarm>& farm) { return farm->getId() == id; }),
            antFarms.end());
    }

    void simulateCombat() {
        for (size_t i = 0; i < antFarms.size(); ++i) {
            for (size_t j = i + 1; j < antFarms.size(); ++j) {
                auto& farm1 = antFarms[i];
                auto& farm2 = antFarms[j];
                if (farm1->isAlive() && farm2->isAlive()) {
                    // Simulate combat
                    int farm1Loss = rand() % 5;
                    int farm2Loss = rand() % 5;
                    farm1->removeAnts(farm1Loss);
                    farm2->removeAnts(farm2Loss);
                    if (!farm2->isAlive()) farm1->recordKill(farm2->getSpecies());
                    if (!farm1->isAlive()) farm2->recordKill(farm1->getSpecies());
                }
            }
        }
    }

    void displaySummary() {
        for (const auto& antFarm : antFarms) {
            antFarm->displaySummary();
            cout << "--------------------------\n";
        }
    }
};
Meadow* Meadow::instance = nullptr;

/**********************
 * Command Functions *
 **********************/
void executeSpawn(int id, string species, int workers, int warriors) {
    auto meadow = Meadow::getInstance();
    auto antFarm = make_shared<AntFarm>(id, species);
    for (int i = 0; i < workers; ++i) {
        antFarm->addAnt(make_shared<WorkerAnt>());
    }
    for (int i = 0; i < warriors; ++i) {
        antFarm->addAnt(make_shared<WarriorAnt>());
    }
    meadow->addAntFarm(antFarm);
    cout << "Colony ID: " << id << " created with " << workers << " workers and " << warriors << " warriors.\n";
}

void executeTick(int ticks) {
    auto meadow = Meadow::getInstance();
    for (int t = 0; t < ticks; ++t) {
        cout << "Tick: " << t + 1 << "\n";
        for (auto& antFarm : meadow->getAntFarms()) {
            if (antFarm->isAlive()) {
                antFarm->tick();
            }
        }
        meadow->simulateCombat();
    }
}

void executeSummary() {
    auto meadow = Meadow::getInstance();
    meadow->displaySummary();
}

/**********************
 * Main Function *
 **********************/
int main() {
    executeSpawn(1, "Killer", 14, 10);
    executeSpawn(2, "Pansy", 4, 0);
    executeTick(25);
    executeSummary();

    return 0;
}
