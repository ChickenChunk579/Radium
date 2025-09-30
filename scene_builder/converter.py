import configparser
from collections import defaultdict
from pathlib import Path

def parse_value(val):
    val = val.strip()
    if val.startswith("{") and val.endswith("}"):
        return val
    if val.lower() in ('true', 'false'):
        return val.lower()
    try:
        iv = int(val)
        return str(iv)
    except ValueError:
        pass
    try:
        fv = float(val)
        return f"{fv}f"
    except ValueError:
        pass
    return f'"{val}"'

def generate_scene_class(scene_name, ini_path):
    config = configparser.ConfigParser()
    config.optionxform = str  # preserve case
    config.read(ini_path)

    scene_data = defaultdict(lambda: defaultdict(dict))
    includes = []

    for section in config.sections():
        if section == "Includes":
            includes = [v.strip() for k, v in config.items(section)]
            continue

        if '/' not in section:
            print(f"Skipping invalid section: {section}")
            continue

        entity, component_full = section.split('/', 1)
        for key, value in config.items(section):
            scene_data[entity][component_full][key] = parse_value(value)

    class_name = f"{scene_name}Scene"

    # --- Header File ---
    hpp_lines = [
        "#pragma once\n",
        "#include <Radium/Scene.hpp>\n",
        "#include <entt/entt.hpp>\n",
    ]
    hpp_lines += [f"#include {inc}\n" for inc in includes]
    hpp_lines += [
        f"\nnamespace Radium {{\n",
        f"class {class_name} : public Scene {{\n",
        "public:\n",
        "    void AddObjects(entt::registry registry) override;\n",
        "};\n",
        "}\n"
    ]

    # --- Source File ---
    cpp_lines = [
        f"#include \"{class_name}.hpp\"\n",
        "\n".join([f"#include {inc}" for inc in includes]),
        "\nnamespace Radium {\n",
        f"void {class_name}::AddObjects(entt::registry registry) {{\n"
    ]

    ent_counter = 0
    for entity_name, components in scene_data.items():
        ent_var = f"ent_{ent_counter}"
        cpp_lines.append(f"    auto {ent_var} = registry.create();\n")
        cpp_lines.append(f'    registry.emplace<Radium::Components::Name>({ent_var}, Radium::Components::Name{{.name = \"{entity_name}\"}});\n')

        for component_full, props in components.items():
            if component_full == "Name":
                continue  # Already handled

            # Handle namespace in component
            component_type = component_full if "::" in component_full else f"Radium::Components::{component_full}"

            prop_inits = ", ".join(f".{k} = {v}" for k, v in props.items())
            cpp_lines.append(
                f"    registry.emplace<{component_type}>({ent_var}, {component_type}{{{prop_inits}}});\n"
            )

        ent_counter += 1

    cpp_lines.append("}\n")
    cpp_lines.append("}\n")

    return "\n".join(hpp_lines), "\n".join(cpp_lines)

# === Usage ===
ini_path = "scene.ini"
scene_name = "Example"

hpp_code, cpp_code = generate_scene_class(scene_name, ini_path)

Path(f"{scene_name}Scene.hpp").write_text(hpp_code)
Path(f"{scene_name}Scene.cpp").write_text(cpp_code)

print("Generated:", f"{scene_name}Scene.hpp and .cpp")
