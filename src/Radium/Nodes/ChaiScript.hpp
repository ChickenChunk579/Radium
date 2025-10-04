#pragma once
#include <Radium/Nodes/Script.hpp>
#include <string>
#undef Bool
#include <Radium/pch.hpp>

namespace Radium::Nodes {
    class ChaiScript: public Script {
    public:
        ChaiScript(std::string path);

        void OnLoad() override;
        void OnTick(float dt) override;
        void OnRender() override;
        void OnImgui() override;

        Node* GetMe();
        void SetMe(Node node);

        template <typename T>
        void SetGlobal(T me, std::string name) {
            chai.add_global(chaiscript::var(me), name);
        }

    private:
        chaiscript::ChaiScript chai;
    };
}
