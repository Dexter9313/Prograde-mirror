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

#ifndef PLANET_H
#define PLANET_H

#include "BasicCamera.hpp"
#include "GLHandler.hpp"
#include "Primitives.hpp"

class Planet
{
  public:
	Planet(float radius = 1.f);

	void initTerrestrial(QColor const& color = QColor(255, 255, 255),
	                     float polarLatitude = 90.f, float atmosphere = 0.f);
	void initGazGiant(QColor const& color  = QColor(255, 255, 255),
	                  float bandsIntensity = 1.f, float stormsIntensity = 1.f);
	void initFromTex(QString const& diffusePath);
	void initFromTex(QString const& diffusePath, QString const& normalPath,
	                 float atmosphere = 0.f);

	void initRing(float innerRing, float outerRing,
	              QString const& texturePath = "");
	void updateRing();
	void renderPlanet(QVector3D const& pos, QVector3D const& lightpos);
	void renderPlanet(QMatrix4x4 const& model, QVector3D const& lightpos);
	void renderRings(QVector3D const& pos, QVector3D const& lightpos);
	void renderRings(QMatrix4x4 const& model, QVector3D const& lightpos);
	~Planet();

  private:
	static void envMap(GLHandler::ShaderProgram& shader, GLHandler::Mesh& mesh,
	                   GLHandler::RenderTarget& renderTarget);

	bool valid = false;
	float radius;

	bool normal = false;

	GLHandler::ShaderProgram shader = {};
	GLHandler::Mesh mesh            = {};
	GLHandler::RenderTarget cubemapDiffuse;
	GLHandler::RenderTarget cubemapNormal;

	bool rings                 = false;
	bool ringTextured          = false;
	GLHandler::Texture ringtex = {};

	GLHandler::ShaderProgram ringShader   = {};
	GLHandler::Mesh ringMesh              = {};
	GLHandler::RenderTarget ringTexTarget = {};
};

#endif // PLANET_H
