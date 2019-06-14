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

#include <QFileInfo>

#include "Camera.hpp"
#include "GLHandler.hpp"
#include "Planet.hpp"
#include "Primitives.hpp"

#include "graphics/Utils.hpp"
#include "physics/CelestialBody.hpp"
//#include "../shaders/FarCelestialBodyShader.hpp"

class CelestialBodyRenderer //: public Renderer
{
  public:
	CelestialBodyRenderer(CelestialBody* drawnBody, double centralBodyRadius,
	                      double declinationTilt);
	void updateMesh(UniversalTime uT, Camera const& camera);
	void render();
	CelestialBody const* getDrawnBody() const { return drawnBody; };
	void setCenterPosition(float centerPosition)
	{
		this->centerPosition = centerPosition;
	};
	~CelestialBodyRenderer();

  private:
	void loadPlanet();
	void unloadPlanet(bool waitIfPlanetNotLoaded = false);

	CelestialBody* drawnBody;
	const double centralBodyRadius;
	const double declinationTilt;
	float centerPosition;
	int shaderParametersId;

	/*
	GLHandler::Mesh mesh;
	GLHandler::ShaderProgram shader;*/
	QMatrix4x4 model;
	float boundingSphere;
	bool culled = false;
	double apparentAngle;

	Planet* planet   = nullptr;
	bool customModel = false;
	QVector3D lightpos;
	float lightradius;
	QMatrix4x4 baseRotation;   // only align axis, no sideral time
	QMatrix4x4 properRotation; // full rotation, sideral time included
	std::array<QVector4D, 5>
	    neighborsPosRadius; // 3D position + radius of 5 closest neighbors
	std::array<QVector3D, 5>
	    neighborsOblateness; // oblateness of 5 closest neighbors

	// POINT
	GLHandler::ShaderProgram pointShader;
	GLHandler::Mesh pointMesh;

	// UNLOADED
	GLHandler::ShaderProgram unloadedShader;
	GLHandler::Mesh unloadedMesh;
};

#endif // CELESTIALBODYDRAWER_HPP
