#pragma once

#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <spdlog/spdlog.h>
#include <algorithm>

template <typename T, typename U>
constexpr size_t offsetOf(U T::*member)
{
    return reinterpret_cast<const char *>(&(reinterpret_cast<T *>(0)->*member)) - reinterpret_cast<const char *>(0);
}

#define CLASSDB_REGISTER(className) Radium::Nodes::ClassDB::Register<className>()
#define CLASSDB_REGISTER_SUBCLASS(className, parentName) Radium::Nodes::ClassDB::Register<className, parentName>()
#define CLASSDB_DECLARE_PROPERTY(className, type, propertyName) Radium::Nodes::ClassDB::RegisterProperty<className, type>(#propertyName, offsetOf(&className::propertyName), sizeof(type))

namespace Radium::Nodes
{

    std::string Demangle(const char *name);

    class Object
    {
    public:
        virtual ~Object() = default;
    };

    namespace ClassDB
    {

        struct PropertyInfo
        {
            std::string name;
            std::string type;
            size_t offset;
            size_t size;
        };

        struct ClassInfo
        {
            std::vector<PropertyInfo> properties;
            ClassInfo *parent = nullptr;
            std::string name;
            std::function<Object *()> factory;
        };

        // Declarations
        extern std::unordered_map<std::string, ClassInfo> registeredClasses;
        extern std::vector<std::string> enums;

        template <typename T>
        void Register()
        {
            std::string typeName = Demangle(typeid(T).name());
            if (registeredClasses.find(typeName) != registeredClasses.end())
            {
                return;
            }

            ClassInfo info;
            info.parent = nullptr;
            info.factory = []() -> Object *
            {
                return new T();
            };
            info.name = typeName;
            registeredClasses[typeName] = info;
        }

        template <typename T>
        void RegisterEnum()
        {
            enums.push_back(Demangle(typeid(T).name()));
        }

        inline bool IsEnum(std::string typeName)
        {
            return std::find(enums.begin(), enums.end(), typeName) != enums.end();
        }

        template <typename T, typename P>
        void Register()
        {
            std::string typeName = Demangle(typeid(T).name());
            std::string parentName = Demangle(typeid(P).name());

            spdlog::info("Registering type {} with parent {}", typeName, parentName);

            if (registeredClasses.find(typeName) != registeredClasses.end())
            {
                spdlog::info("Already registered");
                return;
            }

            // Ensure parent is registered
            if (registeredClasses.find(parentName) == registeredClasses.end())
            {
                spdlog::info("Need to register: {}", parentName);
                Register<P>();
            }

            ClassInfo info;
            info.parent = &registeredClasses[parentName];
            info.factory = []() -> Object *
            {
                return new T();
            };
            info.name = typeName;
            registeredClasses[typeName] = info;
            spdlog::info("Register finished!");
        }

        template <typename T1, typename T2>
        void RegisterProperty(const std::string &propertyName, size_t offset, size_t size)
        {
            std::string typeName = Radium::Nodes::Demangle(typeid(T1).name());
            std::string propertyType = Radium::Nodes::Demangle(typeid(T2).name());

            PropertyInfo prop;
            prop.name = propertyName;
            prop.type = propertyType;
            prop.offset = offset;
            prop.size = size;
            registeredClasses[typeName].properties.push_back(prop);
        }

        template <typename T>
        void SetProperty(std::string propertyName, Object *instance, T value)
        {
            std::string typeName = Demangle(typeid(*instance).name());
            ClassInfo *info = &registeredClasses[typeName];

            while (info)
            {
                for (const PropertyInfo &prop : info->properties)
                {
                    if (prop.name == propertyName)
                    {
                        if (prop.size != sizeof(T))
                        {
                            throw std::runtime_error("Property size mismatch");
                        }

                        uint8_t *basePtr = reinterpret_cast<uint8_t *>(instance);
                        *reinterpret_cast<T *>(basePtr + prop.offset) = value;
                        return;
                    }
                }
                info = info->parent;
            }

            throw std::runtime_error("Property not found");
        }

        template <typename T>
        T GetProperty(std::string propertyName, Object *instance)
        {
            std::string typeName = Demangle(typeid(*instance).name());
            ClassInfo *info = &registeredClasses[typeName];

            while (info)
            {
                for (const PropertyInfo &prop : info->properties)
                {
                    if (prop.name == propertyName)
                    {
                        if (prop.size != sizeof(T))
                        {
                            throw std::runtime_error("Property size mismatch");
                        }

                        uint8_t *basePtr = reinterpret_cast<uint8_t *>(instance);
                        return *reinterpret_cast<T *>(basePtr + prop.offset);
                    }
                }
                info = info->parent;
            }

            throw std::runtime_error("Property not found");
        }

        template <typename T>
        T *GetPropertyPointer(std::string propertyName, Object *instance)
        {
            std::string typeName = Demangle(typeid(*instance).name());
            ClassInfo *info = &registeredClasses[typeName];

            while (info)
            {
                for (const PropertyInfo &prop : info->properties)
                {
                    if (prop.name == propertyName)
                    {
                        if (prop.size != sizeof(T))
                        {
                            throw std::runtime_error("Property size mismatch");
                        }

                        uint8_t *basePtr = reinterpret_cast<uint8_t *>(instance);
                        return reinterpret_cast<T *>(basePtr + prop.offset);
                    }
                }
                info = info->parent;
            }

            throw std::runtime_error("Property not found");
        }

        inline ClassInfo GetClassInfo(Object *object)
        {
            return registeredClasses[Demangle(typeid(*object).name())];
        }

        inline std::vector<PropertyInfo> GetProperties(Object *instance)
        {
            std::vector<PropertyInfo> result;

            std::string typeName = Demangle(typeid(*instance).name());

            auto it = registeredClasses.find(typeName);
            if (it == registeredClasses.end())
            {
                throw std::runtime_error("Type not registered: " + typeName);
            }

            ClassInfo *info = &it->second;

            // Walk the inheritance chain, deepest base first
            std::vector<ClassInfo *> classChain;
            while (info)
            {
                classChain.insert(classChain.begin(), info);
                info = info->parent;
            }

            // Collect properties from base to derived
            for (ClassInfo *cls : classChain)
            {
                result.insert(result.end(), cls->properties.begin(), cls->properties.end());
            }

            return result;
        }

        inline Object *GetPropertyAsObject(Object *instance, const std::string &propertyName)
        {
            if (!instance)
            {
                spdlog::error("No instance!");
                return nullptr;
            }

            std::string typeName = Radium::Nodes::Demangle(typeid(*instance).name());
            auto it = Radium::Nodes::ClassDB::registeredClasses.find(typeName);

            if (it == Radium::Nodes::ClassDB::registeredClasses.end())
            {
                spdlog::error("Class not registered: {}", typeName);
                return nullptr;
            }

            Radium::Nodes::ClassDB::ClassInfo *info = &it->second;

            while (info)
            {
                for (const auto &prop : info->properties)
                {
                    if (prop.name == propertyName)
                    {
                        // Return pointer to sub-object
                        uint8_t *base = reinterpret_cast<uint8_t *>(instance);
                        Object *subObj = reinterpret_cast<Object *>(base + prop.offset);
                        return subObj;
                    }
                }
                info = info->parent;
            }

            return nullptr;
        }

        inline std::string GetType(Object *object)
        {
            return Demangle(typeid(*object).name());
        }

        inline Object *Create(const std::string &className)
        {
            auto it = registeredClasses.find(className);
            spdlog::trace("Post find");
            if (it == registeredClasses.end())
            {
                spdlog::error("Class not registered: " + className);
                abort();
            }

            spdlog::trace("Found: {}", it->second.name);

            if (!it->second.factory)
            {
                throw std::runtime_error("No factory function for class: " + className);
            }

            return it->second.factory();
        }

        inline std::vector<std::string> GetNodeClasses()
        {
            std::vector<std::string> result;

            const std::string baseNodeType = "Radium::Nodes::Node"; // <-- Adjust if your full demangled name differs

            for (const auto &[className, classInfo] : registeredClasses)
            {
                const ClassInfo *current = &classInfo;

                while (current)
                {
                    if (current->name == baseNodeType)
                    {
                        result.push_back(className);
                        break;
                    }
                    current = current->parent;
                }
            }

            return result;
        }

        inline bool HasProperty(std::string propertyName, Object *obj)
        {
            std::string typeName = ClassDB::GetType(obj);

            auto it = registeredClasses.find(typeName);
            if (it == registeredClasses.end())
            {
                throw std::runtime_error("Type not registered: " + typeName);
            }

            ClassInfo *info = &it->second;

            while (info)
            {
                for (const auto &prop : info->properties)
                {
                    if (prop.name == propertyName)
                    {
                        return true;
                    }
                }
                info = info->parent;
            }

            return false;
        }

    }
}
