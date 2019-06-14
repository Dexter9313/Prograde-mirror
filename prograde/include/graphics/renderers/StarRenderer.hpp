/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@hotmail.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#ifndef STARRENDERER_HPP
#define STARRENDERER_HPP

#include "Billboard.hpp"
#include "Camera.hpp"
#include "graphics/Utils.hpp"
#include "physics/Star.hpp"
#include "physics/UniversalTime.hpp"

class StarRenderer
{
  public:
	StarRenderer(Star const* drawnStar);
	void updateMesh(UniversalTime uT, Camera const& camera);
	void render(BasicCamera const& camera);
	~StarRenderer();

  private:
	Star const* drawnStar;

	double camDist;

	double billboardOriginalEdgeSize;
	Billboard billboard;

	// POINT
	GLHandler::ShaderProgram pointShader;
	GLHandler::Mesh pointMesh;
	QVector3D pointPos;
};

#endif // STARRENDERER_HPP
