#pragma once

#include <Radium/Nodes/Script.hpp>
#include <Radium/Nodes/Tree.hpp>
#include <string>
#undef Bool
#include <Radium/pch.hpp>

namespace Radium::Nodes {

    /**
     * @class ChaiScript
     * @brief Script implementation using ChaiScript for scripting support.
     * 
     * Loads, manages, and executes ChaiScript scripts attached to scene nodes.
     */
    class ChaiScript : public Script {
    public:
        /**
         * @brief Constructs a ChaiScript instance.
         * 
         * Loads the script from the specified file path and attaches it to the given scene tree.
         * Optionally, the loading can be stubbed (not fully loaded) for editor or preview purposes.
         * 
         * @param path Path to the ChaiScript script file.
         * @param tree Pointer to the SceneTree the script will be attached to.
         * @param realLoad Whether to perform a real script load (default is true).
         */
        ChaiScript(std::string path, SceneTree* tree, bool realLoad = true);

        /**
         * @brief Called once when the script is loaded.
         * 
         * Override of Script::OnLoad. Used to initialize or execute script logic on load.
         */
        void OnLoad() override;

        /**
         * @brief Called every tick to update the script.
         * 
         * Override of Script::OnTick.
         * 
         * @param dt Delta time since last tick in seconds.
         */
        void OnTick(float dt) override;

        /**
         * @brief Called every render frame to execute rendering-related script code.
         * 
         * Override of Script::OnRender.
         */
        void OnRender() override;

        /**
         * @brief Called during ImGui rendering to allow script-based UI interaction.
         * 
         * Override of Script::OnImgui.
         */
        void OnImgui() override;

        /**
         * @brief Get the path of the loaded script.
         */
        std::string path;

        /**
         * @brief Get the Node this script is attached to.
         * 
         * @return Pointer to the associated Node.
         */
        Node* GetMe();

        /**
         * @brief Get the SceneTree this script is part of.
         * 
         * @return Pointer to the associated SceneTree.
         */
        SceneTree* GetSceneTree();

        /**
         * @brief Set or change the Node this script is attached to.
         * 
         * @param node The new Node to attach this script to.
         */
        void SetMe(Node node);

        /**
         * @brief Register a global variable in the ChaiScript context.
         * 
         * This exposes a C++ variable to the script environment by name.
         * 
         * @tparam T Type of the variable.
         * @param me The variable to expose.
         * @param name The name to expose it as in the script.
         */
        template <typename T>
        void SetGlobal(T me, std::string name) {
            chai.add_global(chaiscript::var(me), name);
        }

    private:
        SceneTree* sceneTree; ///< Pointer to the associated scene tree.
        bool stubbed = false; ///< Whether the script is stubbed (not fully loaded).
        chaiscript::ChaiScript chai; ///< The embedded ChaiScript engine instance.
    };
}
