#ifndef EVENTS_H
#define EVENTS_H

#include <QList>

namespace DB {

class ObjectItemModel;
class ObjectItem;
class ObjectImage;
class LayoutPage;
class ObjectView;
class ImageFile;

struct ScaleChange {
    ImageFile * image;
};
struct ObjectImageChange {
    ObjectImage * image;
};
struct ObjectItemChange {
    ObjectItem * item;
};
struct LayoutItemChange {
    //ObjectImage * image;
};

template <typename Event>
class EventListener {
public:
    EventListener() {}
    virtual ~EventListener() {}
    virtual void    event(Event & event) {Q_UNUSED(event);} // = 0;
};

template <typename Event>
class EventBroadcaster {
    QList<EventListener<Event>* > m_listeners;
public:
    EventBroadcaster() {}
    virtual ~EventBroadcaster() {}

    void    link(EventListener<Event> * listener)   {m_listeners.append(listener);}
    void    unlink(EventListener<Event> * listener) {m_listeners.removeAll(listener);}
    void    emitEvent(Event & event) {foreach(EventListener<Event>* listener, m_listeners) {listener->event(event);}}
};

}


/*
namespace detail
{
    template <typename Event>
    class EventBroadcaster
    {
    public:
        typedef std::function<void(const Event&)> Connection;

        void connect(Connection&& connection)
        {
            connections.push_back(std::move(connection));
        }
        void disconnect(Connection&& connection)
        {
            connections.erase(std::move(connection));
        }

        void signal(const Event& event)
        {
            for (const auto& connection : connections)
            {
                connection(event);
            }
        }
    private:
        std::vector<Connection> connections;
    };

   template <typename T> struct traits
       : public traits<decltype(&T::operator())> {};

   template <typename C, typename R, typename A>
   struct traits<R(C::*)(const A&) const>
   {
       typedef A type;
   };
}

template <typename... Events>
class EventBroadcaster
   : detail::EventBroadcaster<Events>...
{
public:
    template <typename Connection>
    void connect(Connection&& connection)
    {
        typedef typename detail::traits<Connection>::type Event;
        detail::EventBroadcaster<Event>& impl = *this;
        impl.connect(std::move(connection));
    }
    template <typename Connection>
    void disconnect(Connection&& connection)
    {
        typedef typename detail::traits<Connection>::type Event;
        detail::EventBroadcaster<Event>& impl = *this;
        impl.disconnect(std::move(connection));
    }

    template <typename Event>
    void signal(const Event& event)
    {
        detail::EventBroadcaster<Event>& impl = *this;
        impl.signal(event);
    }

    virtual void processEvents() = 0;
};
*/

#endif // EVENTS_H
