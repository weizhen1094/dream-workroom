/**
@file    render.hpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the declaration of the Render system class.

*//*__________________________________________________________________________*/

#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <systems/isystem.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class Render
 * @brief This is the class for Render system.
 */
class Render : public ISystem
{

public:

    void Init() override;
    void Update() override;
    void CleanUp() override;

    /*
     * Ctors and Dtor.
     */

    Render()  {};
    ~Render() {};

private:
};

#endif /* RENDER_SYSTEM_HPP */