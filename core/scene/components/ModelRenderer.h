//
// Created by User on 3/22/2023.
//

#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/resource/Model.h"
namespace HBE
{

	struct HB_API ModelRenderer {
			Model *model = nullptr;
			uint32_t layer = 0;
			bool ordered = false;
			bool active = true;
	};
}
