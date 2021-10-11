#include "Core.h"
#include "GLSL_TYPE.h"

namespace HBE {

    class GraphicPipeline;

    class Texture;

    class Mesh;

    class Shader;

    class ComputePipeline;

    class VertexLayout;

    class HB_API ResourceFactory {
    public:
        virtual ~ResourceFactory() = default;

        virtual GraphicPipeline *createGraphicsPipeline() const = 0;

        virtual Shader *createShader() const = 0;

        virtual Texture *createTexture() const = 0;

        virtual Mesh *createMesh() const = 0;

        virtual ComputePipeline *createComputePipeline() const = 0;

        virtual VertexLayout *createVertexLayout() const = 0;
    };
}
