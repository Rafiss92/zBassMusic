#pragma once

#include "NH/Logger.h"

#include <utility>
#include <vector>
#include <deque>
#include <variant>

namespace NH::Bass
{
    class MusicTheme;

    enum class EventType
    {
        UNKNOWN = 0,
        MUSIC_END,
        MUSIC_TRANSITION,
        MUSIC_CHANGE
    };

    struct Event
    {
        struct MusicEnd { MusicTheme const* Theme = nullptr; std::string AudioId; };
        struct MusicTransition { MusicTheme const* Theme = nullptr; std::string AudioId; float TimeLeft = 0; };
        struct MusicChange { MusicTheme const* Theme = nullptr; std::string AudioId; };
        using DataType = std::variant<MusicEnd, MusicTransition, MusicChange>;

        EventType Type = EventType::UNKNOWN;
        DataType Data;

        Event() = delete;
        Event(const EventType type, DataType data) : Type(type), Data(std::move(data)) {}
        virtual ~Event() = default;
    };

    using EventSubscriberFn = void (*)(const Event&, void*);

    struct EventSubscriber
    {
        EventType Type = EventType::UNKNOWN;
        EventSubscriberFn Function{};
        void* UserData = nullptr;

        bool operator==(const EventSubscriber& other) const
        {
            return this == &other;
        }
    };

    class Engine;

    class EventManager
    {
        static Logger* log;

        friend Engine;

        std::vector<EventSubscriber> m_Subscribers{};
        std::deque<Event> m_EventQueue;

    public:
        EventSubscriber* AddSubscriber(EventType type, EventSubscriberFn function, void* userData = nullptr);

        void RemoveSubscriber(const EventSubscriber* subscriber);

        void DispatchEvent(Event&& event);

        void DispatchEvent(const Event& event);

        void Update();

    private:
        EventManager() = default;
    };

    struct MusicEndEvent final : Event
    {
        MusicEndEvent(const MusicTheme* theme, const std::string& audioId) : Event(EventType::MUSIC_END, MusicEnd{theme, audioId}) {}
    };

    struct MusicTransitionEvent final : Event
    {
        MusicTransitionEvent(const MusicTheme* theme, const std::string& audioId, const float timeLeft)
            : Event(EventType::MUSIC_TRANSITION, MusicTransition{theme, audioId, timeLeft}) {}
    };

    struct MusicChangeEvent final : Event
    {
        MusicChangeEvent(const MusicTheme* theme, const std::string& audioId) : Event(EventType::MUSIC_CHANGE, MusicChange{theme, audioId}) {}
    };
}