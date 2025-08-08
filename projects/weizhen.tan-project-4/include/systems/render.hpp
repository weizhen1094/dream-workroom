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

    /**
     * @brief Loads shaders, creates primitive meshes, loads models using assimp 
     * and creates bounding volumes for models loaded.
     */
    void Init() override;
    /**
    * @brief Contains the rendering logic.
    */
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