#pragma once
#include "Core.h"
#include <core/resource/Mesh.h>
struct HB_API UsedBuffers
{
    bool vertex_position=true;
    bool vertex_uv=true;
    bool vertex_normal=true;
};
class HB_API Geometry {


public:
    static void createQuad(Mesh& mesh,float size_x,float size_y,UsedBuffers used_buffers={});

    static void createCube(Mesh& mesh,float size_x,float size_y,float size_z,UsedBuffers used_buffers={});
    static void createInvertedCube(Mesh& mesh,float size_x,float size_y,float size_z,UsedBuffers used_buffers={});
};
