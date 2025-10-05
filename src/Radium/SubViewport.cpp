#include <Radium/SubViewport.hpp>
#include <Radium/SpriteBatchRegistry.hpp>

namespace Radium {
    SubViewport::SubViewport(int width, int height) : tree("SubViewport") {
        viewport = new Rune::Viewport(width, height);
    }

    void SubViewport::OnLoad() {
        tree.OnLoad();
    }

    void SubViewport::OnTick(float dt) {
        tree.OnTick(dt);
    }

    void SubViewport::OnRender() {
        // Create a render pass for this subviewport so sprite batches
        // rendered by the subscene have a valid activeRenderPass.
        viewport->SetupFrame();
        viewport->Activate();
        tree.OnRender();
        auto batches = Radium::SpriteBatchRegistry::GetAll();
        for (auto batch : batches) {
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