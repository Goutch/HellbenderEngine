
#include "ModelRenderer.h"
#include <core/graphics/Graphics.h>
#include <core/utility/Log.h>
#include "core/entity/Entity.h"
#include "core/resource/Model.h"
#include "core/serialization/Serializer.h"
namespace HBE {
    void ModelRenderer::onAttach() {
        Component::onAttach();
        subscribeDraw();
    }

    void ModelRenderer::onDraw() {
        if (model) {
            auto meshes = model->getMeshes();
            for (std::size_t i = 0; i < meshes.size(); ++i) {
                Graphics::draw(*entity->transform, *meshes[i].first, *meshes[i].second);
            }
        } else {
            Log::warning("Model renderer " + entity->getName() + " does not have a model");
        }
    }

    void ModelRenderer::setModel(const Model &model) {
        this->model = &model;
    }

    const Model *ModelRenderer::getModel() {
        return model;
    }

    void ModelRenderer::serialize(Serializer *serializer) const {
        serializer->begin(toString());

        serializer->end();
    }

    std::string ModelRenderer::toString() const {
        return "ModelRenderer";
    }
}