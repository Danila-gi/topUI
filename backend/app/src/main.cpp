#include <drogon/drogon.h>
using namespace drogon;

#include "proccontroller.h"

int main()
{
    ProcController procController("/host/proc");

    app().addListener("0.0.0.0", 80);
    app().registerHandler("/get_data", 
        [&procController](const HttpRequestPtr &request, 
                                         std::function<void(const HttpResponsePtr &)> &&callback) {
            procController.readProcesses();
            Json::Value ret;
            Json::Value dataArray(Json::arrayValue);
            
            for (const auto proc: procController.getProcData()) {
                Json::Value item;
                item["pid"] = proc.pid;
                item["pr"] = proc.pr;
                item["ni"] = proc.ni;
                item["virt"] = proc.virt;
                item["res"] = proc.res;
                item["state"] = proc.state;
                item["cpu"] = proc.cpuPercent;
                item["time"] = Json::Int64(proc.time);
                item["command"] = proc.command;
                dataArray.append(item);
            }
            
            ret["data"] = dataArray;
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }, 
        {Get});
    
    app().run();
    return 0;
}