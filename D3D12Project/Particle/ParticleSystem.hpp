#pragma once

#include <d3d12.h>
#include <glm/glm.hpp>

class Scene;
class StorageBuffer;
class FrameBuffer;
class Camera;

class ParticleSystem
{
    public:
        // Constructor.
        // pDevice Pointer to D3D11 device.
        // pDeviceContext Pointer to D3D11 device context.
        ParticleSystem(/*ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext*/);

        // Destructor.
        ~ParticleSystem();

        // Update particles.
        // scene Scene to update.
        // dt Delta time.
        void Update(Scene* scene, float dt);

        // Render particles.
        // scene Scene to render.
        // camera Camera to render from.
        void Render(Scene* scene, Camera* camera);

    private:
        /*ID3D11Device* mpDevice;
        ID3D11DeviceContext* mpDeviceContext;

        ID3D11ComputeShader* mComputeShader;

        ID3D11VertexShader* mVertexShader;
        ID3D11GeometryShader* mGeometryShader;
        ID3D11PixelShader* mPixelShader;
        ID3D11BlendState* mBlendState;*/
        
       /* struct UpdateMetaData
        {
            float dt;
            unsigned int particleCount;
            float pad[6];
        } mUpdateMetaData;
        ID3D11ShaderResourceView* mUpdateMetaDataBuffer;

        struct RenderMetaData
        {
            glm::mat4 vpMatrix;
            glm::vec3 lensPosition;
            glm::vec3 lensUpDirection;
            float pad[2];
        } mRenderMetaData;
        ID3D11ShaderResourceView* mRenderMetaDataBuffer;*/
};
