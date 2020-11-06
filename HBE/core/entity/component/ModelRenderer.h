#pragma once


#include <core/entity/component/Component.h>
class Material;
class Model;
class ModelRenderer : public Component {
    const Model *model;

public:
    void onAttach() override;

    void onDraw() override;

    void setModel(const Model &model);
    const Model* getModel();

    void serialize(Serializer* serializer) const override;
};