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

class cOptimizer
{
    public:
     std::vector<cCombined> theCombined;
    std::vector<cCombined> myBestCombined;

    int combinedAreaLimit() const
    {
        return 6;
    }

    /// @brief Search for optimum combination
    /// @param iters number of iterations

    void Optimize( int iters);

    void display();
};

std::vector<cSection> vSection;

cOptimizer O;

void displaySections()
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
            if (a1.myArea + a2.myArea > O.combinedAreaLimit())
                continue;

            // is it physically connected
            if (!a1.isConnected(a2))
                continue;

            // OK to combine
            cCombined comb;
            comb.add(a1);
            comb.add(a2);
            O.theCombined.push_back(comb);
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
            for (auto &U : vSection)
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
int Value()
{
    int value = 0;
    for( auto& C : O.theCombined )
        value += C.myPop;
    return value;
}

void cOptimizer::Optimize( int iters )
{
    combine();
    myBestCombined = theCombined;
    int bestValue = Value();
    for( int t = 0; t < iters; t++ )
    {
        raven::set::cRunWatch aWatcher("try");

         std::random_shuffle ( vSection.begin(), vSection.end() );

         combine();

         int value = Value();
         if( value > bestValue )
         {
            bestValue = value;
            myBestCombined = theCombined;
            std::cout << "Try " << t << " best value now " << bestValue;
         }
    }
}
main()
{


    // unit test
    generateRandom(10, 5);
    displaySections();
    O.Optimize(1);
    O.display();

    // 2000 section time profile
    raven::set::cRunWatch::Start();
    generateRandom(2000, 5);
    O.Optimize( 4000);

    //O.display();
    raven::set::cRunWatch::Report();
    return 0;
}
