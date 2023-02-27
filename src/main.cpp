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

    cSection()
    {
    }
    cSection(const std::string &n,
             int area,
             int pop)
        : myName(n),
          myArea(area),
          myPop(pop)
    {
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

std::vector<cSection> vSection;

void display()
{
    for (auto &sect : vSection)
        sect.display();
}
void display(const std::vector<std::pair<cSection, cSection>> &comb)
{
    for (auto &p : comb)
        std::cout << p.first.myName << ", " << p.second.myName << " | "
                  << "area " << p.first.myArea + p.second.myArea
                  << " pop " << p.first.myPop + p.second.myPop
                  << "\n";
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
    const int maxArea = 10;
    const int maxPop = 100;

    vSection.clear();
    vSection.resize(SectionCount);
    int ks = 0;
    for (auto &s : vSection)
    {
        s.myName = "S" + std::to_string(ks);
        s.myArea = rand() % maxArea;
        s.myPop = rand() % maxPop;

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

std::vector<std::pair<cSection, cSection>> combine()
{
     raven::set::cRunWatch aWatcher("pairwise combine all sections");
    const int maxArea = 6;

    std::vector<std::pair<cSection, cSection>> ret;

    for (auto &a1 : vSection) {
        raven::set::cRunWatch aWatcher("pairwise combine section");
        for (auto &a2 : vSection)
        {
            if (a1.myName == a2.myName)
                continue;
            if (a1.myArea + a2.myArea > maxArea)
                continue;
            if (std::find(
                    a1.adjacent.begin(), a1.adjacent.end(),
                    atoi(a2.myName.substr(1).c_str())) == a1.adjacent.end())
                continue;
            ret.push_back(std::make_pair(a1, a2));
        }
    }
    return ret;
}
main()
{
    raven::set::cRunWatch::Start();
    generateRandom(2000, 5);
    //display();
    auto comb = combine();
    //display(comb);
    raven::set::cRunWatch::Report();
    return 0;
}
