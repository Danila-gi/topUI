#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unistd.h>

#include "proccontroller.h"

namespace fs = std::filesystem;

ProcController::ProcController(const std::string& procCommonPath): mProcCommonPath(procCommonPath){

}

bool ProcController::isStrDigit(std::string& str){
    for (char ch : str) {
        int v = ch;
        if (!(ch >= 48 && ch <= 57)) {
            return false;
        }
    }
    return true;
}

bool ProcController::getNumberFromStr(std::string& token, long long& result){
    try {
        size_t resultConvertPose = 0;
        result = std::stoll(token, &resultConvertPose);
        return resultConvertPose == token.length();
    } catch (const std::exception&) {
        return false;
    }
}

bool ProcController::getNumberFromStr(std::string& token, int& result){
    try {
        size_t resultConvertPose = 0;
        result = std::stoi(token, &resultConvertPose);
        return resultConvertPose == token.length();
    } catch (const std::exception&) {
        return false;
    }
}

long long ProcController::getTotalCpu(){
    std::ifstream in(mProcCommonPath + "/" + mProcStatFile);
    std::string line;
    long long cpu = 0;
    if (in.is_open()){
        if (!std::getline(in, line)){
            in.close();
            return 0;
        }
        std::string attr;
        std::stringstream streamLine(line);
        streamLine >> attr;
        for (int i = 0; i < 10; i++){
            int value;
            streamLine >> value;
            cpu += value;
        }
    }
    in.close();
    return cpu;
}

void ProcController::readProcesses(){
    mProcCollection.clear();
    for (const auto& enty : fs::directory_iterator(mProcCommonPath)){
        std::string entyStr = enty.path().c_str();
        std::vector<std::string> partsOfEnty;
        std::string token;
        std::stringstream entyStream(entyStr);
        while (std::getline(entyStream, token, '/')){
            partsOfEnty.push_back(token);
        }
        
        if (isStrDigit(partsOfEnty.back())) {
            const auto currentProcStat = mProcCommonPath + "/" + partsOfEnty.back() + mProcStatFile; 
            std::ifstream in(currentProcStat);
            std::string line;
            ProcStat procStat;
            if (in.is_open()){
                if (std::getline(in, line)){
                    std::string attr;
                    std::stringstream streamLine(line);
                    std::vector<std::string> attrs;
                    std::getline(streamLine, attr, ' ');
                    attrs.push_back(attr);
                    std::getline(streamLine, attr, ')');
                    attrs.push_back(attr + ")");
                    std::getline(streamLine, attr, ' ');
                    while (std::getline(streamLine, attr, ' ')){
                        attrs.push_back(attr);
                    }
                    
                    getNumberFromStr(attrs[0], procStat.pid);
                    getNumberFromStr(attrs[17], procStat.pr);
                    getNumberFromStr(attrs[18], procStat.ni);
                    getNumberFromStr(attrs[22], procStat.virt);
                    getNumberFromStr(attrs[23], procStat.res);
                    procStat.state = attrs[2];
                    {
                        long long utime, stime;
                        getNumberFromStr(attrs[13], utime);
                        getNumberFromStr(attrs[14], stime);

                        procStat.cpuTotal = getTotalCpu();
                        procStat.time = utime + stime;

                        if (mProcLastTimeCPU.find(procStat.pid) != mProcLastTimeCPU.end()){
                            procStat.cpuPercent = static_cast<double>(procStat.time - mProcLastTimeCPU[procStat.pid].first) / (procStat.cpuTotal - mProcLastTimeCPU[procStat.pid].second);
                        }
                        mProcLastTimeCPU[procStat.pid] = {procStat.time, procStat.cpuTotal};
                    }
                    procStat.command = attrs[1];

                    mProcCollection.push_back(procStat);
                }
            }
        };
    }
}

std::vector<ProcStat> ProcController::getProcData() const{
    return mProcCollection;
}
