#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <unordered_set>
#include <vector>

/** 
 * A simple observable template.
 * 
 * It is as simple as possible:
 * issues arising with observers deleted before observable are omitted.
 * no possibility of setting listener priority.
 */
template<typename ...A> class Observable
{
    typedef void(*EventListener)(A...);  /* Listener callback type */
private:
    std::unordered_set<EventListener> m_listeners; /* Collection of listeners */
    std::vector<EventListener> m_listenerQueue;    /* Call queue of listeners */
public:
    /** Add event listener callback */
    void addListener(EventListener listener)
    {
        if (m_listeners.insert(listener).second)
        {
            m_listenerQueue.push_back(listener);
        }
    }
    /** Remove event listener callback */
    void removeListener(EventListener listener)
    {
        std::size_t listenerCount = m_listeners.size();
        m_listeners.erase(listener);
        if (listenerCount != m_listeners.size())
        {
            for(auto it = m_listenerQueue.begin(); it != m_listenerQueue.end(); ++it)
            {
                if(*it == listener)
                {
                    m_listenerQueue.erase(it);
                    break;
                }
            }
        }
    }
    /** Fire event, call all listeners in order of their registration */
    void fireEvent(A... args)
    {
        for (auto it = m_listenerQueue.begin(); it != m_listenerQueue.end(); ++it)
        {
            (*it)(args...);
        }
    }
};
#endif /* OBSERVABLE_H */
