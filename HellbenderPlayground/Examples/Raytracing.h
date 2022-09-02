#pragma once

#include "HBE.h"
#include "vector"

using namespace HBE;

class Raytracing {
public:
	static void init() {
		AABBAccelerationStructureInfo aabb__acceleration_structure_info{};
		aabb__acceleration_structure_info.max = vec3(0.5, 0.5, 0.5);
		aabb__acceleration_structure_info.min = vec3(-0.5, -0.5, -0.5);

		AABBAccelerationStructure *aabb_acceleration_structure = Resources::createAABBAccelerationStructure(aabb__acceleration_structure_info, "aabbas");


		std::vector<VertexBindingInfo> vertex_binding_infos{};
		vertex_binding_infos.push_back(VertexBindingInfo{0, sizeof(vec3), VERTEX_BINDING_FLAG_NONE});
		MeshInfo mesh_info{};
		mesh_info.flags = MESH_FLAG_USED_IN_RAYTRACING;
		mesh_info.binding_infos = vertex_binding_infos.data();
		mesh_info.binding_info_count = vertex_binding_infos.size();
		Mesh *mesh = Resources::createMesh(mesh_info, "mesh");

		Geometry::createCube(*mesh, 2, 2, 2, VERTEX_FLAG_NONE);

		MeshAccelerationStructureInfo mesh_acceleration_structure_info{};
		mesh_acceleration_structure_info.mesh = mesh;

		MeshAccelerationStructure *mesh_acceleration_structure = Resources::createMeshAccelerationStructure(mesh_acceleration_structure_info, "meshas");


		std::vector<AccelerationStructureInstance> acceleration_structure_instances{};

		Transform transform_mesh{};
		transform_mesh.translate(vec3(0, 0, 5));

		Transform transform_aabb{};
		transform_aabb.translate(vec3(0, 0, -5));

		acceleration_structure_instances.push_back(AccelerationStructureInstance{0, transform_aabb.world(), ACCELERATION_STRUCTURE_TYPE_AABB});
		acceleration_structure_instances.push_back(AccelerationStructureInstance{1, transform_mesh.world(), ACCELERATION_STRUCTURE_TYPE_MESH});


		RootAccelerationStructureInfo root_acceleration_structure_info{};
		root_acceleration_structure_info.aabb_acceleration_structures = &aabb_acceleration_structure;
		root_acceleration_structure_info.aabb_acceleration_structure_count = 1;
		root_acceleration_structure_info.mesh_acceleration_structures = &mesh_acceleration_structure;
		root_acceleration_structure_info.mesh_acceleration_structure_count = 1;
		root_acceleration_structure_info.instances = acceleration_structure_instances.data();
		root_acceleration_structure_info.instance_count = acceleration_structure_instances.size();
	}
};


