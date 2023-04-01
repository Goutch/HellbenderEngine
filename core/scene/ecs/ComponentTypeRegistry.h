

#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/scene/components/Transform.h"
#include "core/scene/components/Camera2D.h"
#include "core/scene/components/MeshRenderer.h"
#include "core/scene/components/ModelRenderer.h"
#include "core/scene/components/Camera.h"
#include "core/scene/components/PixelCamera.h"
#include "core/scene/components/CameraController.h"
#include "core/scene/components/Transform2D.h"
namespace HBE
{
	class HB_API ComponentTypeRegistry {
		static uint32_t current_bit;
	public:
		template<typename T>
		size_t getSignatureBit() {
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<Transform>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<Camera2D>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<MeshRenderer>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<ModelRenderer>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<Camera>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<PixelCamera>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<CameraController>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}
		template<>
		size_t getSignatureBit<Transform2D>()
		{
			static const size_t bit = current_bit++;
			return bit;
		}

	};
}
