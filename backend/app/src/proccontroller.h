#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>

struct ProcStat{
    int pid;
    int pr;
    int ni;
    int virt;
    int res;
    std::string state;
    long long cpuTotal;
    double cpuPercent;
    long long time;
    std::string command;
};

class ProcController{
public:
    ProcController(const std::string& procCommonPath);
    void readProcesses();
    std::vector<ProcStat> getProcData() const;

private:
    long long getTotalCpu();
    static bool isStrDigit(std::string& str);
    bool getNumberFromStr(std::string& token, long long& result);
    bool getNumberFromStr(std::string& token, int& result);

private:
    std::unordered_map<int, std::pair<int, int>> mProcLastTimeCPU;
    std::vector<ProcStat> mProcCollection;
    std::string mProcCommonPath {"/proc"};
    std::string mProcStatFile {"/stat"};
};