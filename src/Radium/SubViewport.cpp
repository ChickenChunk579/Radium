#include <Radium/SubViewport.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Rune/Rune.hpp>
#include <Rune/SpriteBatch.hpp>

namespace Radium {
    SubViewport::SubViewport(int width, int height) : tree("SubViewport") {
        viewport = new Rune::Viewport(width, height);
    }

    SubViewport::~SubViewport() {
        delete viewport;
    }

    void SubViewport::OnLoad() {
        tree.OnLoad();
    }

    void SubViewport::OnTick(float dt) {
        tree.OnTick(dt);
    }

    void SubViewport::OnRender() {
        // Update sprite batches' resolution uniform so shaders use the subviewport size
        auto batchesPre = Radium::SpriteBatchRegistry::GetAll();
        for (auto batch : batchesPre) {
            if (batch && batch->resolutionBuffer) {
                Rune::OtherDataUniform resData = {
                    .screenWidth = (float)viewport->width,
                    .screenHeight = (float)viewport->height,
                    .spriteOrigin = static_cast<int>(batch->origin),
                    .pad = 0
                };
                // write the per-batch resolution buffer so the shader uses the subviewport size
                wgpuQueueWriteBuffer(Rune::queue, batch->resolutionBuffer, 0, &resData, sizeof(resData));
            }
        }

        // Create a render pass for this subviewport so sprite batches
        // rendered by the subscene have a valid activeRenderPass.
        viewport->SetupFrame();
        tree.OnRender();
        auto batches = Radium::SpriteBatchRegistry::GetAll();
        for (auto batch : batches) {
            if (!batch) {
                continue;
            }
            if (batch->started) {
                batch->End();
            }
        }
        // Finish the render pass created for this subviewport
        viewport->FinishFrame();
    }

    void SubViewport::OnImgui() {
        tree.OnImgui();
    }
}