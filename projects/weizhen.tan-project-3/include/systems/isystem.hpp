/**
@file    isystem.hpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the declaration of the ISystem interface.

*//*__________________________________________________________________________*/

#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class ISystem
 * @brief This is the interface for system classes.
 */
class ISystem
{

public:

    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void CleanUp() = 0;

    ISystem() = default;
    virtual ~ISystem() = default;

};

#endif /* ISYSTEM_HPP */