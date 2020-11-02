//
// Created by User on 01-Nov.-2020.
//

#include "ModelRenderer.h"
#include <core/graphics/Graphics.h>
#include "core/entity/Entity.h"
#include "MeshRenderer.h"
#include "core/resource/Resource.h"

void ModelRenderer::onAttach() {
    Component::onAttach();
    subscribeDraw();
}

void ModelRenderer::onDraw() {
    if(model)
    {
        auto meshes = model->getMeshes();
        for (int i = 0; i < meshes.size(); ++i) {
            Graphics::draw(*entity, *meshes[i].first, *meshes[i].second);
        }
    }
    else
    {
        Log::warning("Model renderer "+getEntity()->getName()+" does not have a model");
    }
}

void ModelRenderer::setModel(const Model &model) {
    this->model = &model;
}

const Model *ModelRenderer::getModel() {
    return model;
}
