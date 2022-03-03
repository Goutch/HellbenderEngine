

#include <core/threading/JobManager.h>
#include "Model.h"
#include "Resources.h"

namespace HBE {
	const std::vector<std::pair<Mesh *, GraphicPipeline *>> &Model::getMeshes() const {
		return meshes;
	}

	Model::~Model() {
		clearMeshes();
	}


	Model *Model::load(std::string path) {
		this->path = path;
		Log::status("Loading:" + path);
		data_mutex.lock();
		ModelImporter::load(path, meshes_data);
		data_mutex.unlock();
		constructModel();
		return this;
	}

	void Model::setPipeline(GraphicPipeline *pipeline, int mesh_index) {
		if (meshes.size() <= (size_t) mesh_index)
			meshes.resize(mesh_index + 1);
		meshes[mesh_index].second = pipeline;
	}

	void Model::loadAsync(std::string path) {
		this->path = path;
		Log::status("Loading async:" + path);
		Log::warning("Not implemented: Loading async");

		/*auto job = JobManager::create<void,std::vector<std::pair<MeshData, MaterialData>>&, std::string>();
		job->setCallback<Model>(this, &Model::constructModel);

		job->run(&ModelImporter::load, path, meshes_data);		*/
	}

	void Model::constructModel() {
		uint32_t vertex_count = 0;
		clearMeshes();
		data_mutex.lock();
		for (std::size_t i = 0; i < meshes_data.size(); ++i) {
			//todo:info bindings
			MeshInfo mesh_info{};
			std::vector<VertexBindingInfo> binding_infos = {{0, sizeof(float) * 7, VERTEX_BINDING_FLAG_NONE}};
			meshes.emplace_back(Resources::createMesh(mesh_info), nullptr);
			meshes[i].first->setVertexIndices(meshes_data[i].first.indices);
			vertex_count += meshes_data[i].first.indices.size();

			const MeshData &mesh_data = meshes_data[i].first;
			const MaterialData &material_data = meshes_data[i].second;
			if (!mesh_data.positions.empty()) {
				meshes[i].first->setBuffer(0, mesh_data.positions.data(), mesh_data.positions.size());
			}
			if (!mesh_data.uvs.empty()) {
				meshes[i].first->setBuffer(1, mesh_data.uvs.data(), mesh_data.uvs.size());
			}
			if (!mesh_data.normals.empty()) {
				meshes[i].first->setBuffer(2, mesh_data.normals.data(), mesh_data.normals.size());
			}
			if (!material_data.diffuse_texture_paths.empty()) {
				auto t = Texture::load(material_data.diffuse_texture_paths[0]);
				meshes[i].second->setTexture(0, t);
			}
		}
		meshes_data.clear();
		data_mutex.unlock();
		Log::status(path + " loaded \n\tVertex count = " + std::to_string(vertex_count));

	}

	void Model::clearMeshes() {
		for (std::size_t i = 0; i < meshes_data.size(); ++i) {
			if (meshes[i].first)
				delete meshes[i].first;
			if (meshes[i].second)
				delete meshes[i].second;
		}
		meshes.clear();
	}

	const std::string &Model::getPath() {
		return path;
	}


}

