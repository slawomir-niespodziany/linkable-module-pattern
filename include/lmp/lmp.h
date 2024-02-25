#pragma once

#include <list>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace lmp {

template <typename ModuleId>
class AbstractModule {
public:
    using id_type = ModuleId;

    virtual ~AbstractModule() = default;
    virtual id_type getId() = 0;
};

template <template <typename...> class C, typename... Ts>
std::true_type is_base_of_template_impl(const C<Ts...>*);

template <template <typename...> class C>
std::false_type is_base_of_template_impl(...);

template <template <typename...> class C, typename T>
using is_base_of_template = decltype(is_base_of_template_impl<C>(std::declval<T*>()));

template <typename ModuleInterface>
class ModuleRegistry {
public:
    static_assert(is_base_of_template<AbstractModule, ModuleInterface>::value, "ModuleInterface must inherit after AbstractModule<>.");

    using ModuleId = typename ModuleInterface::id_type;

    virtual ~ModuleRegistry() = default;
    ModuleRegistry(const ModuleRegistry&) = delete;
    ModuleRegistry(ModuleRegistry&&) = delete;
    ModuleRegistry& operator=(const ModuleRegistry&) = delete;
    ModuleRegistry& operator=(ModuleRegistry&&) = delete;

    static ModuleRegistry& getInstance() {
        static ModuleRegistry instance;
        return instance;
    }

    void add(std::shared_ptr<ModuleInterface> pPlugin) { modules_.emplace(pPlugin->getId(), pPlugin); }
    void remove(const ModuleId& id) { modules_.erase(id); }
    bool has(const ModuleId& id) { return (0 != modules_.count(id)); }
    std::unordered_set<ModuleId> all() {
        std::unordered_set<ModuleId> result;
        for (const auto& kv : modules_) {
            result.insert(kv.first);
        }
        return result;
    }

    std::shared_ptr<ModuleInterface> get(const ModuleId& id) {
        std::shared_ptr<ModuleInterface> pResult{nullptr};
        const auto it = modules_.find(id);
        if (modules_.cend() != it) {
            pResult = it->second;
        }
        return pResult;
    }

private:
    ModuleRegistry() = default;

    std::unordered_map<ModuleId, std::shared_ptr<ModuleInterface>> modules_;
};

template <typename ModuleInterface, typename Module>
class ModuleRegisterer {
public:
    static_assert(is_base_of_template<AbstractModule, ModuleInterface>::value, "ModuleInterface must inherit after AbstractModule<>.");
    static_assert(std::is_base_of<ModuleInterface, Module>::value, "Module must inherit after ModuleInterface.");

    template <typename... Args>
    ModuleRegisterer(Args... args) {
        ModuleRegistry<ModuleInterface>::getInstance().add(std::make_shared<Module>(args...));
    }
};

}   // namespace lmp
