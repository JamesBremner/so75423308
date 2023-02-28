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
    std::vector<cSection> vSection;
    int myArea;
    int myPop;
    cCombined()
        : myArea(0),
          myPop(0)
    {
    }
    bool IsConnected(const cSection &sect)
    {
        for (auto &s : vSection)
            if (s.isConnected(sect))
                return true;
        return false;
    }
    void add(cSection &sect)
    {
        vSection.push_back(sect);
        sect.myfCombined = true;
        myArea += sect.myArea;
        myPop += sect.myPop;
    }

    void display()
    {
        for (auto &s : vSection)
            std::cout << s.myName << " ";

        std::cout << " | "
                  << "area " << myArea
                  << " pop " << myPop
                  << "\n";
    }
};

std::vector<cSection> vSection;
std::vector<cCombined> theCombined;
const int maxArea = 6; // Limit on total area of combined sections

void displaySections()
{
    for (auto &sect : vSection)
        sect.display();
}
void displayCombinations()
{
    for (auto &comb : theCombined)
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

void generateRandom(
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

void combine()
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
            if (a1.myArea + a2.myArea > maxArea)
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
        for (auto &C : theCombined)
        {
            // loop over uncombined seaction for possible addition to combimation
            for (auto &U : vSection)
            {
                // it it uncombined
                if (U.myfCombined)
                    continue;

                // is the the combined area under the limit
                if (C.myArea + U.myArea > maxArea)
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
main()
{
    generateRandom(10, 5);
    displaySections();
    combine();
    displayCombinations();

    raven::set::cRunWatch::Start();
    generateRandom(2000, 5);
    combine();
    raven::set::cRunWatch::Report();
    return 0;
}
