#include <algorithm>
#include <iostream>
#include <list>
#include "Data.h"
#include "Objet.h"

const int IN_FIXED = 3;
const int IN_NOT_FIXED = 2;
const int NOT_IN_FIXED = 1;
const int NOT_IN_NOT_FIXED = 0;

struct State {
    int solution[N]; // int declared above
                     // 3 -> 11 :     in solution and     fixed
                     // 2 -> 10 :     in solution and not fixed
                     // 1 -> 01 : not in solution and     fixed
                     // 0 -> 00 : not in solution and not fixed
    int separationIndex;
    double value;
};

// Double precision error can happen with base operators
bool lessOrEqual(const double a, const double b) {
    return (a <= b) || (std::abs(a - b) < 0.001);
}

// Return true if x equals IN_FIXED or NOT_IN_FIXED
bool isFixed(const int x) {
    return (x & 1) == 1;
}

// Return true if x equals IN_FIXED or IN_NOT_FIXED
bool isInSolution(const int x) {
    return (x & 2) == 2;
}

// Construct objects from ui and wi and store them in objects
void initObjects(const double ui[], const double wi[], Objet objects[]) {
    for (int i = 0; i < N; ++i) {
        objects[i] = Objet(i, ui[i], wi[i]);
    }
}

// Return true if obj1 ratio > obj2 ratio
// Sort objects by ration descending if used in std::sort
bool sortObjects(const Objet& obj1, const Objet obj2) {
    return obj1.ratio() > obj2.ratio();
}

// Calculate best solution when the problem is relaxed
// Precondition : objects in objects are sorted by ratio descending
void relax(
    Objet objects[], int optimumSol[], double& optimumValue, bool& isInt, int& separationIndex
) {
    optimumValue = 0;
    isInt = true;
    separationIndex = -1;
    int bagIndex = 0;
    double roomLeft = W;

    for (int i = 0; i < N; i++) {
        optimumSol[i] = NOT_IN_NOT_FIXED;
    }

    while (roomLeft > 0 && bagIndex < N) {
        double itemWeight = objects[bagIndex].getWeight();
        if (lessOrEqual(itemWeight, roomLeft)) {
            optimumValue += objects[bagIndex].getUtility();
        } else {
            isInt = false;
            separationIndex = bagIndex;
            optimumValue += objects[bagIndex].getUtility() * (roomLeft / itemWeight);
        }
        optimumSol[bagIndex] = IN_NOT_FIXED;
        /* roomLeft can be < 0. It is impossible in real life,
           but here it is okay, because the variable purpose is just to end the loop */
        roomLeft -= itemWeight;
        bagIndex += 1;
    }
}

// Calculate best solution when the problem is relaxed using constraints
// Precondition : objects in objects are sorted by ratio descending
// Return true if solution is possible
bool constrainedRelax(
    Objet objects[], int optimumSol[], double& optimumValue, bool& isInt, int& separationIndex
) {
    optimumValue = 0;
    isInt = true;
    separationIndex = -1;
    int bagIndex = 0;
    double roomLeft = W;

    for (int i = 0; i < N; i++) {
        if (optimumSol[i] == IN_FIXED) {
            roomLeft -= objects[i].getWeight();
        }
    }

    if (roomLeft < 0) {
        return false;
    }

    while (roomLeft > 0 && bagIndex < N) {
        int indexSol = optimumSol[bagIndex];
        if (!isFixed(indexSol)) {
            double itemWeight = objects[bagIndex].getWeight();
            if (lessOrEqual(itemWeight, roomLeft)) {
                optimumValue += objects[bagIndex].getUtility();
            } else {
                isInt = false;
                separationIndex = bagIndex;
                optimumValue += objects[bagIndex].getUtility() * (roomLeft / itemWeight);
            }
            optimumSol[bagIndex] = IN_NOT_FIXED;
            /* roomLeft can be < 0. It is impossible in real life,
               but here it is okay, because the variable purpose is just to end the loop */
            roomLeft -= itemWeight;
        }
        bagIndex += 1;
    }

    return true;
}

void insertState(std::list<State>& box, State& inserted) {
    for (std::list<State>::iterator it = box.begin(); it != box.end(); it++) {
        if (it->value < inserted.value) {
            box.insert(it, inserted);
            return;
        }
    }
    box.push_back(inserted);
}

void copySolution(int from[], int to[]) {
    for (int i = 0; i < N; i++) {
        to[i] = from[i];
    }
}

// Precondition : toExplore is sorted by State::value descending
// Precondition : toExplore is not empty
void branchAndBound(
    std::list<State>& toExplore, Objet objects[], double& optimumValue, int optimumSol[]
) {
    const State& father = toExplore.front();
    State child0;
    State child1;
    bool isIntChild0;
    bool isIntChild1;
    for (int i = 0; i < N; i++) {
        int indexSol = isFixed(father.solution[i]) ? father.solution[i] : 0;
        child0.solution[i] = indexSol;
        child1.solution[i] = indexSol;
    }
    child0.solution[father.separationIndex] = 1;
    child1.solution[father.separationIndex] = 3;
    bool couldAddChild0 = constrainedRelax(objects, child0.solution, child0.value,
        isIntChild0, child0.separationIndex);
    bool couldAddChild1 = constrainedRelax(objects, child1.solution, child1.value,
        isIntChild1, child1.separationIndex);

    toExplore.erase(toExplore.begin()); // Remove first element
    if (couldAddChild0) {
        if (child0.value > optimumValue) {
            if (isIntChild0) {
                optimumValue = child0.value;
                copySolution(child0.solution, optimumSol);
            } else {
                insertState(toExplore, child0);
            }
        }
    }
    if (couldAddChild1) {
        if (child1.value > optimumValue) {
            if (isIntChild1) {
                optimumValue = child1.value;
                copySolution(child1.solution, optimumSol);
            } else {
                insertState(toExplore, child1);
            }
        }
    }
}

int main() {
    // Initialization of objects
    Objet objects[N];
    initObjects(ui, wi, objects);
    std::sort(objects, objects + N, sortObjects);

    // Get relaxed solution
    struct State relaxed;
    bool isRelaxedInt;
    relax(objects, relaxed.solution, relaxed.value, isRelaxedInt, relaxed.separationIndex);
    std::cout << "Relaxed problem, optimum solution : " << relaxed.value << std::endl;

    // Main algorithm
    double optimumValue = 0;
    int optimumSol[N];
    std::list<State> toExplore;
    toExplore.push_back(relaxed);
    while (!toExplore.empty()) {
        branchAndBound(toExplore, objects, optimumValue, optimumSol);
    }
    
    // Output solution
    std::cout << "Optimum solution : " << optimumValue << std::endl;
    std::cout << "With objects : " << std::endl;
    for (int i = 0; i < N; i++) {
        if (isInSolution(optimumSol[i])) {
            std::cout << " - "
                << objects[i].getId() << " : ("
                << objects[i].getUtility() << ", "
                << objects[i].getWeight() << ")"
                << std::endl;
        }
    }

    return 0;
}
