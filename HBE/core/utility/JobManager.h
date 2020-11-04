class IJob;

#include "Job.h"
#include "list"
class JobManager {
private:
    static std::list<IJob*> running_jobs;
public:
    template<typename Return, typename... Args>
    static Job<Return, Args...> *create() {
        Job* job=new Job<Return,Args...>();
        running_jobs.push_back(job);
        return job;
    }
    static void updateJobsStatus(){
        for (auto i = running_jobs.begin(); i != running_jobs.end();) {
            if ((*i)->isFinish()) {
                (*i)->onFinish();
                delete (*i);
                auto current=i;
                --i;
                running_jobs.erase(current);
            } else {
                ++i;
            }
        }
    }
};
