//Task class and basic methods
#include<iostream>
#include<vector>
#include<string>
#include <ctime>
#include <nlohmann/json.hpp>
#define client_accept 15
using namespace std;
struct Task{
    int task_type;// 0 for map 1 for reduce
    string file_id;
    int NMap;
    int Nreduce;//number of reducer
    int task_id;
    bool Finished = false;
    //clock_t start = clock(); fault tolerance
    Task(){
        Finished = false;
    }
    Task(int type, string id, int nMap,int nReduce, int taskId)
        : task_type(type), file_id(id),NMap(nMap), Nreduce(nReduce), task_id(taskId), Finished(false) {}
};
inline void to_json(nlohmann::json& j, const Task& t) {
    j = nlohmann::json{{"task_type", t.task_type}, {"file_id", t.file_id}, {"NMap",t.NMap},
    {"Nreduce", t.Nreduce},{"task_id",t.task_id},{"Finished",t.Finished}};
}
inline void from_json(const nlohmann::json& j, Task& t) {
    j.at("task_type").get_to(t.task_type);
    j.at("file_id").get_to(t.file_id);
    j.at("NMap").get_to(t.NMap);
    j.at("Nreduce").get_to(t.Nreduce);
    j.at("task_id").get_to(t.task_id);
    j.at("Finished").get_to(t.Finished);
}