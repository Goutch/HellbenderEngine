#pragma once

#include "Core.h"
#include <core/entity/component/Component.h>
class Material;
class Model;
class HB_API ModelRenderer : public Component {
    const Model *model;

public:
    void onAttach() override;

    void onDraw() override;

    void setModel(const Model &model);
    const Model* getModel();

    void serialize(Serializer* serializer) const override;

    std::string toString()const override;
};