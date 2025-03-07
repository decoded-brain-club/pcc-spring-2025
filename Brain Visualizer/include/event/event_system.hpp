#pragma once

// thanks imhex <3

#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string_view>
#include <type_traits>
#include <utility>
#include <tsl/robin_map.h>

// im not typin allat
using u32 = std::uint32_t;

#define EVENT_DEF(event_name, ...)                                                             \
    struct event_name final : public EventSystem::Event<__VA_ARGS__> {                         \
        constexpr static auto Id = [] { return EventSystem::EventId(#event_name); }();         \
                                                                                               \
        explicit event_name(Callback func) noexcept : Event(std::move(func)) { }               \
                                                                                               \
        static void subscribe(const Callback& function) {                                             \
            EventSystem::subscribe<event_name>(std::move(function));                           \
        }                                                                                      \
                                                                                               \
        static void unsubscribe() noexcept {                                                   \
            EventSystem::unsubscribe<event_name>();                                            \
        }                                                                                      \
                                                                                               \
        static void post(auto &&...args) {                                                     \
            EventSystem::post<event_name>(std::forward<decltype(args)>(args)...);              \
        }                                                                                      \
    }

namespace EventSystem
{
    class EventId
    {
    public:
        explicit constexpr EventId(const char* eventName)
        {
            m_hash = 0x811C'9DC5;
            for (const char c : std::string_view(eventName))
            {
                m_hash = (m_hash >> 5) | (m_hash << 27);
                m_hash ^= c;
            }
        }

        constexpr bool operator==(const EventId& other) const
        {
            return m_hash == other.m_hash;
        }

        constexpr auto operator<=>(const EventId& other) const
        {
            return m_hash <=> other.m_hash;
        }

        [[nodiscard]] u32 hash() const
        {
            return m_hash;
        }

    private:
        u32 m_hash;
    };

    struct EventIdHash
    {
        std::size_t operator()(const EventId& id) const
        {
            return id.hash();
        }
    };

    struct EventBase
    {
        EventBase() noexcept = default;

        virtual ~EventBase() = default;
    };

    template<typename... Params>
    struct Event : EventBase
    {
        using Callback = std::function<void(Params...)>;

        explicit Event(Callback func) noexcept : m_func(std::move(func))
        {
        }

        void call(Params... params) const
        {
            if (m_func)
            {
                try
                {
                    m_func(params...);
                }
                catch (const std::exception& e)
                {
                    // TODO: decide what to do here, log? rethrow? both?
                    throw;
                }
            }
        }

    private:
        Callback m_func;
    };

    template<typename T>
    concept EventType = std::derived_from<T, EventBase>;

    tsl::robin_map<EventId, std::unique_ptr<EventBase>, EventIdHash>& get_event_registry();

    std::recursive_mutex& get_event_mutex();

    /**
     * @brief Subscribes to an event (replaces any existing handler)
     * @tparam E Event type
     * @param function Function to call when the event is posted
     */
    template<EventType E>
    void subscribe(const typename E::Callback& function)
    {
        std::scoped_lock lock(get_event_mutex());

        // Replace any existing handler with the new one
        get_event_registry()[E::Id] = std::make_unique<E>(function);
    }

    /**
     * @brief Unsubscribes from an event
     * @tparam E Event type
     */
    template<EventType E>
    void unsubscribe() noexcept
    {
        std::scoped_lock lock(get_event_mutex());

        // Remove the event handler if it exists
        get_event_registry().erase(E::Id);
    }

    /**
     * @brief Posts an event to its handler
     * @tparam E Event type
     * @param args Arguments to pass to the event handler
     */
    template<EventType E>
    void post(auto&&... args)
    {
        {
            std::scoped_lock lock(get_event_mutex());
            auto it = get_event_registry().find(E::Id);
            if (it == get_event_registry().end())
            {
                return; // none found
            }

            const auto& eventHandler = it->second;
            if (!eventHandler)
            {
                return;
            }

            // this is safer by default but can be changed if performance degrades too much, tho my tests show no  real difference
            static_cast<E &>(*eventHandler).call(std::forward<decltype(args)>(args)...);
        }
    }

    /**
     * @brief Clears all event subscriptions
     */
    void clear() noexcept;
} // namespace EventSystem

inline tsl::robin_map<EventSystem::EventId, std::unique_ptr<EventSystem::EventBase>, EventSystem::EventIdHash>&
EventSystem::get_event_registry()
{
    static tsl::robin_map<EventId, std::unique_ptr<EventBase>, EventIdHash> registry;
    return registry;
}

inline std::recursive_mutex& EventSystem::get_event_mutex()
{
    static std::recursive_mutex mutex;
    return mutex;
}

inline void EventSystem::clear() noexcept
{
    std::scoped_lock lock(get_event_mutex());
    get_event_registry().clear();
}