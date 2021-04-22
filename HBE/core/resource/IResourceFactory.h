#include "IShader.h"

namespace HBE {

    class IGraphicPipeline;

    class ITexture;

    class IMesh;

    class IShader;

    class IResourceFactory {
    public:
        virtual ~IResourceFactory() = default;

        virtual IGraphicPipeline *createGraphicsPipeline() const = 0;

        virtual IShader *createShader() const = 0;

        virtual ITexture *createTexture() const = 0;

        virtual IMesh *createMesh() const = 0;
    };
}
