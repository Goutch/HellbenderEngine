//
// Created by User on 03-Jun.-2020.
//

#include "Geometry.h"
#include <algorithm>

namespace HBE {
    void Geometry::createQuad(Mesh &mesh, float size_x, float size_y, UsedBuffers used_buffers) {
        if (used_buffers.vertex_position) {
            std::vector<vec3> vertex_positions = {
                    vec3(0.5f * size_x, -0.5f * size_y, .0f),
                    vec3(0.5f * size_x, 0.5f * size_y, .0f),
                    vec3(-0.5f * size_x, 0.5f * size_y, .0f),
                    vec3(-0.5f * size_x, -0.5f * size_y, .0f)
            };
            mesh.setBuffer(0, vertex_positions);
        }
        if (used_buffers.vertex_uv) {
            std::vector<vec2> vertex_uvs = {
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
            };
            mesh.setBuffer(1, vertex_uvs);
        }
        if (used_buffers.vertex_normal) {
            std::vector<vec3> vertex_normals = {
                    vec3(0, 0, -1.0f),
                    vec3(0, 0, -1.0f),
                    vec3(0, 0, -1.0f),
                    vec3(0, 0, -1.0f),
            };
            mesh.setBuffer(2, vertex_normals);
        }
        std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0,
        };
        mesh.setIndices(indices);
    }

    void Geometry::createCube(Mesh &mesh, float size_x, float size_y, float size_z, UsedBuffers used_buffers) {
        //Back   Front
        //1---2  2---1
        //| / |  | / |
        //0---3  3---0
        if (used_buffers.vertex_position) {
            std::vector<vec3> vertex_positions = {
                    //Back
                    vec3(0.5f * size_x, -0.5f * size_y, .5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(-0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(-0.5f * size_x, -0.5f * size_y, .5 * size_z),
                    //Right
                    vec3(.5 * size_x, -.5 * size_y, -.5 * size_z),
                    vec3(.5 * size_x, .5 * size_y, -.5 * size_z),
                    vec3(.5 * size_x, .5 * size_y, .5 * size_z),
                    vec3(.5 * size_x, -.5 * size_y, .5 * size_z),
                    //Front
                    vec3(-0.5f * size_x, -0.5f * size_y, -.5 * size_z),
                    vec3(-0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    vec3(0.5f * size_x, -0.5f * size_y, -.5 * size_z),

                    //Left
                    vec3(-.5 * size_x, -.5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, .5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, .5 * size_y, -.5 * size_z),
                    vec3(-.5 * size_x, -.5 * size_y, -.5 * size_z),
                    //Top
                    vec3(-0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    vec3(-0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    //Bottom
                    vec3(.5 * size_x, -.5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, -.5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, -.5 * size_y, -.5 * size_z),
                    vec3(.5 * size_x, -.5 * size_y, -.5 * size_z),

            };
            mesh.setBuffer(0, vertex_positions);
        }
        if (used_buffers.vertex_uv) {
            std::vector<vec2> vertex_uvs = {

                    //Back
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    //Right
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    //Front
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),

                    //Left
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),

                    //Top
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),

                    //Bottom
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f)

            };
            mesh.setBuffer(1, vertex_uvs);
        }
        std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0,
                0 + 4, 1 + 4, 2 + 4,
                2 + 4, 3 + 4, 0 + 4,
                0 + 8, 1 + 8, 2 + 8,
                2 + 8, 3 + 8, 0 + 8,
                0 + 12, 1 + 12, 2 + 12,
                2 + 12, 3 + 12, 0 + 12,
                0 + 16, 1 + 16, 2 + 16,
                2 + 16, 3 + 16, 0 + 16,
                0 + 20, 1 + 20, 2 + 20,
                2 + 20, 3 + 20, 0 + 20,
        };
        mesh.setIndices(indices);


    }

    void Geometry::createInvertedCube(Mesh &mesh, float size_x, float size_y, float size_z, UsedBuffers used_buffers) {
        //Back   Front
        //1---2  2---1
        //| / |  | / |
        //0---3  3---0
        if (used_buffers.vertex_position) {
            std::vector<vec3> vertex_positions = {
                    //Back
                    vec3(-0.5f * size_x, -0.5f * size_y, -.5 * size_z),
                    vec3(-0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    vec3(0.5f * size_x, -0.5f * size_y, -.5 * size_z),
                    //Right
                    vec3(.5 * size_x, -.5 * size_y, -.5 * size_z),
                    vec3(.5 * size_x, .5 * size_y, -.5 * size_z),
                    vec3(.5 * size_x, .5 * size_y, .5 * size_z),
                    vec3(.5 * size_x, -.5 * size_y, .5 * size_z),
                    //Front
                    vec3(0.5f * size_x, -0.5f * size_y, .5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(-0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(-0.5f * size_x, -0.5f * size_y, .5 * size_z),

                    //Left
                    vec3(-.5 * size_x, -.5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, .5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, .5 * size_y, -.5 * size_z),
                    vec3(-.5 * size_x, -.5 * size_y, -.5 * size_z),
                    //Top
                    vec3(-0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    vec3(-0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, .5 * size_z),
                    vec3(0.5f * size_x, 0.5f * size_y, -.5 * size_z),
                    //Bottom
                    vec3(.5 * size_x, -.5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, -.5 * size_y, .5 * size_z),
                    vec3(-.5 * size_x, -.5 * size_y, -.5 * size_z),
                    vec3(.5 * size_x, -.5 * size_y, -.5 * size_z),

            };

            std::reverse(vertex_positions.begin(), vertex_positions.end());
            mesh.setBuffer(0, vertex_positions);
        }
        if (used_buffers.vertex_uv) {
            std::vector<vec2> vertex_uvs = {

                    //Back
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    //Right
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    //Front
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),

                    //Left
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),

                    //Top
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),

                    //Bottom
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f)
            };
            std::reverse(vertex_uvs.begin(), vertex_uvs.end());
            mesh.setBuffer(1, vertex_uvs);
        }
        std::vector<unsigned int> indices = {
                0, 1, 2,
                2, 3, 0,
                0 + 4, 1 + 4, 2 + 4,
                2 + 4, 3 + 4, 0 + 4,
                0 + 8, 1 + 8, 2 + 8,
                2 + 8, 3 + 8, 0 + 8,
                0 + 12, 1 + 12, 2 + 12,
                2 + 12, 3 + 12, 0 + 12,
                0 + 16, 1 + 16, 2 + 16,
                2 + 16, 3 + 16, 0 + 16,
                0 + 20, 1 + 20, 2 + 20,
                2 + 20, 3 + 20, 0 + 20,
        };
        mesh.setIndices(indices);
    }
}