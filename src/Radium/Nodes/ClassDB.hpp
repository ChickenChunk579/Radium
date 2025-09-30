#pragma once

#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

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
        };

        // Declarations
        extern std::unordered_map<std::string, ClassInfo> registeredClasses;

        template <typename T>
        void Register()
        {
            std::string typeName = typeid(T).name();
            if (registeredClasses.find(typeName) != registeredClasses.end())
            {
                return;
            }


            ClassInfo info;
            info.parent = nullptr;
            registeredClasses[typeName] = info;
        }

        template <typename T, typename P>
        void Register()
        {
            std::string typeName = typeid(T).name();
            std::string parentName = typeid(P).name();

            if (registeredClasses.find(typeName) != registeredClasses.end())
            {
                return;
            }

            // Ensure parent is registered
            if (registeredClasses.find(parentName) == registeredClasses.end())
            {
                Register<P>();
            }

            ClassInfo info;
            info.parent = &registeredClasses[parentName];
            registeredClasses[typeName] = info;
        }

        template <typename T1, typename T2>
        void RegisterProperty(const std::string &propertyName, size_t offset, size_t size)
        {
            std::string typeName = typeid(T1).name();
            PropertyInfo prop;
            prop.name = propertyName;
            prop.type = typeid(T2).name();
            prop.offset = offset;
            prop.size = size;
            registeredClasses[typeName].properties.push_back(prop);
        }

        template <typename T>
        void SetProperty(std::string propertyName, Object *instance, T value)
        {
            std::string typeName = typeid(*instance).name();
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
            std::string typeName = typeid(*instance).name();
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

        inline std::vector<PropertyInfo> GetProperties(Object *instance)
        {
            std::vector<PropertyInfo> result;

            std::string typeName = typeid(*instance).name();
            ClassInfo *info = &registeredClasses[typeName];

            while (info)
            {
                result.insert(result.begin(), info->properties.begin(), info->properties.end());
                info = info->parent;
            }

            return result;
        }

        inline std::string GetType(Object *object)
        {
            return typeid(*object).name();
        }

    }
}
