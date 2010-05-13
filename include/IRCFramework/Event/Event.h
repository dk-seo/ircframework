#include <vector>

#ifndef _EVENT_H_
#define _EVENT_H_

namespace Event
{
    /*
     * Subscription interface class.
     * Use the Subsription class for subscribing to events.
     * _DataType: The type of the data send by the owner when the event is fired.
     */
    template <typename _DataType>
    class ISubscription {
    public:
        /*
         * Call the subsriber. Virtual.
         * aSender: The sender of the event.
         * aData: Data send with the event.
         */
        virtual void Call(void *, _DataType) = 0;

        /*
         * Gets the address of the subscriber. Virtual.
         */
        virtual void * GetSubscriber() const = 0;

        /*
         * Destructor. Virtual.
         */
        virtual ~ISubscription()
        {
        };
    };


    //================================================================================================


    /*
     * Event class.
     * _DataType: The type of the data sent by the owner when the event is fired.
     */
    template <typename _DataType>
    class EventServer {
    public:
        /*
         * The type of the data send by the owner when the event is fired.
         */
        typedef _DataType DataType;

    private:
        /*
         * Non copyable class.
         */
        EventServer(const EventServer<_DataType> &);
        EventServer<_DataType> & operator = (const EventServer<_DataType> &);

        /*
         * Type for storing subscriptions.
         */
        typedef std::vector<ISubscription<_DataType> *> SubscriptionVector;

        SubscriptionVector mSubscriptions; //Stores subsriptions.
        void * mOwner; //Store the owners address.

    public:
        /*
         * Initialize the event handler.
         * aOwner: The address of the event handler owner.
         */
        EventServer(void * aOwner)
            : mSubscriptions(),
              mOwner(aOwner)
        {
        }

        /*
         * Destroy all subscriptions.
         */
        void DestroyAllSubscription() throw()
        {
            SubscriptionVector::size_type subCount = mSubscriptions.size();
            for (SubscriptionVector::size_type i = 0; i < subCount; i++)
                delete mSubscriptions[i];
            mSubscriptions.clear();
        }

        /*
         * Destructor. Destroy all subscriptions.
         */
        ~EventServer() throw()
        {
            DestroyAllSubscription();
        };

        /*
         * Fire the event.
         * aSender: Sender of the event.
         * aData: Data to be sent to the subsribers.
         */
        void Fire(void * aSender, _DataType aData)
        {
            SubscriptionVector::size_type subCount = mSubscriptions.size();
            for (SubscriptionVector::size_type i = 0; i < subCount; i++)
                mSubscriptions[i]->Call(aSender, aData);
        }

        /*
         * Fire the event. The sender is the owner of the event.
         * aData: Data to be sent to the subsribers.
         */
        inline void Fire(_DataType aData)
        {
            Fire(mOwner, aData);
        }

        /*
         * Add a new subsription.
         * aNewSub: The new subscription.
         * return: The new subscription.
         */
        inline ISubscription<_DataType> * operator += (ISubscription<_DataType> * aNewSub)
        {
            mSubscriptions.push_back(aNewSub);
            return aNewSub;
        }

        /*
         * Add a new subsription.
         * aNewSub: The new subscription.
         * return: The new subscription.
         */
        inline ISubscription<_DataType> * AddSubscription(ISubscription<_DataType> * aNewSub)
        {
            mSubscriptions.push_back(aNewSub);
            return aNewSub;
        }

        /*
         * Returns true if the event is subscribed otherwise false.
         */
        inline bool IsSubscribed()
        {
            return mSubscriptions.size != 0;
        }

        /*
         * Remove subcription and destroy it.
         * aSub: Subcription to be removed.
         * return: The removed subcsription.
         */
        void operator -= (ISubscription<_DataType> * aSub)
        {
            SubscriptionVector::size_type subCount = mSubscriptions.size();
            for (SubscriptionVector::size_type i = 0; i < subCount; i++)
                if (aSub == mSubscriptions[i]) {
                    delete mSubscriptions[i];
                    mSubscriptions.erase(mSubscriptions.begin() + i);
                }
        }

        /*
         * Remove subcription and destroy it.
         * aSub: Subcription to be removed.
         * return: The removed subcsription.
         */
        inline void RemoveSubscription(ISubscription<_DataType> * aSub)
        {
            (*this) -= aSub;
        }

        /*
         * Removes all subscriptions from the EventServer that made by "aSubber"
         * aSubber: the subscriber.
         */
        void operator /= (void * aSubber)
        {
            SubscriptionVector::iterator iter(mSubscriptions.begin());
            while (iter != mSubscriptions.end()) {
                if ((*iter)->GetSubscriber() == aSubber)
                    iter = mSubscriptions.erase(iter);
                else
                    iter++;
            }
        }

        /*
         * Remove all subscriptions of the subscriber and destroy them.
         * aSubber: the subscriber.
         */
        void DestroyAllSubscriptionBy(void * aSubber)
        {
            (*this) /= aSub;
        }

        /*
         * Gets the owner of the event handler.
         */
        inline void * GetOwner() throw()
        {
            return mOwner;
        }

    };


    //================================================================================================


    /*
     * Subcription class (for member functions).
     * _DataType: The type of the data send by the owner when the event is fired.
     * _Class: type of the subscriber class.
     */
    template <typename _DataType, typename _Class>
    class ObjSubscription : public ISubscription<_DataType>
    {
    public:
        /*
         * Type of the subsribed member function.
         */
        typedef void (_Class::*MemberFunc)(void *, _DataType);

    private:
        /*
         * Non copyable class.
         */
        ObjSubscription(const ObjSubscription<_DataType, _Class> &);
        ObjSubscription<_DataType, _Class> & operator = (const ObjSubscription<_DataType, _Class> &);

        MemberFunc mMemberFunc; //Subsribed member function.
        _Class * mSubscriber; //Subscriber class.

    public:
        /*
         * Initialize the subsription.
         * aSubber: The subsribing class.
         * aFunc: The subsribed member function.
         */
        ObjSubscription(_Class * aSubber, MemberFunc aFunc)
            : mSubscriber(aSubber),
              mMemberFunc(aFunc)
        {
        }

        /*
         * Destructor. Virtual.
         */
        virtual ~ObjSubscription()
        {
        }

        /*
         * Call the subsriber. Virtual.
         * aSender: The sender of the event.
         * aData: Data send with the event.
         */
        virtual void Call(void * aSender, _DataType aData)
        {
            (mSubscriber->*mMemberFunc)(aSender, aData);
        }

        /*
         * Gets the address of the subscriber. Virtual.
         */
        virtual void * GetSubscriber() const
        {
            return mSubscriber;
        }
    };


    //================================================================================================


    /*
     * Subcription class (for static functions).
     * _DataType: The type of the data send by the owner when the event is fired.
     */
    template <typename _DataType>
    class StaticSubscription : public ISubscription<_DataType>
    {
    public:
        /*
         *
         */
        typedef void (* Func)(void *, _DataType);

    private:
        /*
         * Non copyable class.
         */
        StaticSubscription(const StaticSubscription<_DataType> &);
        StaticSubscription<_DataType> & operator = (const StaticSubscription<_DataType> &);

        Func mSubbedFunc;

    public:
        /*
         * Initialize the subscription.
         * aSubber: The subsribing function.
         */
        StaticSubscription(Func aSubber)
            : mSubbedFunc(aSubber)
        {
        }

        /*
         * Destructor. Virtual.
         */
        virtual ~StaticSubscription()
        {
        }

        /*
         * Call the subsriber. Virtual.
         * aSender: The sender of the event.
         * aData: Data send with the event.
         */
        virtual void Call(void * aSender, _DataType aData)
        {
            (*mSubbedFunc)(aSender, aData);
        }

        /*
         * Gets the address of the subscriber.
         */
        virtual void * GetSubscriber() const
        {
            return mSubbedFunc;
        }
    };


} /*namespace Event*/
#endif
