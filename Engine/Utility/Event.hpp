//=============================================================================
// 
// (C) Copyright 2015 Amer Saffo (amer_saffo@yahoo.com)
// Distributed under the Code Project Open License 1.02 (CPOL)
// http://www.codeproject.com/info/cpol10.aspx
// 
//=============================================================================

//=============================================================================
 // version 1.1
 //=============================================================================

#pragma once

//-----------------------------------------------------------------------------
// define SHARP_EVENT_NO_BOOST to have no dependencies and manage thread-safety
// at the application level. Boost is used ONLY to make this class thread-safe.
//-----------------------------------------------------------------------------
//#ifndef SHARP_EVENT_NO_BOOST
//
//#include <ThirdParty/boost_1_65_1/boost/thread/locks.hpp>
//#include <ThirdParty/boost_1_65_1/boost/thread/shared_mutex.hpp>
//#endif // SHARP_EVENT_NO_BOOST

#include "Engine/api.hpp"
#include <ThirdParty/EASTL-master/include/EASTL/list.h>
#include <ThirdParty/EASTL-master/include/EASTL/utility.h>

namespace Sharp    // short for SharpTools
{
	//=============================================================================
	   // #region EventHandlerImpl
	   //=============================================================================

	   /**
	   * base class for all event handlers
	   * event handlers are used internally by Event when EventHandler::Bind() is called.
	   * @note : You won't create one yourself
	   * @author Amer Saffo
	   */
	template<typename T>
	class ENGINE_API EventHandlerImplBase
	{
	public:
		EventHandlerImplBase() {}  // needed to define the constructor since we defined the destructor (compiler would otherwise object)

		virtual ~EventHandlerImplBase() {} // destructor should be made virtual for base as we are going to delete through a base pointer

		virtual bool IsBindedToSameFunctionAs(EventHandlerImplBase<T>*) = 0;   // verify if both handlers are binded to the same function.



   /** convenient function to make inheriting classes implementation of IsBindedToSameFunctionAs a little bit easier*/

		bool IsSametype(EventHandlerImplBase<T>* pHandler2)
		{
			if (!pHandler2 ||  // a null pointer can never be the same as one that points to an actual handler
				typeid(*this) != typeid(*pHandler2))
			{
				return false;
			}

			return true;
		}
	};

	//------------------------------------
	// one argument event handlers support
	//------------------------------------

	template<typename T>
	class ENGINE_API EventHandlerImpl : public EventHandlerImplBase<T>
	{
	public:
		virtual void OnEvent(T&) = 0;  // will be called eventually when a Event is raised

	};



	/** A handler non-member function calls */

	template<typename T>
	class ENGINE_API EventHandlerImplForNonMemberFunction : public EventHandlerImpl<T>
	{
	public:
		/** Saves the passed function for use later when an event is raised */
		EventHandlerImplForNonMemberFunction(void(*functionToCall)(T&))
			: m_pFunctionToCall(functionToCall)
		{
		}

		/** will be called eventually when an Event is raised */
		virtual void OnEvent(T& evt)
		{
			m_pFunctionToCall(evt);
		}

		/** verify if this handler will eventually call the same function as the passed handler. */
		virtual bool IsBindedToSameFunctionAs(EventHandlerImplBase<T>* pHandler2)
		{
			if (!IsSametype(pHandler2))
			{
				return false;
			}

			// they are the same type so we can safely cast to this class type
			EventHandlerImplForNonMemberFunction<T>* pHandlerCasted = dynamic_cast<EventHandlerImplForNonMemberFunction<T>*>(pHandler2);
			if (!pHandlerCasted)
			{
				// error, should never happen
				return false;
			}

			return this->m_pFunctionToCall == pHandlerCasted->m_pFunctionToCall;
		}

	private:
		void(*m_pFunctionToCall)(T&); // passed in the constructor. Will get called when an event is raised.

	};



	/** A helper that handles member function calls */

	template<typename T, typename U>
	class ENGINE_API EventHandlerImplForMemberFunction : public EventHandlerImpl<T>
	{
	public:
		/** Saves the passed function for use later when an event arrive */
		EventHandlerImplForMemberFunction(void(U::*memberFunctionToCall)(T&), U* thisPtr)
			: m_pCallerInstance(thisPtr)
			, m_pMemberFunction(memberFunctionToCall)
		{
		}

		/** will be called eventually when an Event is raised */
		virtual void OnEvent(T& evt)
		{
			if (m_pCallerInstance)
			{
				(m_pCallerInstance->*m_pMemberFunction)(evt);
			}
		}

		/** verify if this handler will eventually call the same function as the passed handler. */
		virtual bool IsBindedToSameFunctionAs(EventHandlerImplBase<T>* pHandler2)
		{
			if (!IsSametype(pHandler2))
			{
				return false;
			}

			// they are the same type so we can safely cast to this class type
			EventHandlerImplForMemberFunction<T, U>* pHandlerCasted = dynamic_cast<EventHandlerImplForMemberFunction<T, U>*>(pHandler2);
			if (!pHandlerCasted)
			{
				// error, should never happen
				return false;
			}

			return this->m_pCallerInstance == pHandlerCasted->m_pCallerInstance && this->m_pMemberFunction == pHandlerCasted->m_pMemberFunction;
		}

	private:
		U* m_pCallerInstance;  // passed in the constructor. This watcher will only be used to call a member function, so m_pCallerInstance would hold the object through which that member function is called.

		void(U::*m_pMemberFunction)(T&); // passed in the constructor. This watcher will only be used to call a member function through m_pCallerInstance.

	};



	//------------------------------------

	// no arguments event handlers support

	//------------------------------------



	/**
	* a specialization of the EventHandlerImpl for when void is passed
	* which happen when user define an Event<void>
	*/
	template<>
	class ENGINE_API EventHandlerImpl<void> : public EventHandlerImplBase<void>
	{
	public:
		virtual void OnEvent() = 0;  // will be called eventually when an Event is raised

	};



	/** A handler non-member void function calls */

	template<>
	class ENGINE_API EventHandlerImplForNonMemberFunction<void> : public EventHandlerImpl<void>
	{
	public:
		/** Saves the passed function for use later when an event is raised */
		EventHandlerImplForNonMemberFunction(void(*functionToCall)())
			: m_pFunctionToCall(functionToCall)
		{
		}

		/** will be called eventually when an Event is raised */
		virtual void OnEvent()
		{
			m_pFunctionToCall();
		}

		/** verify if this handler will eventually call the same function as the passed handler. */
		virtual bool IsBindedToSameFunctionAs(EventHandlerImplBase<void>* pHandler2)
		{
			if (!IsSametype(pHandler2))
			{
				return false;
			}

			// they are the same type so we can safely cast to this class type
			EventHandlerImplForNonMemberFunction<void>* pHandlerCasted = dynamic_cast<EventHandlerImplForNonMemberFunction<void>*>(pHandler2);
			if (!pHandlerCasted)
			{
				// error, should never happen
				return false;
			}

			return this->m_pFunctionToCall == pHandlerCasted->m_pFunctionToCall;
		}

	private:
		void(*m_pFunctionToCall)(); // passed in the constructor. Will get called when an event is raised.

	};



	/** A helper that handles void member function calls */

	template<typename U>
	class ENGINE_API EventHandlerImplForMemberFunction<void, U> : public EventHandlerImpl<void>
	{
	public:
		/** Saves the passed function for use later when an event arrive */
		EventHandlerImplForMemberFunction(void(U::*memberFunctionToCall)(), U* thisPtr)
			: m_pCallerInstance(thisPtr)
			, m_pMemberFunction(memberFunctionToCall)
		{
		}

		/** will be called eventually when an Event is raised */
		virtual void OnEvent()
		{
			if (m_pCallerInstance)
			{
				(m_pCallerInstance->*m_pMemberFunction)();
			}
		}

		/** verify if this handler will eventually call the same function as the passed handler. */
		virtual bool IsBindedToSameFunctionAs(EventHandlerImplBase<void>* pHandler2)
		{
			if (!IsSametype(pHandler2))
			{
				return false;
			}

			// they are the same type so we can safely cast to this class type
			EventHandlerImplForMemberFunction<void, U>* pHandlerCasted = dynamic_cast<EventHandlerImplForMemberFunction<void, U>*>(pHandler2);
			if (!pHandlerCasted)
			{
				// error, should never happen
				return false;
			}

			return this->m_pCallerInstance == pHandlerCasted->m_pCallerInstance && this->m_pMemberFunction == pHandlerCasted->m_pMemberFunction;
		}

	private:
		U* m_pCallerInstance;  // passed in the constructor. This watcher will only be used to call a member function, so m_pCallerInstance would hold the object through which that member function is called.

		void(U::*m_pMemberFunction)(); // passed in the constructor. This watcher will only be used to call a member function through m_pCallerInstance.

	};



	//=============================================================================

	// #endregion EventHandlerImpl

	//=============================================================================



	//=============================================================================

	// #region EventHandler

	//=============================================================================



	/** a syntax-sugar class that is used to assign a function as a handler to an Event

	* {@code myEvent += EventHandler::Bind(OnMessage);}  // for non member or static functions

	* {@code myEvent += EventHandler::Bind(&ThisClass::OnMessage, this);}  // called inside ThisClass constructor as recommended

	* @author Amer Saffo

	*/

	class ENGINE_API EventHandler

	{

	public:



		//------------------------------------

		// one argument event handlers support

		//------------------------------------



		/**

		* to be used when setting the event handler function to an event

		* the event data type T should be the same as the one used to create the Event or the compiler will complain

		* that the the event can't accept the binded handler

		* @note : EventHandler::Bind() was designed to be used ONLY to set event handlers to Event. // myEvent += EventHandler::Bind(...)

		*         It is important to note that Event will own the memory created by Bind when the created event handler is assigned to it by its += call,

		*         and so Event would destroy that memory when it no longer need it. So make sure not to keep the returned pointer yourself.

		*/

		template<typename T>
		static EventHandlerImpl<T>* Bind(void(*nonMemberFunctionToCall)(T&))
		{
			return new EventHandlerImplForNonMemberFunction<T>(nonMemberFunctionToCall);
		}

		/** @overload */
		template<typename T, typename U>
		static EventHandlerImpl<T>* Bind(void(U::*memberFunctionToCall)(T&), U* thisPtr)
		{
			return new EventHandlerImplForMemberFunction<T, U>(memberFunctionToCall, thisPtr);
		}

		//------------------------------------
		// no arguments event handlers support
		//------------------------------------

		/** @overload */
		static EventHandlerImpl<void>* Bind(void(*nonMemberFunctionToCall)())
		{
			return new EventHandlerImplForNonMemberFunction<void>(nonMemberFunctionToCall);
		}

		/** @overload */
		template<typename U>
		static EventHandlerImpl<void>* Bind(void(U::*memberFunctionToCall)(), U* thisPtr)
		{
			return new EventHandlerImplForMemberFunction<void, U>(memberFunctionToCall, thisPtr);
		}

	private:
		EventHandler();  // default constructor made private to prevent creating instances of this class. EventHandler only purpose is to provide Event with the Bind function

	};



	//=============================================================================

	// #endregion EventHandler

	//=============================================================================



	//=============================================================================

	// #region Event

	//=============================================================================



	/**

			 * Sharp Event provide an event mechanism that is similar to that found in C#

			 * {@code myEvent += EventHandler::Bind(&ThisClass::OnMessage, this);}  // called inside ThisClass constructor as recommended

			 * @author Amer Saffo

			 */

	template<typename T>
	class ENGINE_API EventBase
	{
	public:
		EventBase() {}

		/**
					 * it is by design that Event is the owner of the memory of all the handlers assigned to it
					 * so it is the duty of the destructor to erase that memory
					 * @see Eventhandler Bind function documentation for more details
					 */
		virtual ~EventBase()
		{
//#ifndef SHARP_EVENT_NO_BOOST
			// we are going to modify the handlers list, so we need a write lock.
			//WriteLock handlersWriteLock(m_handlersMutex);
//#endif // SHARP_EVENT_NO_BOOST

			for (eastl::list< EventHandlerImpl<T>* >::iterator iter = m_eventHandlers.begin(); iter != m_eventHandlers.end(); ++iter)
			{
				EventHandlerImpl<T>* pHandler = *iter;
				if (pHandler)
				{
					delete pHandler;
					pHandler = 0;  // just to be consistent
				}
			}
			m_eventHandlers.clear();
		}

		/**
					 * This is how you connect a handler to this event.
				 * {@code myEvent += EventHandler::Bind(&ThisClass::OnMessage, this);}  // example of binding inside ThisClass constructor as recommended
					 *   {@code myEvent -= EventHandler::Bind(&ThisClass::OnMessage, this);}  // example of unbinding in destructor
					 * @note : You can Bind to a private member function as long as you have access to that private function at the time of binding (i.e binding from within the class).
					 * @note on memory: As a rule of thumb, always bind in the constructor and unbind in the destructor.
					 *                                    However, you can go lazy and rely on the Event destructor to unbind if:
					 *                                    [1] if you are passing [this] to EventHandler::Bind() AND the event is a member, which is the most common scenario.
					 *                                            as that means the member event object will be automatically destroyed by the class destructor.
					 *                                    [2] or if you binded to a non member function, as those are never destroyed.
					 *   In short, before your binded class instance is destroyed, make sure to unbind it. Otherwise, the binded event might try to make a call through your destroyed instance.
					 * @note : For completeness, you are warned NOT to store the returned value of Eventhandler::Bind() yourself, as after this call, Event becomes the owner of the implicitly created EventHandlerImpl<T> and it later destroys it.
					 */
		EventBase<T>& operator += (EventHandlerImpl<T>* pHandlerToAdd)
		{
			// bellow is commented because we decided to let the user add the same handler multiple time and make it his responsibility to remove all those added
				//if( FindHandlerWithSameBinding(pHandlerToAdd) != m_eventHandlers.end())

			if (pHandlerToAdd)
			{

//#ifndef SHARP_EVENT_NO_BOOST
				// we are going to modify the handlers list, so we need a write lock.
				//WriteLock handlersWriteLock(m_handlersMutex);
//#endif // SHARP_EVENT_NO_BOOST

				// the handler added bellow along with all handlers in the list will be called later when an event is raised
				m_eventHandlers.push_back(pHandlerToAdd);
			}

			return *this;
		}

		/**
					 * you can use this to remove a handler you previously added.
					 * @note : removing a handler that was already removed is harmless, as this call does nothing and simply return when it does not find the handler.
					 *   {@code myEvent -= EventHandler::Bind(&ThisClass::OnMessage, this);}  // example of unbinding in destructor
					 */
		EventBase<T>& operator -= (EventHandlerImpl<T>* pHandlerToRemove)
		{
			if (!pHandlerToRemove)
			{
				return *this;  // a null passed, so nothing to do
			}

//#ifndef SHARP_EVENT_NO_BOOST
			// we start by searching the handlers list and modify it ONLY when the passed handler is found
			//UpgradeableReadLock handlersReadLock(m_handlersMutex);  // acquire a read lock for the search and switch to a write lock later when the handler is found and is to be deleted.
//#endif // SHARP_EVENT_NO_BOOST

			// search for a handler that has the same binding as the passed one
			// search linearly (no other way)
			for (eastl::list< EventHandlerImpl<T>* >::iterator iter = m_eventHandlers.begin(); iter != m_eventHandlers.end(); ++iter)
			{
				EventHandlerImpl<T>* pHandler = *iter;
				if (pHandlerToRemove->IsBindedToSameFunctionAs(pHandler))
				{
//#ifndef SHARP_EVENT_NO_BOOST
					// found the handler, we need to get a write lock as we are going to modify the handlers list.
					//UpgradedWriteLock hanldersWriteLock(handlersReadLock);  // this get a write lock without releasing the read lock already acquired.
//#endif // SHARP_EVENT_NO_BOOST

					// erase the memory that was created by the Bind function
					// this memory is that of an EventHandler class and has nothing to do with the actual functions/class passed to it on Bind
					EventHandlerImpl<T>* pFoundHandler = *iter;
					if (pFoundHandler)
					{
						delete pFoundHandler;
						pFoundHandler = 0;
					}

					// remove it form the list (safe to do it here as we'll break the loop)
					m_eventHandlers.erase(iter);
					break;
				}
			}

			// also delete the passed handler as we don't need it anymore (by design, Event always owns the memory of the handlers passed to it)
			if (pHandlerToRemove)
			{
				delete pHandlerToRemove;
				pHandlerToRemove = 0;
			}

			return *this;
		}

	private:
		EventBase(const EventBase&);  // private to disable copying

		EventBase& operator=(const EventBase&); // private to disable copying


	protected:

		eastl::list< EventHandlerImpl<T>* > m_eventHandlers;  // all handlers will be notified when operator() is called.



//#ifndef SHARP_EVENT_NO_BOOST

		//typedef boost::unique_lock< boost::shared_mutex > WriteLock;
		//typedef boost::shared_lock< boost::shared_mutex > ReadLock;
		//typedef boost::upgrade_lock< boost::shared_mutex > UpgradeableReadLock;
		//typedef boost::upgrade_to_unique_lock< boost::shared_mutex > UpgradedWriteLock;  // upgrade an upgradeable read lock to a write lock without releasing it.
		//boost::shared_mutex m_handlersMutex; // used a shared mutex because we don't want threads raising the event to block each other. while adding/removing a handler should block all access.

//#endif // SHARP_EVENT_NO_BOOST

	};



	//------------------------------------

	// one argument event handlers support

	//------------------------------------

	template<typename T>
	class ENGINE_API Event : public EventBase<T>
	{
	public:
		/**
		* this call actually raises the event.
		* It does so by passing the event data to all event handlers.
		* {@code myEvent(data);}  // this how you would normally raise an event
		*/
		void operator()(T& eventData)
		{
//#ifndef SHARP_EVENT_NO_BOOST
			// this event just go through the list without modifying it, so a read lock is enough.
			// we could have copied the handler list and released the lock, but we didn't do that for two reasons:
			// 1. a thread removing a handler deletes that handler memory, and so we need to lock the usage of each handler and not only the retrieval of it from the list.
			// 2. no performance issue here as reading threads don't block each other, and adding/removing a handler is expected to happen a few times only.
			//ReadLock handlersReadLock(m_handlersMutex);
//#endif // SHARP_EVENT_NO_BOOST

			// raise the event by telling all the handlers
			for (eastl::list< EventHandlerImpl<T>* >::iterator iter = m_eventHandlers.begin(); iter != m_eventHandlers.end(); ++iter)
			{
				EventHandlerImpl<T>* pHandler = *iter;
				if (pHandler)
				{
					pHandler->OnEvent(eventData);  // this is a virtual function that will eventually call the function passed to Eventhandler::Bind() for this handler
				}
			}
		}
	};

	//----------------------------
	// no arguments event handlers support
	//----------------------------
	template<>
	class ENGINE_API Event<void> : public EventBase<void>
	{
	public:
		/**
		* this call actually raises the event.
		* It does so by calling all event handlers.
		* {@code myEvent();}  // this how you would normally raise a void event
		*/
		void operator()()
		{
//#ifndef SHARP_EVENT_NO_BOOST
			// this event just go through the list without modifying it, so a read lock is enough.
			// we could have copied the handler list and released the lock, but we didn't do that for two reasons:
			// 1. a thread removing a handler deletes that handler memory, and so we need to lock the usage of each handler and not only the retrieval of it from the list.
			// 2. no performance issue here as reading threads don't block each other, and adding/removing a handler is expected to happen a few times only.
			//ReadLock handlersReadLock(m_handlersMutex);
//#endif // SHARP_EVENT_NO_BOOST

			// raise the event by telling all the handlers
			for (eastl::list< EventHandlerImpl<void>* >::iterator iter = m_eventHandlers.begin(); iter != m_eventHandlers.end(); ++iter)
			{
				EventHandlerImpl<void>* pHandler = *iter;
				if (pHandler)
				{
					pHandler->OnEvent();  // this is a virtual function that will eventually call the function passed to Eventhandler::Bind() for this handler
				}
			}
		}
	};

	//=============================================================================
	// #endregion Event
	//=============================================================================
}