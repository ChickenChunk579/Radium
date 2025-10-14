#pragma once

#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <Flux/Flux.hpp>
#include <algorithm>
#include <functional>
#include <stdexcept>

/**
 * @brief Calculate the offset of a member variable within a class.
 *
 * @tparam T Class type
 * @tparam U Member type
 * @param member Pointer to member
 * @return size_t Offset in bytes
 */
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
        /**
         * @struct PropertyInfo
         * @brief Stores metadata about a class property.
         * 
         * Contains the property's name, type, memory offset within class,
         * and its size in bytes.
         */
        struct PropertyInfo
        {
            std::string name; /**< The name of the property */
            std::string type; /**< The type of the property, demangled */
            size_t offset; /**< Byte offset of the property within the class */
            size_t size; /**< Size of the property in bytes */
        };

        /**
         * @struct ClassInfo
         * @brief Holds reflection metadata for a registered class
         * 
         * Includes a list of properties, pointer to the parent class's metadata,
         * the class name, and a factory function to instantiate the object.
         */
        struct ClassInfo
        {
            std::vector<PropertyInfo> properties;
            ClassInfo *parent = nullptr;
            std::string name;
            std::function<Object *()> factory;
        };

        // Declarations
        /**
         * Holds a map of all registered class names to there relevant ClassInfo metadata
         */
        extern std::unordered_map<std::string, ClassInfo> registeredClasses;
        /**
         * Holds a list for the names of all registered enums
         */
        extern std::vector<std::string> enums;

        /**
         * @brief Register a class with ClassDB.
         *
         * @tparam T Class type to register
         */
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
        /**
         * @brief Register an enum type with ClassDB.
         *
         * @tparam T Enum type to register
         */
        template <typename T>
        void RegisterEnum()
        {
            enums.push_back(Demangle(typeid(T).name()));
        }

        inline bool IsEnum(std::string typeName)
        {
            return std::find(enums.begin(), enums.end(), typeName) != enums.end();
        }

        /**
         * @brief Register a subclass and its parent class with ClassDB.
         *
         * @tparam T Subclass type to register
         * @tparam P Parent class type
         */
        template <typename T, typename P>
        void Register()
        {
            std::string typeName = Demangle(typeid(T).name());
            std::string parentName = Demangle(typeid(P).name());

            Flux::Info("Registering type {} with parent {}", typeName, parentName);

            if (registeredClasses.find(typeName) != registeredClasses.end())
            {
                Flux::Info("Already registered");
                return;
            }

            // Ensure parent is registered
            if (registeredClasses.find(parentName) == registeredClasses.end())
            {
                Flux::Info("Need to register: {}", parentName);
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
            Flux::Info("Register finished!");
        }

        /**
         * @brief Register a property of a class.
         *
         * @tparam T1 Class type owning the property
         * @tparam T2 Property type
         * @param propertyName Name of the property as string
         * @param offset Byte offset of the property within the class
         * @param size Size of the property type
         */
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

        /**
         * @brief Set a property value on an instance of a registered class.
         *
         * @tparam T Property type
         * @param propertyName Name of the property
         * @param instance Pointer to the object instance
         * @param value New value to set
         * @throws std::runtime_error If property not found or size mismatch
         */
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

        /**
         * @brief Get a property value from an instance of a registered class.
         *
         * @tparam T Property type
         * @param propertyName Name of the property
         * @param instance Pointer to the object instance
         * @return T Value of the property
         * @throws std::runtime_error If property not found or size mismatch
         */
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

        /**
         * @brief Get a pointer to a property inside an object instance.
         *
         * @tparam T Property type
         * @param propertyName Name of the property
         * @param instance Pointer to the object instance
         * @return T* Pointer to the property data
         * @throws std::runtime_error If property not found or size mismatch
         */
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

        /**
         * @brief Get class information for a given object instance.
         *
         * @param object Pointer to the object instance
         * @return ClassInfo Class information structure
         */
        inline ClassInfo GetClassInfo(Object *object)
        {
            return registeredClasses[Demangle(typeid(*object).name())];
        }

        /**
         * @brief Get all properties of an object instance, including inherited ones.
         *
         * @param instance Pointer to the object instance
         * @return std::vector<PropertyInfo> List of property info structures
         * @throws std::runtime_error If class is not registered
         */
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

        /**
         * @brief Get a sub-object property as an Object pointer.
         *
         * @param instance Pointer to the object instance
         * @param propertyName Name of the property
         * @return Object* Pointer to the sub-object, or nullptr if not found
         */
        inline Object *GetPropertyAsObject(Object *instance, const std::string &propertyName)
        {
            if (!instance)
            {
                Flux::Error("No instance!");
                return nullptr;
            }

            std::string typeName = Radium::Nodes::Demangle(typeid(*instance).name());
            auto it = Radium::Nodes::ClassDB::registeredClasses.find(typeName);

            if (it == Radium::Nodes::ClassDB::registeredClasses.end())
            {
                Flux::Error("Class not registered: {}", typeName);
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

        /**
        * @brief Get the demangled type name of an object instance.
        *
        * @param object Pointer to the object instance
        * @return std::string Demangled type name
        */
        inline std::string GetType(Object *object)
        {
            return Demangle(typeid(*object).name());
        }

        /**
        * @brief Create a new instance of a registered class by name.
        *
        * @param className Name of the class
        * @return Object* Pointer to the new instance
        * @throws std::runtime_error If class not registered or no factory available
        */
        inline Object *Create(const std::string &className)
        {
            auto it = registeredClasses.find(className);
            Flux::Trace("Post find");
            if (it == registeredClasses.end())
            {
                Flux::Error("Class not registered: ", className);
                abort();
            }

            Flux::Trace("Found: {}", it->second.name);

            if (!it->second.factory)
            {
                throw std::runtime_error("No factory function for class: " + className);
            }

            return it->second.factory();
        }

        /**
        * @brief Get all classes derived from the base Node class.
        *
        * @return std::vector<std::string> List of registered Node subclass names
        */
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

        /**
        * @brief Check if an object has a property by name.
        *
        * @param propertyName Name of the property to check
        * @param obj Pointer to the object instance
        * @return true If the property exists
        * @return false Otherwise
        * @throws std::runtime_error If class is not registered
        */
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
