/**
@file    isingleton.hpp
@author  weizhen.tan@digipen.edu
@date    08/05/2025

This file contains the declaration of the ISingleton class template.

*//*__________________________________________________________________________*/

#ifndef ISINGLETON_HPP
#define ISINGLETON_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

/**
  * @class ISingleton
  * @brief Singleton design pattern that ensures that a class only has one instance,
  *	and you can access the instance globally.
  */
template <typename T>
class ISingleton
{

public:

	/**
	 * @brief Returns the instance, creates a new instance if it does not exist.
	 */
	static T& GetInstance();

protected:

	/**
	 * @brief Protected Constructor so that instance can only be created in
	 *	GetInstance().
	 */
	ISingleton() = default;

	/**
	 * @brief Deleted copy and move Constructors.
	 */

	ISingleton(const ISingleton&) = delete;
	ISingleton& operator=(const ISingleton&) = delete;
	ISingleton(ISingleton&&) noexcept = delete;
	ISingleton& operator=(const ISingleton&&) = delete;


	/**
	 * @brief Protected Destructor, virtual function because classes will
	 *  inherit from this class.
	 */
	virtual ~ISingleton() = default;

};

template <typename T>
T& ISingleton<T>::GetInstance()
{
	static T s_Instance{};
	return s_Instance;
}

#endif /* ISINGLETON_HPP */