/**
@file    collision.hpp
@author  weizhen.tan@digipen.edu
@date    15/07/2025

This file contains the declaration of the Collision system class.

*//*__________________________________________________________________________*/

#ifndef COLLISION_SYSTEM_HPP
#define COLLISION_SYSTEM_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <systems/isystem.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class Collision
 * @brief This class is responsible for checking intersection between geometry primitives.
 */
class Collision : public ISystem
{

public:

    void Init() override;
    void Update() override;
    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    Collision() {};
    ~Collision() {};

private:
};

#endif /* COLLISION_SYSTEM_HPP */