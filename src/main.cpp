#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "cRunWatch.h"

class cSection
{
public:
    std::string myName;
    int myArea;
    int myPop;
    std::vector<int> adjacent;
    bool myfCombined;

    cSection()
        : myfCombined(false)
    {
    }
    cSection(const std::string &n,
             int area,
             int pop)
        : myName(n),
          myArea(area),
          myPop(pop),
          myfCombined(false)
    {
    }
    bool isConnected(const cSection &other)
    {
        return (std::find(
                    adjacent.begin(), adjacent.end(),
                    atoi(other.myName.substr(1).c_str())) != adjacent.end());
    }
    void display()
    {
        std::cout << myName
                  << " area " << myArea
                  << " pop " << myPop
                  << " connected ";
        for (auto a : adjacent)
            std::cout << a << " ";
        std::cout << "\n";
    }
    bool operator==(const cSection &other) const
    {
        return myName == other.myName;
    }
};

class cCombined
{
public:
    std::vector<cSection> mySection; // sections in combination
    int myArea;                     // total area of sections
    int myPop;                      // total population of sections

    cCombined();

    /// @brief Is there a physical connection to a section not in this combination
    /// @param sect
    /// @return

    bool IsConnected(const cSection &sect);

    /// @brief Add section to combination
    /// @param sect

    void add(cSection &sect);

    void display();
};

class cOptimizer
{
public:
    std::vector<cSection> vSection;
    std::vector<cCombined> theCombined;
    std::vector<cCombined> myBestCombined;

    int combinedAreaLimit() const
    {
        return 6;
    }

    /// @brief generate some random sections
    /// @param SectionCount number of sections
    /// @param AvAdjacentCount average connections to other sections

    void generateRandom(
        int SectionCount,
        int AvAdjacentCount);

        void combine();

    /// @brief Search for optimum combination
    /// @param iters number of iterations

    void Optimize(int iters);

    /// @brief Value ( population ) of all combined sections
    /// @return 
    int Value();

    void displaySections();
    void display();
};

cOptimizer O;

cCombined::cCombined()
    : myArea(0),
      myPop(0)
{
}
void cCombined::add(cSection &sect)
{
    mySection.push_back(sect);
    sect.myfCombined = true;
    myArea += sect.myArea;
    myPop += sect.myPop;
}

bool cCombined::IsConnected(const cSection &sect)
{
    for (auto &s : mySection)
        if (s.isConnected(sect))
            return true;
    return false;
}

void cCombined::display()
{
    for (auto &s : mySection)
        std::cout << s.myName << " ";

    std::cout << " | "
              << "area " << myArea
              << " pop " << myPop
              << "\n";
}

void cOptimizer::displaySections()
{
    for (auto &sect : vSection)
        sect.display();
}
void cOptimizer::display()
{
    for (auto &comb : myBestCombined)
        comb.display();
}

// void generate1()
// {
//     vSection.push_back(cSection("A", 6, 100));
//     vSection.back().adjacent.push_back("B");
//     vSection.back().adjacent.push_back("C");
//     vSection.push_back(cSection("B", 4, 50));
//     vSection.back().adjacent.push_back("A");
//     vSection.push_back(cSection("C", 2, 20));
//     vSection.back().adjacent.push_back("A");
//     vSection.back().adjacent.push_back("D");
//     vSection.push_back(cSection("D", 3, 70));
//     vSection.back().adjacent.push_back("C");
//     vSection.back().adjacent.push_back("E");
//     vSection.push_back(cSection("E", 2, 75));
//     vSection.back().adjacent.push_back("D");
// }

void cOptimizer::generateRandom(
    int SectionCount,
    int AvAdjacentCount)
{
    const int maxArea = 2;
    const int maxPop = 100;

    vSection.clear();
    vSection.resize(SectionCount);
    int ks = 0;
    for (auto &s : vSection)
    {
        s.myName = "S" + std::to_string(ks);
        s.myArea = rand() % maxArea + 1;
        s.myPop = rand() % maxPop + 1;

        int adjCount = rand() % (2 * AvAdjacentCount);
        for (int ka = 0; ka < adjCount; ka++)
        {
            int index = ks;
            auto it = s.adjacent.end();
            while (index == ks || it != s.adjacent.end())
            {
                index = rand() % SectionCount;
                it = std::find(
                    s.adjacent.begin(), s.adjacent.end(),
                    index);
            }
            s.adjacent.push_back(index);
        }
        ks++;
    }
}

void cOptimizer::combine()
{
    raven::set::cRunWatch aWatcher("combine sections");

    // Finds pairs of sections that can be combined
    for (auto &a1 : vSection)
    {
        // check for already combined
        if (a1.myfCombined)
            continue;

        // search for possible section to combine with a1
        for (auto &a2 : vSection)
        {
            // is it uncombined
            if (a2.myfCombined)
                continue;

            // is it itself
            if (a1.myName == a2.myName)
                continue;

            // is the the combined area under the limit
            if (a1.myArea + a2.myArea > combinedAreaLimit())
                continue;

            // is it physically connected
            if (!a1.isConnected(a2))
                continue;

            // OK to combine
            cCombined comb;
            comb.add(a1);
            comb.add(a2);
            theCombined.push_back(comb);
            break;
        }
    }

    // Try adding uncombined sections to the combinations already found

    bool fimproved = true;
    while (fimproved)
    {
        // loop over the combinations until no more new combinations found
        fimproved = false;
        for (auto &C : O.theCombined)
        {
            // loop over uncombined seaction for possible addition to combimation
            for (auto &U : O.vSection)
            {
                // it it uncombined
                if (U.myfCombined)
                    continue;

                // is the the combined area under the limit
                if (C.myArea + U.myArea > O.combinedAreaLimit())
                    continue;

                // is it physically connected
                if (!C.IsConnected(U))
                    continue;

                // OK, add to combination
                C.add(U);
                fimproved = true;
            }
        }
        // check we are still finding additional combinations
        if (!fimproved)
            break;
    }
}
int cOptimizer::Value()
{
    int value = 0;
    for (auto &C : O.theCombined)
        value += C.myPop;
    return value;
}

void cOptimizer::Optimize(int iters)
{
    // do an initial combination
    combine();

    // store combination and its value
    myBestCombined = theCombined;
    int bestValue = Value();

    // loop for the requested number of iterations
    for (int t = 0; t < iters; t++)
    {
        // start timer
        raven::set::cRunWatch aWatcher("try");

        // randomize the order of sections
        std::random_shuffle(vSection.begin(), vSection.end());

        // do another combination
        combine();

        // check for an improved value
        int value = Value();
        if (value > bestValue)
        {
            // store improved combination and its value
            bestValue = value;
            myBestCombined = theCombined;
            std::cout << "Try " << t << " best value now " << bestValue;
        }
    }
}
main()
{

    // unit test
    O.generateRandom(10, 5);
    O.displaySections();
    O.Optimize(1);
    O.display();

    // 2000 section time profile
    raven::set::cRunWatch::Start();
    O.generateRandom(2000, 5);
    O.Optimize(4000);

    // O.display();
    raven::set::cRunWatch::Report();
    return 0;
}
