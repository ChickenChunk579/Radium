#pragma once

#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <spdlog/spdlog.h>
#include <cxxabi.h>

template <typename T, typename U>
constexpr size_t offsetOf(U T::*member)
{
    return reinterpret_cast<const char *>(&(reinterpret_cast<T *>(0)->*member)) - reinterpret_cast<const char *>(0);
}

#define CLASSDB_REGISTER(className) Radium::Nodes::ClassDB::Register<className>()
#define CLASSDB_REGISTER_SUBCLASS(className, parentName) Radium::Nodes::ClassDB::Register<className, parentName>()
#define CLASSDB_DECLARE_PROPERTY(className, type, propertyName) Radium::Nodes::ClassDB::RegisterProperty<className, type>(#propertyName, offsetOf(&className::propertyName), sizeof(type))
#define CLASSDB_DECLARE_METHOD(className, returnType, methodName, ...) Radium::Nodes::ClassDB::RegisterMethod<className, returnType, __VA_ARGS__>(#methodName)

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
        struct MethodInfo
        {
            void* funcPtr;
            std::string retType;
            std::vector<std::string> argTypes;
        };


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
            std::unordered_map<std::string, MethodInfo> methods;
            ClassInfo *parent = nullptr;
            std::function<Object *()> factory;
        };

        // Declarations
        extern std::unordered_map<std::string, ClassInfo> registeredClasses;

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
            registeredClasses[typeName] = info;
        }

        template <typename T, typename P>
        void Register()
        {
            std::string typeName = Demangle(typeid(T).name());
            std::string parentName = Demangle(typeid(P).name());

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
            info.factory = []() -> Object *
            {
                return new T();
            };
            registeredClasses[typeName] = info;
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

        template <typename Class, typename Ret, typename... Args>
        void RegisterMethod(const std::string& name, Ret (Class::*method)(Args...))
        {
            std::string typeName = Demangle(typeid(Class).name());
            MethodInfo info;
            info.funcPtr = reinterpret_cast<void*>(method);
            info.retType = Demangle(typeid(Ret).name());
            (info.argTypes = { Demangle(typeid(Args).name())... });

            registeredClasses[typeName].methods[name] = info;
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

        inline std::vector<MethodInfo> GetMethods(Object *instance)
        {
            std::string className = Radium::Nodes::Demangle(typeid(*instance).name());
            auto it = registeredClasses.find(className);
            if (it == registeredClasses.end())
            {
                throw std::runtime_error("Class not registered: " + className);
            }

            ClassInfo *info = &it->second;

            // Inheritance support
            std::vector<MethodInfo> result;
            while (info)
            {
                result.insert(result.begin(), info->methods.begin(), info->methods.end());
                info = info->parent;
            }

            return result;
        }

        inline std::string GetType(Object *object)
        {
            return typeid(*object).name();
        }

        inline Object *Create(const std::string &className)
        {
            auto it = registeredClasses.find(className);
            if (it == registeredClasses.end())
            {
                throw std::runtime_error("Class not registered: " + className);
            }

            if (!it->second.factory)
            {
                throw std::runtime_error("No factory function for class: " + className);
            }

            return it->second.factory();
        }

        template <typename... Args>
        std::tuple<Args...> unpack_va_args(va_list args);

        template <typename T>
        T get_va_arg(va_list &args)
        {
            if constexpr (std::is_same_v<T, int>)
                return va_arg(args, int);
            else if constexpr (std::is_same_v<T, float>)
                return static_cast<float>(va_arg(args, double));
            else if constexpr (std::is_same_v<T, double>)
                return va_arg(args, double);
            else if constexpr (std::is_same_v<T, const char *>)
                return va_arg(args, const char *);
            else
                static_assert(!sizeof(T), "Unsupported type in va_arg");
        }

        // Recursive unpacking
        template <typename... Args>
        std::tuple<Args...> unpack_va_args(va_list args)
        {
            va_list args_copy;
            va_copy(args_copy, args);
            return unpack_va_args_impl<Args...>(args_copy);
        }

        template <typename... Args, size_t... I>
        std::tuple<Args...> unpack_va_args_impl_helper(va_list &args, std::index_sequence<I...>)
        {
            return std::make_tuple(get_va_arg<Args>(args)...);
        }

        template <typename... Args>
        std::tuple<Args...> unpack_va_args_impl(va_list &args)
        {
            return unpack_va_args_impl_helper<Args...>(args, std::index_sequence_for<Args...>{});
        }


        template<typename Ret = void>
        Ret Invoke(Object* object, const std::string& methodName, ...) {
            std::string className = Demangle(typeid(*object).name());
            auto it = ClassDB::registeredClasses.find(className);
            if (it == ClassDB::registeredClasses.end()) {
                throw std::runtime_error("Class not registered: " + className);
            }

            for (const auto& method : it->second.methods) {
                if (method.name == methodName) {
                    if (!method.invokeFunc) {
                        throw std::runtime_error("Method not invokable: " + methodName);
                    }

                    va_list args;
                    va_start(args, methodName);
                    void* resultPtr = method.invokeFunc(object, args);
                    va_end(args);

                    if constexpr (!std::is_void_v<Ret>) {
                        Ret result = *reinterpret_cast<Ret*>(resultPtr);
                        delete reinterpret_cast<Ret*>(resultPtr);
                        return result;
                    } else {
                        return;
                    }
                }
            }

            throw std::runtime_error("Method not found: " + methodName);
        }


    }
}
