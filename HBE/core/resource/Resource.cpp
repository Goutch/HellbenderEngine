#include "Resource.h"

std::list<Job<std::vector<std::pair<MeshData, MaterialData>> *,std::string>*> Resource::running_jobs;

void Resource::updateJobsStatus() {
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
