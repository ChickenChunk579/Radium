#include <Radium/Application.hpp>
#include <Radium/SpriteBatchRegistry.hpp>
#include <Radium/Nodes/ClassDB.hpp>
#include <Radium/SubViewport.hpp>
#include <Radium/Nodes/2D/Sprite2D.hpp>
#include "imgui.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <mutex>
#include <vector>
#include <string>
#include <memory>
#include <Radium/imgui_impl_rune.h>
#include <Rune/Viewport.hpp>
#include <Rune/Texture.hpp>
#include <TextEditor.h>
#include <unordered_map>
#include <cstdint>
#include <algorithm>

// Simple thread-safe console buffer used by the ImGui sink and UI
struct ImGuiConsole
{
    std::mutex mutex;
    std::vector<std::string> lines;
    bool autoScroll = true;

    void AddLine(const std::string &s)
    {
        std::lock_guard<std::mutex> lk(mutex);
        lines.emplace_back(s);
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lk(mutex);
        lines.clear();
    }

    std::vector<std::string> Snapshot()
    {
        std::lock_guard<std::mutex> lk(mutex);
        return lines;
    }
};

static ImGuiConsole g_imguiConsole;

// custom sink forwarding formatted messages to g_imguiConsole
class ImGuiSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
    ImGuiConsole *console;
    explicit ImGuiSink(ImGuiConsole *c) : console(c) {}

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override
    {
        spdlog::memory_buf_t formatted;
        base_sink<std::mutex>::formatter_->format(msg, formatted);
        std::string s(formatted.data(), formatted.size());
        if (console)
            console->AddLine(s);
    }

    void flush_() override {}
};

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

    palette[(int)PI::Default]              = IM_COL32(220, 220, 220, 255);  // neutral light gray
    palette[(int)PI::Keyword]              = IM_COL32(235, 46, 71, 255);    // red (from theme: 0.92f, 0.18f, 0.29f)
    palette[(int)PI::Number]               = IM_COL32(147, 220, 236, 255);  // cyan/teal
    palette[(int)PI::String]               = IM_COL32(200, 235, 153, 255);  // light yellow-green
    palette[(int)PI::CharLiteral]          = IM_COL32(255, 198, 109, 255);  // orange-ish
    palette[(int)PI::Punctuation]          = IM_COL32(220, 220, 220, 255);  // neutral
    palette[(int)PI::Preprocessor]         = IM_COL32(235, 46, 71, 255);    // red
    palette[(int)PI::Identifier]           = IM_COL32(178, 220, 245, 255);  // soft cyan
    palette[(int)PI::KnownIdentifier]      = IM_COL32(106, 200, 255, 255);  // blue-cyan
    palette[(int)PI::PreprocIdentifier]    = IM_COL32(255, 109, 122, 255);  // soft red-pink
    palette[(int)PI::Comment]              = IM_COL32(144, 153, 163, 255);  // gray-blue
    palette[(int)PI::MultiLineComment]     = IM_COL32(144, 153, 163, 255);  // gray-blue
    palette[(int)PI::Background]           = IM_COL32(34, 36, 44, 255);     // dark background (0.13f, 0.14f, 0.17f)
    palette[(int)PI::Cursor]               = IM_COL32(255, 255, 255, 255);  // white
    palette[(int)PI::Selection]            = IM_COL32(235, 46, 71, 110);    // red + alpha (ImGuiCol_TextSelectedBg)
    palette[(int)PI::ErrorMarker]          = IM_COL32(255, 0, 0, 255);      // bright red
    palette[(int)PI::Breakpoint]           = IM_COL32(255, 50, 50, 255);    // bright red
    palette[(int)PI::LineNumber]           = IM_COL32(128, 128, 128, 255);  // medium gray
    palette[(int)PI::CurrentLineFill]      = IM_COL32(64, 64, 64, 0);      // subtle background
    palette[(int)PI::CurrentLineFillInactive] = IM_COL32(48, 48, 48, 0);  // slightly dimmer
    palette[(int)PI::CurrentLineEdge]      = IM_COL32(60, 60, 60, 0);     // line edge indicator

    return palette;
}



static bool TokenizeCStyleString(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

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

static bool TokenizeCStyleCharacterLiteral(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

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

static bool TokenizeCStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

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

static bool TokenizeCStyleNumber(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
{
	const char * p = in_begin;

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

static bool TokenizeCStylePunctuation(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end)
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

const TextEditor::LanguageDefinition& ChaiScript() {
    static bool inited = false;
	static TextEditor::LanguageDefinition langDef;
	if (!inited)
	{
		static const char* const keywords[] = {
			"attr", "auto", "break", "def", "else",
            "for", "fun", "if", "try", "while", "var",
            "global"
		};
		for (auto& k : keywords)
			langDef.mKeywords.insert(k);

		static const char* const identifiers[] = {
			"this"
		};
		for (auto& k : identifiers)
		{
			TextEditor::Identifier id;
			id.mDeclaration = "Built-in function";
			langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
		}

		langDef.mTokenize = [](const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, TextEditor::PaletteIndex & paletteIndex) -> bool
		{
			paletteIndex = TextEditor::PaletteIndex::Max;

			while (in_begin < in_end && isascii(*in_begin) && isblank(*in_begin))
				in_begin++;

			if (in_begin == in_end)
			{
				out_begin = in_end;
				out_end = in_end;
				paletteIndex = TextEditor::PaletteIndex::Default;
			}
			else if (TokenizeCStyleString(in_begin, in_end, out_begin, out_end))
				paletteIndex = TextEditor::PaletteIndex::String;
			else if (TokenizeCStyleCharacterLiteral(in_begin, in_end, out_begin, out_end))
				paletteIndex = TextEditor::PaletteIndex::CharLiteral;
			else if (TokenizeCStyleIdentifier(in_begin, in_end, out_begin, out_end))
				paletteIndex = TextEditor::PaletteIndex::Identifier;
			else if (TokenizeCStyleNumber(in_begin, in_end, out_begin, out_end))
				paletteIndex = TextEditor::PaletteIndex::Number;
			else if (TokenizeCStylePunctuation(in_begin, in_end, out_begin, out_end))
				paletteIndex = TextEditor::PaletteIndex::Punctuation;

			return paletteIndex != TextEditor::PaletteIndex::Max;
		};

		langDef.mCommentStart = "/*";
		langDef.mCommentEnd = "*/";
		langDef.mSingleLineComment = "//";

		langDef.mCaseSensitive = true;
		langDef.mAutoIndentation = true;

		langDef.mName = "ChaiScript";

		inited = true;
	}
	return langDef;
}

class Editor : public Radium::Application
{
public:
    TextEditor* editor;
    Radium::SubViewport *scene;
    ImTextureID sceneTexture;
    // Keep the sink alive for the lifetime of the application
    std::shared_ptr<spdlog::sinks::sink> imguiSink;
    Radium::Nodes::Node* selectedNode = nullptr;

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
        Radium::SpriteBatchRegistry::Add("flappy", "texture.png", Rune::SpriteOrigin::TopLeft, Rune::SamplingMode::Nearest);

        imguiTheme();
        scene = new Radium::SubViewport(1280, 720);
        Rune::Texture *tex = new Rune::Texture(scene->viewport->textureView, Rune::SamplingMode::Nearest);
        
        
        Radium::Nodes::Sprite2D* background = new Radium::Nodes::Sprite2D();
        background->batchTag = "flappy";
        background->sourceRect = {0, 0, 288, 512};
        background->textureWidth = 1024;
        background->textureHeight = 1024;
        background->position = {0, 0};
        background->r = 1.0f;
        background->g = 1.0f;
        background->b = 1.0f;
        background->z = -10.0f;
        background->origin = Radium::Nodes::CoordinateOrigin::TopLeft;
        background->position.x = 0;
        background->size = {288, 512};
        background->name = "Background";

        scene->tree.nodes.push_back(background);

        Radium::Nodes::Sprite2D* background2 = new Radium::Nodes::Sprite2D();
        background2->batchTag = "flappy";
        background2->sourceRect = {0, 0, 288, 512};
        background2->textureWidth = 1024;
        background2->textureHeight = 1024;
        background2->position = {0, 0};
        background2->r = 1.0f;
        background2->g = 1.0f;
        background2->b = 1.0f;
        background2->z = -10.0f;
        background2->origin = Radium::Nodes::CoordinateOrigin::TopLeft;
        background2->position.x = 0;
        background2->size = {288, 512};
        background2->name = "Background";

        scene->tree.nodes.push_back(background2);
        
        scene->OnLoad();

        sceneTexture = ImGui_ImplRune_TextureToID(tex);
    
        editor = new TextEditor();

        auto lang = ChaiScript();

        editor->SetLanguageDefinition(lang);
        editor->SetPalette(RadiumPalette());
        editor->SetShowWhitespaces(false);

        Radium::Nodes::Node::Register();
        Radium::Nodes::Node2D::Register();
        Radium::Nodes::Sprite2D::Register();
    }

    void OnEarlyLoad() override
    {
        // attach the previously-declared ImGuiSink so spdlog messages are shown in the UI console
        auto sink = std::make_shared<ImGuiSink>(&g_imguiConsole);
        imguiSink = sink;
        //if (spdlog::default_logger())
        //{
        //    spdlog::default_logger()->sinks().push_back(sink);
        //}
    }

    void OnTick(float dt) override
    {
        //scene->OnTick(dt);
    }

    void OnPreRender() override
    {
        scene->OnRender();
    }

    void OnRender() override
    {
    }

    void DrawNode(Radium::Nodes::Node* node) {
        ImGui::PushID(node);
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

        if (node->children.empty()) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        bool open = ImGui::TreeNodeEx((void*)(intptr_t)&node, flags, "%s", node->name.c_str());

        if (ImGui::IsItemClicked()) {
            selectedNode = node;
        }

        if (open && !node->children.empty()) {
            for (auto& child : node->children)
                DrawNode(child);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void ImGuiTextEdit(const std::string &label, std::string &val) {
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
        if (buf.empty()) {
            // initialize buffer with existing value; keep a reasonable reserve
            size_t desired = std::max<size_t>(256, val.size() + 1);
            buf.assign(desired, '\0');
            strncpy(buf.data(), val.c_str(), desired - 1);
        }

        // Ensure unique ImGui id (PushID) so labels may be reused across nodes
        ImGui::PushID(key.c_str());
        bool changed = ImGui::InputText(label.c_str(), buf.data(), buf.size());
        ImGui::PopID();

        if (changed) {
            val = std::string(buf.data());
        }
    }


    void OnImgui() override
    {
        scene->OnImgui();
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
                    if (ImGui::MenuItem("New", NULL, false))
                        exit(0);
                    if (ImGui::MenuItem("Open", NULL, false))
                        exit(0);
                    if (ImGui::MenuItem("Save", NULL, false))
                        exit(0);

                    if (ImGui::MenuItem("Close", NULL, false))
                        exit(0);
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Inspector");

            if (selectedNode) {
                ImGuiTextEdit("Name", selectedNode->name);
                
                ImGui::Separator();

                for (auto property : Radium::Nodes::ClassDB::GetProperties(selectedNode)) {
                    ImGui::PushID(selectedNode);
                    ImGui::PushID(property.name.c_str());
                    if (property.name == "name") {
                        continue;
                    }
                    if (property.type == "Radium::Vector2f") {
                        Radium::Vector2f* value = Radium::Nodes::ClassDB::GetPropertyPointer<Radium::Vector2f>(property.name, selectedNode);
                    
                        float val[2] = {value->x, value->y};

                        ImGui::InputFloat2(property.name.c_str(), val);

                        value->x = val[0];
                        value->y = val[1];
                    }
                    if (property.type == "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >") {
                        // Edit the actual property string in-place. Use GetPropertyPointer so changes
                        // persist back to the node.
                        std::string* strPtr = Radium::Nodes::ClassDB::GetPropertyPointer<std::string>(property.name, selectedNode);
                        if (strPtr) {
                            ImGuiTextEdit(property.name, *strPtr);
                        }
                    }
                    if (property.type == "float") {
                        float* f = Radium::Nodes::ClassDB::GetPropertyPointer<float>(property.name, selectedNode);
                        ImGui::InputFloat(property.name.c_str(), f);
                    }
                    if (property.type == "unsigned int") {
                        unsigned int* f = Radium::Nodes::ClassDB::GetPropertyPointer<unsigned int>(property.name, selectedNode);
                        ImGui::InputScalar(property.name.c_str(), ImGuiDataType_U32, f);
                    }
                    if (property.type == "Radium::RectangleF") {
                        Radium::RectangleF* value = Radium::Nodes::ClassDB::GetPropertyPointer<Radium::RectangleF>(property.name, selectedNode);
                    
                        float pos[2] = {value->x, value->y};
                        float size[2] = {value->w, value->h};

                        if (ImGui::CollapsingHeader(property.name.c_str())) {
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
                    ImGui::PopID();
                    ImGui::PopID();
                }
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Assets");

            ImGui::End();
        }
        {
            ImGui::Begin("Hierarchy");

            static bool openMenu = false;
            static char filter[128] = "";
            static int selectedItem = -1;
            static std::vector<std::string> items = Radium::Nodes::ClassDB::GetNodeClasses();
            
            if (ImGui::Button("Add Node")) {
                openMenu = true;
                filter[0] = '\0'; // reset filter
                selectedItem = -1;
                ImGui::OpenPopup("RofiMenu");
            }

            if (ImGui::BeginPopup("RofiMenu")) {
                ImGui::InputText("##Filter", filter, IM_ARRAYSIZE(filter));

                ImGui::Separator();

                ImGui::BeginChild("ScrollRegion", ImVec2(0, 150), true);

                int index = 0;
                for (const auto& item : items) {
                    if (strlen(filter) > 0 && item.find(filter) == std::string::npos)
                        continue;

                    if (ImGui::Selectable(item.c_str())) {
                        selectedItem = index;
                        openMenu = false;
                        ImGui::CloseCurrentPopup();
                    }

                    ++index;
                }

                ImGui::EndChild();
                ImGui::EndPopup();
            }

            if (selectedItem != -1) {
                std::string selected = items[selectedItem];
                
                selectedItem = -1;
                
                spdlog::info("Selected: {}", selected);

                auto node = (Radium::Nodes::Node*)Radium::Nodes::ClassDB::Create(selected);

                scene->tree.nodes.push_back(node);
            }

            for (auto node : scene->tree.nodes) {
                DrawNode(node);
            }

            ImGui::End();
        }
        {
            ImGui::Begin("Console");

            // toolbar
            if (ImGui::Button("Clear"))
            {
                g_imguiConsole.Clear();
            }
            ImGui::SameLine();
            bool autoScroll = g_imguiConsole.autoScroll;
            if (ImGui::Checkbox("Auto-scroll", &autoScroll))
            {
                g_imguiConsole.autoScroll = autoScroll;
            }

            ImGui::Separator();

            ImGui::BeginChild("ConsoleScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
            auto snapshot = g_imguiConsole.Snapshot();
            for (const auto &line : snapshot)
            {
                ImGui::TextUnformatted(line.c_str());
            }
            if (g_imguiConsole.autoScroll)
            {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

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
    }
};
#ifndef __ANDROID__
RADIUM_ENTRYPOINT(Editor)
#else
extern "C" int SDL_main(int argc, char *argv[])
{
    MyApp app;
    Radium::currentApplication = &app;
    app.Run();
    return 0;
}
#endif