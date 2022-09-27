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

// Originally written by Amer Saffo, but heavily modified to fit the needs of this project

#pragma once

#include "Engine/Api/Api.hpp"

#include <list>
#include <memory>

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
	template<typename... T>
	class ENGINE_LOCAL EventHandlerImplBase {
	public:
		EventHandlerImplBase() = default;  // needed to define the constructor since we defined the destructor (compiler would otherwise object)

		virtual ~EventHandlerImplBase() = default; // destructor should be made virtual for base as we are going to delete through a base pointer
		EventHandlerImplBase(const EventHandlerImplBase& other) = delete;
		EventHandlerImplBase(EventHandlerImplBase&& other) noexcept = delete;

		EventHandlerImplBase& operator=(const EventHandlerImplBase& other) = delete;
		EventHandlerImplBase& operator=(EventHandlerImplBase&& other) noexcept = delete;

		virtual bool IsBoundToSameFunctionAs(std::shared_ptr<EventHandlerImplBase<T...>>) = 0;   // verify if both handlers are bound to the same function.

		/** convenient function to make inheriting classes implementation of IsBoundToSameFunctionAs a little bit easier*/
		bool IsSameType(std::shared_ptr<EventHandlerImplBase<T...>> pHandler2) {
			if (!pHandler2 ||  // a null pointer can never be the same as one that points to an actual handler
				typeid(*this) != typeid(*pHandler2))
				return false;

			return true;
		}
	};

	//------------------------------------
	// any argument event handlers support
	//------------------------------------

	template<typename... T>
	class ENGINE_LOCAL  EventHandlerImpl : public EventHandlerImplBase<T...> {
	public:
		virtual void OnEvent(T...) = 0;  // will be called eventually when a Event is raised
	};

	/** A handler non-member function calls */
	template<typename... T>
	class ENGINE_LOCAL EventHandlerImplForNonMemberFunction final : public EventHandlerImpl<T...> {
	public:
		/** Saves the passed function for use later when an event is raised */
		explicit EventHandlerImplForNonMemberFunction(void(*functionToCall)(T...))
			: m_pFunctionToCall(functionToCall) {}

		/** will be called eventually when an Event is raised */
		virtual void OnEvent(T... evt) override {
			m_pFunctionToCall(evt...);
		}

		/** verify if this handler will eventually call the same function as the passed handler. */
		virtual bool IsBoundToSameFunctionAs(std::shared_ptr<EventHandlerImplBase<T...>> pHandler2) override {
			if (!IsSameType(pHandler2))
				return false;

			// they are the same type so we can safely cast to this class type
			std::shared_ptr<EventHandlerImplForNonMemberFunction<T...>> pHandlerCast = std::dynamic_pointer_cast<EventHandlerImplForNonMemberFunction<T...>>(pHandler2);
			if (!pHandlerCast) {
				// error, should never happen
				return false;
			}

			return this->m_pFunctionToCall == pHandlerCast->m_pFunctionToCall;
		}

	private:
		void(*m_pFunctionToCall)(T...); // passed in the constructor. Will get called when an event is raised.
	};

	/** A helper that handles member function calls */
	template<typename U, typename... T>
	class ENGINE_LOCAL EventHandlerImplForMemberFunction final : public EventHandlerImpl<T...> {
	public:
		/** Saves the passed function for use later when an event arrive */
		EventHandlerImplForMemberFunction(U * thisPtr, void(U::* memberFunctionToCall)(T...))
			: m_pCallerInstance(thisPtr)
			, m_pMemberFunction(memberFunctionToCall) {}

		/** will be called eventually when an Event is raised */
		virtual void OnEvent(T... evt) override {
			if (m_pCallerInstance)
				(m_pCallerInstance->*m_pMemberFunction)(evt...);
		}

		/** verify if this handler will eventually call the same function as the passed handler. */
		virtual bool IsBoundToSameFunctionAs(std::shared_ptr<EventHandlerImplBase<T...>> pHandler2) override {
			if (!IsSameType(pHandler2))
				return false;

			// they are the same type so we can safely cast to this class type
			std::shared_ptr<EventHandlerImplForMemberFunction<U, T...>> pHandlerCast = std::dynamic_pointer_cast<EventHandlerImplForMemberFunction<U, T...>>(pHandler2);
			if (!pHandlerCast) {
				// error, should never happen
				return false;
			}

			return this->m_pCallerInstance == pHandlerCast->m_pCallerInstance && this->m_pMemberFunction == pHandlerCast->m_pMemberFunction;
		}

	private:
		U * m_pCallerInstance;  // passed in the constructor. This watcher will only be used to call a member function, so m_pCallerInstance would hold the object through which that member function is called.

		void(U::* m_pMemberFunction)(T...); // passed in the constructor. This watcher will only be used to call a member function through m_pCallerInstance.
	};

	//=============================================================================
	// #endregion EventHandlerImpl
	//=============================================================================

	//=============================================================================
	// #region EventHandler
	//=============================================================================

	/** a syntax-sugar class that is used to assign a function as a handler to an Event
	 * {@code myEvent += EventHandler::Bind(OnMessage);}  // for non member or static functions
	 * {@code myEvent += EventHandler::Bind(this, &ThisClass::OnMessage);}  // called inside ThisClass constructor as recommended
	 * @author Amer Saffo
	 */
	class ENGINE_API EventHandler {
	public:
		//------------------------------------
		// any argument event handlers support
		//------------------------------------

		/**
		 * to be used when setting the event handler function to an event
		 * the event data type T should be the same as the one used to create the Event or the compiler will complain
		 * that the the event can't accept the bound handler
		 * @note : EventHandler::Bind() was designed to be used ONLY to set event handlers to Event. // myEvent += EventHandler::Bind(...)
		 * It is important to note that Event will own the memory created by Bind when the created event handler is assigned to it by its += call,
		 * and so Event would destroy that memory when it no longer need it. So make sure not to keep the returned pointer yourself.
		 */
		template<typename... T>
		static std::shared_ptr<EventHandlerImpl<T...>> Bind(void(*nonMemberFunctionToCall)(T...)) {
			return std::make_shared<EventHandlerImplForNonMemberFunction<T...>>(nonMemberFunctionToCall());
		}

		/** @overload */
		template<typename U, typename... T>
		static std::shared_ptr<EventHandlerImpl<T...>> Bind(U* thisPtr, void(U::* memberFunctionToCall)(T...)) {
			return std::make_shared<EventHandlerImplForMemberFunction<U, T...>>(thisPtr, memberFunctionToCall);
		}

		EventHandler() = delete;  // default constructor made private to prevent creating instances of this class. EventHandler only purpose is to provide Event with the Bind function
	};

	//=============================================================================
	// #endregion EventHandler
	//=============================================================================

	//=============================================================================
	// #region Event
	//=============================================================================

	/**
	 * Sharp Event provide an event mechanism that is similar to that found in C#
	 * {@code myEvent += EventHandler::Bind(this, &ThisClass::OnMessage);}  // called inside ThisClass constructor as recommended
	 * @author Amer Saffo
	 */
	template<typename... T>
	class ENGINE_LOCAL EventBase {
	public:
		EventBase() = default;
		virtual ~EventBase() = default;

		/**
		 * it is by design that Event is the owner of the memory of all the handlers assigned to it
		 * so it is the duty of the destructor to erase that memory
		 * @see Eventhandler Bind function documentation for more details
		 */
		//virtual ~EventBase() {
		//	for (std::shared_ptr<EventHandlerImplBase<T...>> handler : m_eventHandlers) {
		//		if (handler) {
		//			handler.reset();
		//			//delete handler;
		//			//handler = nullptr;  // just to be consistent
		//		}
		//	}
		//	m_eventHandlers.clear();
		//}

		EventBase(const EventBase& other) = delete;
		EventBase(EventBase&& other) noexcept = delete;

		EventBase& operator=(const EventBase& other) = delete;
		EventBase& operator=(EventBase&& other) noexcept = delete;

		/**
		 * This is how you connect a handler to this event.
		 * {@code myEvent += EventHandler::Bind(this, &ThisClass::OnMessage);}  // example of binding inside ThisClass constructor as recommended
		 * {@code myEvent -= EventHandler::Bind(this, &ThisClass::OnMessage);}  // example of unbinding in destructor
		 * @note : You can Bind to a private member function as long as you have access to that private function at the time of binding (i.e binding from within the class).
		 * @note on memory: As a rule of thumb, always bind in the constructor and unbind in the destructor.
		 * However, you can go lazy and rely on the Event destructor to unbind if:
		 * [1] if you are passing [this] to EventHandler::Bind() AND the event is a member, which is the most common scenario.
		 * as that means the member event object will be automatically destroyed by the class destructor.
		 * [2] or if you bound to a non member function, as those are never destroyed.
		 * In short, before your bound class instance is destroyed, make sure to unbind it. Otherwise, the bound event might try to make a call through your destroyed instance.
		 * @note : For completeness, you are warned NOT to store the returned value of Eventhandler::Bind() yourself, as after this call, Event becomes the owner of the implicitly created EventHandlerImpl<T> and it later destroys it.
		 */
		void operator += (std::shared_ptr<EventHandlerImpl<T...>> pHandlerToAdd) {
			// bellow is commented because we decided to let the user add the same handler multiple time and make it his responsibility to remove all those added
			//if( FindHandlerWithSameBinding(pHandlerToAdd) != m_eventHandlers.end())

			if (pHandlerToAdd) {
				// the handler added bellow along with all handlers in the list will be called later when an event is raised
				m_eventHandlers.push_back(pHandlerToAdd);
			}
		}

		/**
		 * you can use this to remove a handler you previously added.
		 * @note : removing a handler that was already removed is harmless, as this call does nothing and simply return when it does not find the handler.
		 * {@code myEvent -= EventHandler::Bind(this, &ThisClass::OnMessage);}  // example of unbinding in destructor
		 */
		void operator -= (std::shared_ptr<EventHandlerImpl<T...>> pHandlerToRemove) {
			if (!pHandlerToRemove)
				return;  // a null passed, so nothing to do

			// search for a handler that has the same binding as the passed one
			for (std::shared_ptr<EventHandlerImpl<T...>> handler : m_eventHandlers) {
				if (pHandlerToRemove->IsBoundToSameFunctionAs(handler)) {
					// erase the memory that was created by the Bind function
					// this memory is that of an EventHandler class and has nothing to do with the actual functions/class passed to it on Bind
					if (handler)
						handler.reset();

					// remove it form the list (safe to do it here as we'll break the loop)
					m_eventHandlers.remove(handler);
					break;
				}
			}

			// also delete the passed handler as we don't need it anymore (by design, Event always owns the memory of the handlers passed to it)
			if (pHandlerToRemove)
				pHandlerToRemove.reset();
		}

	protected:
		std::list<std::shared_ptr<EventHandlerImpl<T...>>> m_eventHandlers;  // all handlers will be notified when operator() is called.
	};

	//------------------------------------
	// any argument event handlers support
	//------------------------------------

	template<typename... T>
	class ENGINE_API Event final : public EventBase<T...> {
	public:
		/**
		* this call actually raises the event.
		* It does so by passing the event data to all event handlers.
		* {@code myEvent(data);}  // this how you would normally raise an event
		*/
		void operator()(T... eventData) {
			// raise the event by telling all the handlers
			for (std::shared_ptr<EventHandlerImpl<T...>> handler : m_eventHandlers) {
				if (handler)
					handler->OnEvent(eventData...);  // this is a virtual function that will eventually call the function passed to Eventhandler::Bind() for this handler
			}
		}
	};

	//=============================================================================
	// #endregion Event
	//=============================================================================
}