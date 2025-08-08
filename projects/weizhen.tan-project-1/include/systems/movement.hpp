/**
@file    movement.hpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the declaration of the Movement system class.

*//*__________________________________________________________________________*/

#ifndef MOVEMENT_SYSTEM_HPP
#define MOVEMENT_SYSTEM_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <systems/isystem.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class Movement
 * @brief This class is responsible for update of transform components.
 */
class Movement : public ISystem
{

public:

    void Init() override;
    void Update() override;
    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    Movement() {};
    ~Movement() {};

private:
    void UpdateCurrentSelectedEntity();
};

#endif /* MOVEMENT_SYSTEM_HPP */