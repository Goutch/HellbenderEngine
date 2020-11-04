#pragma once
#include "core/utility/Job.h"
#include "core/utility/ModelImporter.h"
#include "list"

class Resource {
protected:
    static std::list<Job<std::vector<std::pair<MeshData, MaterialData>> *,std::string>*> running_jobs;
public:
    static void updateJobsStatus();
};
