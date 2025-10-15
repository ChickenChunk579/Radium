#include <Radium/Application.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/SubViewport.hpp>
#include <Radium/AssetLoader.hpp>
#include <Radium/GameDataParser.hpp>
#include <Radium/Camera.hpp>
#include <Radium/Input.hpp>
#include <Radium/Nodes/LuaScript.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include <Radium/Nodes/2D/RigidBody.hpp>
#include "imgui.h"
#include <Flux/Flux.hpp>
#include <mutex>
#include <vector>
#include <string>
#include <memory>
#include <Radium/imgui_impl_rune.h>
#include <Rune/Viewport.hpp>
#include <Rune/Texture.hpp>
#include <Rune/SpriteBatch.hpp>
#include <TextEditor.h>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <ImGuiFileDialog.h>
#include <filesystem>
#include <iostream>
#include <process.hpp>

namespace fs = std::filesystem;

std::string MakeRelativeToCWD(const std::string &filePathName)
{
    fs::path absPath = fs::absolute(filePathName); // Ensure it's absolute
    fs::path cwd = fs::current_path();             // Get current working directory

    fs::path relative = fs::relative(absPath, cwd);
    return relative.string();
}

// credit to https://github.com/simongeilfus/Cinder-ImGui
void imguiTheme()
{
    ImGuiStyle &mStyle = ImGui::GetStyle();

    mStyle.WindowMinSize = ImVec2(160, 20);
    mStyle.FramePadding = ImVec2(4, 2);
    mStyle.ItemSpacing = ImVec2(6, 2);
    mStyle.ItemInnerSpacing = ImVec2(6, 4);
    mStyle.Alpha = 0.95f;
    mStyle.WindowRounding = 4.0f;
    mStyle.FrameRounding = 2.0f;
    mStyle.IndentSpacing = 6.0f;
    mStyle.ItemInnerSpacing = ImVec2(2, 4);
    mStyle.ColumnsMinSpacing = 50.0f;
    mStyle.GrabMinSize = 14.0f;
    mStyle.GrabRounding = 16.0f;
    mStyle.ScrollbarSize = 12.0f;
    mStyle.ScrollbarRounding = 16.0f;
    mStyle.WindowBorderSize = 1.5f;

    ImGuiStyle &style = mStyle;
    style.Colors[ImGuiCol_Text] = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.17f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.65f, 0.20f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.92f, 0.18f, 0.29f, 0.50f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.92f, 0.18f, 0.29f, 1.0f); // Bright red outline, fully opaque
}

TextEditor::Palette RadiumPalette()
{
    using PI = TextEditor::PaletteIndex;
    TextEditor::Palette palette;

    palette[(int)PI::Default] = IM_COL32(220, 220, 220, 255);            // neutral light gray
    palette[(int)PI::Keyword] = IM_COL32(235, 46, 71, 255);              // red (from theme: 0.92f, 0.18f, 0.29f)
    palette[(int)PI::Number] = IM_COL32(147, 220, 236, 255);             // cyan/teal
    palette[(int)PI::String] = IM_COL32(200, 235, 153, 255);             // light yellow-green
    palette[(int)PI::CharLiteral] = IM_COL32(255, 198, 109, 255);        // orange-ish
    palette[(int)PI::Punctuation] = IM_COL32(220, 220, 220, 255);        // neutral
    palette[(int)PI::Preprocessor] = IM_COL32(235, 46, 71, 255);         // red
    palette[(int)PI::Identifier] = IM_COL32(178, 220, 245, 255);         // soft cyan
    palette[(int)PI::KnownIdentifier] = IM_COL32(106, 200, 255, 255);    // blue-cyan
    palette[(int)PI::PreprocIdentifier] = IM_COL32(255, 109, 122, 255);  // soft red-pink
    palette[(int)PI::Comment] = IM_COL32(144, 153, 163, 255);            // gray-blue
    palette[(int)PI::MultiLineComment] = IM_COL32(144, 153, 163, 255);   // gray-blue
    palette[(int)PI::Background] = IM_COL32(34, 36, 44, 255);            // dark background (0.13f, 0.14f, 0.17f)
    palette[(int)PI::Cursor] = IM_COL32(255, 255, 255, 255);             // white
    palette[(int)PI::Selection] = IM_COL32(235, 46, 71, 110);            // red + alpha (ImGuiCol_TextSelectedBg)
    palette[(int)PI::ErrorMarker] = IM_COL32(255, 0, 0, 255);            // bright red
    palette[(int)PI::Breakpoint] = IM_COL32(255, 50, 50, 255);           // bright red
    palette[(int)PI::LineNumber] = IM_COL32(128, 128, 128, 255);         // medium gray
    palette[(int)PI::CurrentLineFill] = IM_COL32(64, 64, 64, 0);         // subtle background
    palette[(int)PI::CurrentLineFillInactive] = IM_COL32(48, 48, 48, 0); // slightly dimmer
    palette[(int)PI::CurrentLineEdge] = IM_COL32(60, 60, 60, 0);         // line edge indicator

    return palette;
}

static bool TokenizeCStyleString(const char *in_begin, const char *in_end, const char *&out_begin, const char *&out_end)
{
    const char *p = in_begin;

    if (*p == '"')
    {
        p++;

        while (p < in_end)
        {
            // handle end of string
            if (*p == '"')
            {
                out_begin = in_begin;
                out_end = p + 1;
                return true;
            }

            // handle escape character for "
            if (*p == '\\' && p + 1 < in_end && p[1] == '"')
                p++;

            p++;
        }
    }

    return false;
}

static bool TokenizeCStyleCharacterLiteral(const char *in_begin, const char *in_end, const char *&out_begin, const char *&out_end)
{
    const char *p = in_begin;

    if (*p == '\'')
    {
        p++;

        // handle escape characters
        if (p < in_end && *p == '\\')
            p++;

        if (p < in_end)
            p++;

        // handle end of character literal
        if (p < in_end && *p == '\'')
        {
            out_begin = in_begin;
            out_end = p + 1;
            return true;
        }
    }

    return false;
}

static bool TokenizeCStyleIdentifier(const char *in_begin, const char *in_end, const char *&out_begin, const char *&out_end)
{
    const char *p = in_begin;

    if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '_')
    {
        p++;

        while ((p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_'))
            p++;

        out_begin = in_begin;
        out_end = p;
        return true;
    }

    return false;
}

static bool TokenizeCStyleNumber(const char *in_begin, const char *in_end, const char *&out_begin, const char *&out_end)
{
    const char *p = in_begin;

    const bool startsWithNumber = *p >= '0' && *p <= '9';

    if (*p != '+' && *p != '-' && !startsWithNumber)
        return false;

    p++;

    bool hasNumber = startsWithNumber;

    while (p < in_end && (*p >= '0' && *p <= '9'))
    {
        hasNumber = true;

        p++;
    }

    if (hasNumber == false)
        return false;

    bool isFloat = false;
    bool isHex = false;
    bool isBinary = false;

    if (p < in_end)
    {
        if (*p == '.')
        {
            isFloat = true;

            p++;

            while (p < in_end && (*p >= '0' && *p <= '9'))
                p++;
        }
        else if (*p == 'x' || *p == 'X')
        {
            // hex formatted integer of the type 0xef80

            isHex = true;

            p++;

            while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
                p++;
        }
        else if (*p == 'b' || *p == 'B')
        {
            // binary formatted integer of the type 0b01011101

            isBinary = true;

            p++;

            while (p < in_end && (*p >= '0' && *p <= '1'))
                p++;
        }
    }

    if (isHex == false && isBinary == false)
    {
        // floating point exponent
        if (p < in_end && (*p == 'e' || *p == 'E'))
        {
            isFloat = true;

            p++;

            if (p < in_end && (*p == '+' || *p == '-'))
                p++;

            bool hasDigits = false;

            while (p < in_end && (*p >= '0' && *p <= '9'))
            {
                hasDigits = true;

                p++;
            }

            if (hasDigits == false)
                return false;
        }

        // single precision floating point type
        if (p < in_end && *p == 'f')
            p++;
    }

    if (isFloat == false)
    {
        // integer size type
        while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L'))
            p++;
    }

    out_begin = in_begin;
    out_end = p;
    return true;
}

static bool TokenizeCStylePunctuation(const char *in_begin, const char *in_end, const char *&out_begin, const char *&out_end)
{
    (void)in_end;

    switch (*in_begin)
    {
    case '[':
    case ']':
    case '{':
    case '}':
    case '!':
    case '%':
    case '^':
    case '&':
    case '*':
    case '(':
    case ')':
    case '-':
    case '+':
    case '=':
    case '~':
    case '|':
    case '<':
    case '>':
    case '?':
    case ':':
    case '/':
    case ';':
    case ',':
    case '.':
        out_begin = in_begin;
        out_end = in_begin + 1;
        return true;
    }

    return false;
}

class Editor : public Radium::Application
{
public:
    TextEditor *editor;
    Radium::SubViewport *scene;
    ImTextureID sceneTexture;
    Radium::Nodes::Node *selectedNode = nullptr;
    std::string projectFolder = "none";
    json copiedNodeJson = "";

    std::vector<std::string> directories;
    std::vector<std::string> files;

    void RefreshAssets(const std::string &path)
    {
        files.clear();
        directories.clear();

        std::filesystem::path basePath = path;

        try
        {
            for (const auto &entry : std::filesystem::directory_iterator(basePath))
            {
                std::filesystem::path relativePath = std::filesystem::relative(entry.path(), basePath);

                if (entry.is_regular_file())
                {
                    files.push_back(relativePath.generic_string());
                }
                else if (entry.is_directory())
                {
                    directories.push_back(relativePath.generic_string());
                }
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            Flux::Error("Failed to refresh assets at '{}': {}", path, e.what());
        }
    }

    std::string GetTitle() override
    {
        return "Radium Editor";
    }

    Radium::Vector2i GetPreferredSize() override
    {
        return {1280, 720};
    }

    Radium::Vector2f GetGravity() override
    {
        return {0.0f, -200.0f};
    }

    void OnLoad() override
    {
        scene = new Radium::SubViewport(1280, 720);
        Rune::Texture *tex = new Rune::Texture(scene->viewport->textureView, Rune::SamplingMode::Nearest);

        Radium::Nodes::Node::Register();
        Radium::Nodes::Node2D::Register();
        Radium::Nodes::Sprite2D::Register();
        Radium::Nodes::RigidBody::Register();
        Radium::Camera::Register();

        Radium::Nodes::ClassDB::RegisterEnum<Radium::Nodes::CoordinateOrigin>();
        Radium::Nodes::ClassDB::RegisterEnum<Radium::Nodes::BodyType>();
        Radium::Nodes::ClassDB::RegisterEnum<Radium::Nodes::CollisionType>();

        scene->OnLoad();

        sceneTexture = ImGui_ImplRune_TextureToID(tex);

        editor = new TextEditor();
        
        editor->SetPalette(RadiumPalette());
        editor->SetShowWhitespaces(false);
    }

    void OnEarlyLoad() override
    {
        // if (spdlog::default_logger())
        //{
        //     spdlog::default_logger()->sinks().push_back(sink);
        // }
    }

    bool isDraggingCamera = false;
    Radium::Vector2f lastMousePos;
    Radium::Vector2f cameraVelocity{0.0f, 0.0f};

    void OnTick(float dt) override
    {
        scene->tree.UpdateAllGlobalPositions();

        // Camera movement with middle mouse button
        Radium::Vector2f currentMousePos = Radium::Input::GetMousePosition();
        
        if (Radium::Input::IsMiddleMouseButtonDown())
        {
            if (!isDraggingCamera)
            {
                // Start dragging
                isDraggingCamera = true;
                lastMousePos = currentMousePos;
            }
            else
            {
                // Calculate mouse delta
                Radium::Vector2f delta = currentMousePos - lastMousePos;
                
                // Update camera offset (negative because we're moving the world, not the camera)
                Radium::currentApplication->GetCamera()->offset.x -= delta.x;
                Radium::currentApplication->GetCamera()->offset.y -= -delta.y;
                
                // Calculate velocity for momentum (optional)
                cameraVelocity = delta * 10.0f;
            }
            
            lastMousePos = currentMousePos;
        }
        else
        {
            if (isDraggingCamera)
            {
                isDraggingCamera = false;
            }
            
            if (cameraVelocity.x != 0.0f || cameraVelocity.y != 0.0f)
            {
                Radium::currentApplication->GetCamera()->offset.x -= cameraVelocity.x * dt;
                Radium::currentApplication->GetCamera()->offset.y -= -cameraVelocity.y * dt;
                
                // Decelerate
                float deceleration = 0.9f;
                cameraVelocity.x *= deceleration;
                cameraVelocity.y *= deceleration;
                
                // Stop if velocity is very small
                if (std::abs(cameraVelocity.x) < 0.1f) cameraVelocity.x = 0.0f;
                if (std::abs(cameraVelocity.y) < 0.1f) cameraVelocity.y = 0.0f;
            }
        }
    }

    void OnPreRender() override
    {
        scene->OnRender();
    }

    void OnRender() override
    {
    }

    bool IsDescendantOf(Radium::Nodes::Node *parent, Radium::Nodes::Node *possibleChild)
    {
        if (!possibleChild)
            return false;
        if (possibleChild == parent)
            return true;

        for (auto *child : possibleChild->children)
        {
            if (IsDescendantOf(parent, child))
                return true;
        }

        return false;
    }

    void DrawNode(Radium::Nodes::Node *node)
    {
        ImGui::PushID(node);

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

        const bool isLeaf = node->children.empty();
        if (isLeaf)
        {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }
        if (node == selectedNode)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool open = ImGui::TreeNodeEx((void *)(intptr_t)node, flags, "%s", node->name.c_str());

        // Select node on click only (not drag)
        if (ImGui::IsItemClicked() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            selectedNode = node;
        }

        // === Drag Source ===
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("RADIUM_NODE", &node, sizeof(node));
            ImGui::Text("Move %s", node->name.c_str());
            ImGui::EndDragDropSource();
        }

        // === Drop Target ===
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("RADIUM_NODE"))
            {
                Radium::Nodes::Node *dragged = *(Radium::Nodes::Node **)payload->Data;

                if (dragged != node && dragged->parent != node && !IsDescendantOf(dragged, node))
                {
                    // Detach from old parent
                    if (dragged->parent)
                    {
                        auto &siblings = dragged->parent->children;
                        siblings.erase(std::remove(siblings.begin(), siblings.end(), dragged), siblings.end());
                    }

                    // Reassign parent
                    dragged->parent = node;
                    node->children.push_back(dragged);

                    // If dragged node was a root node, remove it from the root list
                    auto &roots = scene->tree.nodes;
                    roots.erase(std::remove(roots.begin(), roots.end(), dragged), roots.end());
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Only draw children and TreePop if this node was actually opened (and not a leaf)
        if (open && !isLeaf)
        {
            for (auto &child : node->children)
            {
                DrawNode(child);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void ImGuiTextEdit(const std::string &label, std::string &val)
    {
        // Use a persistent per-node+property buffer so multiple InputText fields
        // don't share the same backing memory (which causes collisions and
        // ImGui ID warnings). Keyed by selectedNode pointer + property label.
        static std::unordered_map<std::string, std::vector<char>> s_textBuffers;

        // Build a unique key per selected node and property label
        std::string nodeKey = "null";
        if (selectedNode)
            nodeKey = std::to_string((uintptr_t)selectedNode);
        std::string key = nodeKey + ":" + label;

        auto &buf = s_textBuffers[key];
        if (buf.empty())
        {
            // initialize buffer with existing value; keep a reasonable reserve
            size_t desired = std::max<size_t>(256, val.size() + 1);
            buf.assign(desired, '\0');
            strncpy(buf.data(), val.c_str(), desired - 1);
        }

        // Ensure unique ImGui id (PushID) so labels may be reused across nodes
        ImGui::PushID(key.c_str());
        bool changed = ImGui::InputText(label.c_str(), buf.data(), buf.size());
        ImGui::PopID();

        if (changed)
        {
            val = std::string(buf.data());
        }
    }

    bool endsWith(const std::string &str, const std::string &suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    void OnImgui() override
    {
        static int dialogState = 0;
        static std::string openPath = "";

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        { // => will show a dialog
            if (ImGuiFileDialog::Instance()->IsOk())
            { // action if OK
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                std::string currentFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
                // action

                std::string relativePath = MakeRelativeToCWD(filePathName);

                switch (dialogState)
                {
                case 0: // new
                    openPath = relativePath;
                    scene->tree.nodes.clear();
                    break;

                case 1: // open
                    openPath = relativePath;
                    Flux::Info("Open path: {}", openPath);
                    scene->tree.Deserialize(relativePath, true, true);
                    break;

                case 2: // save
                    scene->tree.Serialize(relativePath);
                    openPath = relativePath;
                    break;

                case 3: // add script
                {
                    std::string finalPath = filePathName;
                    fs::path relativePath = fs::relative(finalPath, projectFolder);
                    selectedNode->script = new Radium::Nodes::LuaScript(relativePath.generic_string(), &scene->tree, false);
                    selectedNode->script->me = selectedNode;
                    break;
                }

                case 4: // open project
                {
                    projectFolder = filePath;

                    Radium::assetBase = projectFolder + "/";

                    Flux::Info("Opened project: {}", projectFolder);

                    RefreshAssets(projectFolder);

                    std::string appConfig = Radium::ReadFileToString("app.json");
                    json j = json::parse(appConfig);
                    auto config = j.get<Radium::GameConfig>();

                    Radium::SpriteBatchRegistry::Clear();

                    for (auto batchInfo : config.spriteBatches)
                    {
                        Radium::SpriteBatchRegistry::Add(batchInfo.tag, batchInfo.path, batchInfo.origin, Rune::SamplingMode::Nearest);
                    }

                    break;
                }

                default:
                    break;
                }

                Flux::Info("dialogState: {}", dialogState);
                Flux::Info("openPath: {}", openPath);
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
        {
            ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            ImGui::Begin("Radium Editor", NULL, window_flags);
            ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New", "Ctrl+N", false))
                    {
                        dialogState = 0;
                        IGFD::FileDialogConfig config;
                        config.path = ".";
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose Path", ".json", config);
                    }
                    if (ImGui::MenuItem("Open", "Ctrl+O", false))
                    {
                        dialogState = 1;
                        IGFD::FileDialogConfig config;
                        config.path = ".";
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Open JSON Scene", ".json", config);
                    }
                    if (ImGui::MenuItem("Save", "Ctrl+S", false))
                    {
                        if (openPath == "")
                        {
                            dialogState = 2;
                            IGFD::FileDialogConfig config;
                            config.path = ".";
                            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Save JSON Scene", ".json", config);
                        }
                        else
                        {
                            Flux::Info("Open path {}", openPath);
                            scene->tree.Serialize((fs::path(projectFolder) / openPath).generic_string());
                        }
                    }
                    if (ImGui::MenuItem("Close", "Ctrl+Q", false))
                    {
                        exit(0);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Project"))
                {
                    if (ImGui::MenuItem("Open", "Ctrl+Shift+O", false))
                    {
                        dialogState = 4;
                        IGFD::FileDialogConfig config;
                        config.path = ".";
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Select app.json file", ".json", config);
                    }
                    if (ImGui::MenuItem("Run", "Ctrl+R", false))
                    {
                        TinyProcessLib::Process lsProcess(std::filesystem::current_path().string() + "/RadiumRuntime .", projectFolder,
                                                          [](const char *bytes, size_t n)
                                                          {
                                                              std::cout << std::string(bytes, n);
                                                          });
                    }
                    if (ImGui::MenuItem("Refresh Batches", NULL, false))
                    {
                        std::string appConfig = Radium::ReadFileToString(projectFolder + "/app.json");
                        json j = json::parse(appConfig);
                        auto config = j.get<Radium::GameConfig>();

                        Radium::SpriteBatchRegistry::Clear();

                        for (auto batchInfo : config.spriteBatches)
                        {
                            Radium::SpriteBatchRegistry::Add(batchInfo.tag, batchInfo.path, batchInfo.origin, Rune::SamplingMode::Nearest);
                        }
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            // Add this OUTSIDE the menu bar (after ImGui::End() of the dockspace window)
            // Check for keyboard shortcuts
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
            {
                if (ImGui::IsKeyPressed(ImGuiKey_S, false))
                {
                    // Save
                    if (openPath == "")
                    {
                        dialogState = 2;
                        IGFD::FileDialogConfig config;
                        config.path = ".";
                        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Save JSON Scene", ".json", config);
                    }
                    else
                    {
                        Flux::Info("Open path {}", openPath);
                        scene->tree.Serialize((fs::path(projectFolder) / openPath).generic_string());
                    }
                }
                if (ImGui::IsKeyPressed(ImGuiKey_Q, false))
                {
                    // Close
                    exit(0);
                }
                if (ImGui::IsKeyPressed(ImGuiKey_R, false))
                {
                    // Run
                    TinyProcessLib::Process lsProcess(std::filesystem::current_path().string() + "/RadiumRuntime .", projectFolder,
                                                      [](const char *bytes, size_t n)
                                                      {
                                                          std::cout << std::string(bytes, n);
                                                      });
                }

                // Ctrl+Shift+O for Open Project
                if ((ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) && ImGui::IsKeyPressed(ImGuiKey_O, false))
                {
                    dialogState = 4;
                    IGFD::FileDialogConfig config;
                    config.path = ".";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Select app.json file", ".json", config);
                }
                // Ctrl+O (without shift) for Open Scene
                else if (ImGui::IsKeyPressed(ImGuiKey_O, false))
                {
                    dialogState = 1;
                    IGFD::FileDialogConfig config;
                    config.path = ".";
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Open JSON Scene", ".json", config);
                }
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Inspector");

            if (selectedNode)
            {
                ImGuiTextEdit("Name", selectedNode->name);

                ImGui::Separator();

                // Script picker
                if (selectedNode->script)
                {
                    Radium::Nodes::LuaScript *script = reinterpret_cast<Radium::Nodes::LuaScript *>(selectedNode->script);
                    if (script != nullptr)
                        ImGui::Text("Script: %s", script->path.c_str());

                    if (ImGui::Button("Remove"))
                    {
                        selectedNode->script = NULL;
                    }
                }
                else
                {
                    ImGui::Text("No Script Attached");
                }

                if (ImGui::Button("Select a new Lua script"))
                {
                    dialogState = 3;
                    IGFD::FileDialogConfig config;
                    config.path = projectFolder;
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose a lua script", ".lua", config);
                }

                ImGui::Separator();

                for (auto property : Radium::Nodes::ClassDB::GetProperties(selectedNode))
                {
                    ImGui::PushID(selectedNode);
                    ImGui::PushID(property.name.c_str());
                    if (property.name == "name")
                    {
                        ImGui::PopID();
                        ImGui::PopID();
                        continue;
                    }
                    if (Radium::Nodes::ClassDB::IsEnum(property.type))
                    {
                        int *f = Radium::Nodes::ClassDB::GetPropertyPointer<int>(property.name, selectedNode);
                        ImGui::InputInt(property.name.c_str(), f);
                    }
                    if (property.type == "Radium::Vector2f")
                    {
                        Radium::Vector2f *value = Radium::Nodes::ClassDB::GetPropertyPointer<Radium::Vector2f>(property.name, selectedNode);

                        float val[2] = {value->x, value->y};

                        ImGui::InputFloat2(property.name.c_str(), val);

                        value->x = val[0];
                        value->y = val[1];
                    }
#if !defined(_MSC_VER)
                    if (property.type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >")
                    {
#else
                    if (property.type == "std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >")
                    {
#endif
                        // Edit the actual property string in-place. Use GetPropertyPointer so changes
                        // persist back to the node.
                        std::string *strPtr = Radium::Nodes::ClassDB::GetPropertyPointer<std::string>(property.name, selectedNode);
                        if (strPtr)
                        {
                            ImGuiTextEdit(property.name, *strPtr);
                        }
                    }
                    if (property.type == "float")
                    {
                        float *f = Radium::Nodes::ClassDB::GetPropertyPointer<float>(property.name, selectedNode);
                        ImGui::InputFloat(property.name.c_str(), f);
                    }
                    if (property.type == "unsigned int")
                    {
                        unsigned int *f = Radium::Nodes::ClassDB::GetPropertyPointer<unsigned int>(property.name, selectedNode);
                        ImGui::InputScalar(property.name.c_str(), ImGuiDataType_U32, f);
                    }
                    if (property.type == "int")
                    {
                        int *f = Radium::Nodes::ClassDB::GetPropertyPointer<int>(property.name, selectedNode);
                        ImGui::InputInt(property.name.c_str(), f);
                    }
                    if (property.type == "bool")
                    {
                        bool *f = Radium::Nodes::ClassDB::GetPropertyPointer<bool>(property.name, selectedNode);
                        ImGui::Checkbox(property.name.c_str(), f);
                    }
                    if (property.type == "Radium::RectangleF")
                    {
                        Radium::RectangleF *value = Radium::Nodes::ClassDB::GetPropertyPointer<Radium::RectangleF>(property.name, selectedNode);

                        float pos[2] = {value->x, value->y};
                        float size[2] = {value->w, value->h};

                        if (ImGui::CollapsingHeader(property.name.c_str()))
                        {
                            ImGui::Indent();
                            ImGui::InputFloat2("Position", pos);
                            ImGui::InputFloat2("Size", size);
                            ImGui::Unindent();

                            value->x = pos[0];
                            value->y = pos[1];
                            value->w = size[0];
                            value->h = size[1];
                        }
                    }
                    if (property.type == "Radium::Nodes::Node*")
                    {
                        Radium::Nodes::Node **ptr = Radium::Nodes::ClassDB::GetPropertyPointer<Radium::Nodes::Node *>(property.name, selectedNode);

                        if (ptr && *ptr)
                            ImGui::Text("%s: %s", property.name.c_str(), (*ptr)->name.c_str());
                        else
                            ImGui::Text("%s: <None>", property.name.c_str());

                        // Allow dropping node into this slot
                        if (ImGui::BeginDragDropTarget())
                        {
                            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("RADIUM_NODE"))
                            {
                                Flux::Info("Got payload");
                                Radium::Nodes::Node *dropped = *(Radium::Nodes::Node **)payload->Data;

                                if (ptr && dropped != selectedNode)
                                {
                                    *ptr = dropped;
                                }
                            }
                            ImGui::EndDragDropTarget();
                        }
                    }

                    ImGui::PopID();
                    ImGui::PopID();
                }
            }

            ImGui::End();
        }

        {
            static std::string folder = "";
            std::string expandedFolder = (fs::path(projectFolder) / folder).generic_string();

            ImGui::Begin("Assets");

            if (projectFolder == "none")
            {
                ImGui::Text("No project is open :(");
            }
            else
            {
                ImGui::Text(folder.c_str());
                ImGui::SameLine();
                if (ImGui::Button("Reload"))
                {
                    RefreshAssets(expandedFolder);
                }
                ImGui::Separator();

                int i = 0;

                if (folder != "/")
                {
                    if (i % 2 == 0)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                    }
                    if (ImGui::Selectable("[Back]"))
                    {
                        fs::path newFolder = fs::path(folder).parent_path();
                        folder = newFolder.generic_string();
                        RefreshAssets(projectFolder + folder);
                    }

                    ImGui::PopStyleColor(3);
                }

                for (auto dir : directories)
                {
                    if (i % 2 == 0)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                    }
                    if (ImGui::Selectable(dir.c_str()))
                    {
                        fs::path combined = fs::path(projectFolder) / folder / dir;
                        folder = (fs::path(folder) / dir).generic_string();

                        RefreshAssets(combined.generic_string());
                    }
                    ImGui::PopStyleColor(3);
                }

                for (auto file : files)
                {
                    if (i % 2 == 0)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                    }
                    if (ImGui::Selectable(file.c_str()))
                    {
                        if (endsWith(file, ".rscn"))
                        {
                            if (openPath != "")
                                scene->tree.Serialize(openPath);
                            fs::path path = fs::path(folder) / file;
                            Flux::Info("Scene: {}", path.generic_string());
                            scene->tree.Deserialize(path.generic_string(), true, true);
                            openPath = path.generic_string();
                        }
                    }
                    ImGui::PopStyleColor(3);
                }
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Hierarchy");

            if (selectedNode)
            {
                if (ImGui::Button("Copy"))
                {
                    // Serialize selected node to string
                    copiedNodeJson = Radium::Nodes::SerializeNode(selectedNode); // Assumes you have SerializeToString()
                    Flux::Info("Copied node: {}", selectedNode->name);
                }
                ImGui::SameLine();
                if (ImGui::Button("Paste"))
                {
                    if (!copiedNodeJson.empty())
                    {
                        // Deserialize the node
                        Radium::Nodes::Node *cloned = Radium::Nodes::DeserializeNode(copiedNodeJson, &scene->tree, nullptr, true); // You’ll implement this
                        if (cloned)
                        {
                            cloned->name += "_copy";
                            scene->tree.nodes.push_back(cloned);
                            selectedNode = cloned;
                            Flux::Info("Pasted node as: {}", cloned->name);
                        }
                        else
                        {
                            Flux::Error("Failed to paste node.");
                        }
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Up"))
                {
                    auto &siblings = (selectedNode->parent ? selectedNode->parent->children : scene->tree.nodes);
                    auto it = std::find(siblings.begin(), siblings.end(), selectedNode);
                    if (it != siblings.begin() && it != siblings.end())
                    {
                        std::iter_swap(it, it - 1);
                        Flux::Info("Moved node up: {}", selectedNode->name);
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button("Down"))
                {
                    auto &siblings = (selectedNode->parent ? selectedNode->parent->children : scene->tree.nodes);
                    auto it = std::find(siblings.begin(), siblings.end(), selectedNode);
                    if (it != siblings.end() && (it + 1) != siblings.end())
                    {
                        std::iter_swap(it, it + 1);
                        Flux::Info("Moved node down: {}", selectedNode->name);
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button("Delete"))
                {
                    auto it = std::find(scene->tree.nodes.begin(), scene->tree.nodes.end(), selectedNode);
                    if (it != scene->tree.nodes.end())
                    {
                        scene->tree.nodes.erase(it);
                    }
                }
            }

            static bool openMenu = false;
            static char filter[128] = "";
            static int selectedItem = -1;
            static std::vector<std::string> items = Radium::Nodes::ClassDB::GetNodeClasses();

            if (ImGui::Button("Add Node"))
            {
                openMenu = true;
                filter[0] = '\0'; // reset filter
                selectedItem = -1;
                ImGui::OpenPopup("RofiMenu");
            }

            if (ImGui::BeginPopup("RofiMenu"))
            {
                ImGui::InputText("##Filter", filter, IM_ARRAYSIZE(filter));

                ImGui::Separator();

                ImGui::BeginChild("ScrollRegion", ImVec2(0, 150), true);

                int index = 0;
                for (const auto &item : items)
                {
                    if (strlen(filter) > 0 && item.find(filter) == std::string::npos)
                        continue;

                    if (ImGui::Selectable(item.c_str()))
                    {
                        selectedItem = index;
                        openMenu = false;
                        ImGui::CloseCurrentPopup();
                    }

                    ++index;
                }

                ImGui::EndChild();
                ImGui::EndPopup();
            }

            if (selectedItem != -1)
            {
                std::string selected = items[selectedItem];

                selectedItem = -1;

                Flux::Info("Selected: {}", selected);

                auto node = (Radium::Nodes::Node *)Radium::Nodes::ClassDB::Create(selected);

                scene->tree.nodes.push_back(node);
            }

            for (auto node : scene->tree.nodes)
            {
                DrawNode(node);
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Console");

            ImGui::Separator();

            ImGui::End();
        }
        {
            ImGui::Begin("Viewport");

            // Get available content region inside the window
            ImVec2 avail = ImGui::GetContentRegionAvail();

            // Fallback in case viewport size is invalid
            int imgW = (scene && scene->viewport->width > 0) ? scene->viewport->width : 1280;
            int imgH = (scene && scene->viewport->height > 0) ? scene->viewport->height : 720;

            float imageAspect = (float)imgW / (float)imgH;

            // Compute render size that fits the available region while keeping the aspect ratio
            ImVec2 renderSize;
            if (avail.y <= 0.0f || avail.x <= 0.0f)
            {
                renderSize = ImVec2((float)imgW, (float)imgH);
            }
            else
            {
                float availAspect = avail.x / avail.y;
                if (availAspect > imageAspect)
                {
                    // available region is wider than image -> fit by height
                    renderSize.y = avail.y;
                    renderSize.x = renderSize.y * imageAspect;
                }
                else
                {
                    // fit by width
                    renderSize.x = avail.x;
                    renderSize.y = renderSize.x / imageAspect;
                }
            }

            // Center the image inside the available region
            ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
            float offsetX = (avail.x - renderSize.x) * 0.5f;
            float offsetY = (avail.y - renderSize.y) * 0.5f;
            if (offsetX < 0.0f)
                offsetX = 0.0f;
            if (offsetY < 0.0f)
                offsetY = 0.0f;
            ImGui::SetCursorScreenPos(ImVec2(cursorScreenPos.x + offsetX, cursorScreenPos.y + offsetY));
            cursorScreenPos = ImGui::GetCursorScreenPos();

            ImGui::Image(sceneTexture, renderSize);

            // Move cursor to the end of the content region so following widgets are laid out correctly
            ImGui::SetCursorScreenPos(ImVec2(cursorScreenPos.x + avail.x, cursorScreenPos.y + avail.y));

            ImGui::Dummy({0, 0});
            ImGui::End();
        }

        {
            ImGui::Begin("Code Editor");

            editor->Render("Code Editor");

            ImGui::End();
        }

        ImGui::ShowDemoWindow();
    }
};
#ifndef __ANDROID__
RADIUM_ENTRYPOINT(Editor)
#else
extern "C" int SDL_main(int argc, char *argv[])
{
    Editor app;
    Radium::currentApplication = &app;
    app.Run();
    return 0;
}
#endif