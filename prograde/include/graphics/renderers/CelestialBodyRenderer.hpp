/*
    Copyright (C) 2018 Florian Cabot <florian.cabot@hotmail.fr>

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

#ifndef CELESTIALBODYRENDERER_HPP
#define CELESTIALBODYRENDERER_HPP

#include "GLHandler.hpp"
#include "Primitives.hpp"

#include "graphics/Utils.hpp"
#include "physics/CelestialBody.hpp"
//#include "../shaders/FarCelestialBodyShader.hpp"

class CelestialBodyRenderer //: public Renderer
{
  public:
	CelestialBodyRenderer(CelestialBody const* drawnBody);
	void updateMesh(UniversalTime uT,
	                        Vector3 const& cameraPos);
	void render();
	CelestialBody const* getDrawnBody() const { return drawnBody; };
	void setCenterPosition(float centerPosition)
	{
		this->centerPosition = centerPosition;
	};
	~CelestialBodyRenderer();

  private:
	CelestialBody const* drawnBody;
	float centerPosition;
	int shaderParametersId;

	GLHandler::Mesh mesh;
	GLHandler::ShaderProgram shader;
	QMatrix4x4 model;
};

#endif // CELESTIALBODYDRAWER_HPP
