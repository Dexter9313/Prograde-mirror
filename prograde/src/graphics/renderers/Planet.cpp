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

#include "graphics/renderers/Planet.hpp"

Planet::Planet(float radius)
    : radius(radius)
{
	cubemapDiffuse = GLHandler::newRenderTarget(2048, 2048, GL_RGBA16F, true);
}

void Planet::initTerrestrial(QColor const& color, float polarLatitude,
                             float atmosphere)
{
	valid  = true;
	shader = GLHandler::newShader("planet/planet", "planet/diffnormplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);
	normal = true;
	cubemapNormal = GLHandler::newRenderTarget(2048, 2048, GL_RGBA16F, true);

	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "norm", 1);
	GLHandler::setShaderParam(shader, "atmosphere", atmosphere);

	// NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	float seed = 10000.f * static_cast<float>(rand()) / INT_MAX;

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/terrestrialdiff");
	GLHandler::setShaderParam(sdiff, "color", color);
	GLHandler::setShaderParam(sdiff, "polarLatitude", polarLatitude);
	GLHandler::setShaderParam(sdiff, "seed", seed);
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);

	GLHandler::ShaderProgram snorm
	    = GLHandler::newShader("planet/planet", "planet/terrestrialnorm");
	GLHandler::setShaderParam(snorm, "color", color);
	GLHandler::setShaderParam(snorm, "polarLatitude", polarLatitude);
	GLHandler::setShaderParam(snorm, "seed", seed);
	GLHandler::Mesh mnorm = Primitives::newUnitSphere(snorm, 50, 50);

	envMap(snorm, mnorm, cubemapNormal);

	GLHandler::deleteMesh(mnorm);
	GLHandler::deleteShader(snorm);
}

void Planet::initGazGiant(QColor const& color, float bandsIntensity,
                          float stormsIntensity)
{
	valid  = true;
	shader = GLHandler::newShader("planet/planet", "planet/diffplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	// NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	float seed = 10000.f * static_cast<float>(rand()) / INT_MAX;

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/gazgiantdiff");
	GLHandler::setShaderParam(sdiff, "color", color);
	GLHandler::setShaderParam(sdiff, "bandsIntensity", bandsIntensity);
	GLHandler::setShaderParam(sdiff, "stormsIntensity", stormsIntensity);
	GLHandler::setShaderParam(sdiff, "seed", seed);
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);
}

void Planet::initFromTex(QString const& diffusePath)
{
	valid  = true;
	shader = GLHandler::newShader("planet/planet", "planet/diffplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/difftocube");
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	GLHandler::Texture tdiff
	    = GLHandler::newTexture(diffusePath.toLatin1().data());

	GLHandler::useTextures({tdiff});
	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteTexture(tdiff);
	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);
}

void Planet::initFromTex(QString const& diffusePath, QString const& normalPath,
                         float atmosphere)
{
	valid  = true;
	shader = GLHandler::newShader("planet/planet", "planet/diffnormplanet");
	mesh   = Primitives::newUnitSphere(shader, 50, 50);
	normal = true;
	cubemapNormal = GLHandler::newRenderTarget(2048, 2048, GL_RGBA16F, true);

	GLHandler::setShaderParam(shader, "diff", 0);
	GLHandler::setShaderParam(shader, "norm", 1);
	GLHandler::setShaderParam(shader, "atmosphere", atmosphere);

	GLHandler::ShaderProgram sdiff
	    = GLHandler::newShader("planet/planet", "planet/difftocube");
	GLHandler::Mesh mdiff = Primitives::newUnitSphere(sdiff, 50, 50);

	GLHandler::Texture tdiff
	    = GLHandler::newTexture(diffusePath.toLatin1().data());

	GLHandler::useTextures({tdiff});
	envMap(sdiff, mdiff, cubemapDiffuse);

	GLHandler::deleteTexture(tdiff);
	GLHandler::deleteMesh(mdiff);
	GLHandler::deleteShader(sdiff);

	GLHandler::ShaderProgram snorm
	    = GLHandler::newShader("planet/planet", "planet/normtocube");
	GLHandler::Mesh mnorm = Primitives::newUnitSphere(snorm, 50, 50);

	GLHandler::Texture tnorm
	    = GLHandler::newTexture(normalPath.toLatin1().data(), false);

	GLHandler::useTextures({tnorm});
	envMap(snorm, mnorm, cubemapNormal);

	GLHandler::deleteTexture(tnorm);
	GLHandler::deleteMesh(mnorm);
	GLHandler::deleteShader(snorm);
}

void Planet::initRing(float innerRing, float outerRing,
                      QString const& texturePath)
{
	rings      = true;
	ringShader = GLHandler::newShader("planet/ring");
	ringMesh   = GLHandler::newMesh();

	GLHandler::setShaderParam(ringShader, "inner", innerRing);
	GLHandler::setShaderParam(ringShader, "outer", outerRing);
	GLHandler::setShaderParam(ringShader, "planetradius", radius);

	float coeff(1.f / cos(3.1415 / 20.0));

	std::vector<float> ringVertices;
	std::vector<unsigned int> ringElements;
	for(unsigned int i(0); i < 20; ++i)
	{
		// 2 * i
		ringVertices.push_back(innerRing * cos(i * 3.1415f / 10.f));
		ringVertices.push_back(innerRing * sin(i * 3.1415f / 10.f));

		// 2 * i + 1
		ringVertices.push_back(outerRing * coeff * cos(i * 3.1415f / 10.f));
		ringVertices.push_back(outerRing * coeff * sin(i * 3.1415f / 10.f));

		if(i != 0)
		{
			ringElements.push_back(2 * (i - 1));
			ringElements.push_back(2 * (i - 1) + 1);
			ringElements.push_back(2 * i + 1);

			ringElements.push_back(2 * (i - 1));
			ringElements.push_back(2 * i + 1);
			ringElements.push_back(2 * i);
		}
	}
	ringElements.push_back(2 * (20 - 1));
	ringElements.push_back(2 * (20 - 1) + 1);
	ringElements.push_back(1);

	ringElements.push_back(2 * (20 - 1));
	ringElements.push_back(1);
	ringElements.push_back(0);

	GLHandler::setVertices(ringMesh, ringVertices, ringShader,
	                       {{"position", 2}}, ringElements);

	if(texturePath == "")
	{
		ringTexTarget = GLHandler::newRenderTarget(16000, 1);
		updateRing();
	}
	else
	{
		ringTextured = true;
		ringtex      = GLHandler::newTexture(texturePath.toLatin1().data());
	}
}

void Planet::updateRing()
{
	if(ringTextured)
	{
		return;
	}

	GLHandler::ShaderProgram shader = GLHandler::newShader("planet/ringtex");
	GLHandler::Mesh mesh            = GLHandler::newMesh();
	GLHandler::setVertices(mesh, {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f},
	                       shader, {{"position", 2}});

	GLHandler::setShaderParam(shader, "color", QColor(210, 180, 140));
	GLHandler::setShaderParam(
	    shader, "seed",
	    // NOLINTNEXTLINE(cert-msc30-c, cert-msc50-c, cert-msc50-cpp)
	    10000.f * static_cast<float>(rand()) / INT_MAX);

	GLHandler::beginRendering(ringTexTarget);
	GLHandler::useShader(shader);
	GLHandler::render(mesh, GLHandler::PrimitiveType::TRIANGLE_STRIP);

	GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);
}

void Planet::renderPlanet(QVector3D const& pos, QVector3D const& lightpos)
{
	QMatrix4x4 model;

	model.translate(pos);
	model.scale(radius);

	renderPlanet(model, lightpos);
}

void Planet::renderPlanet(QMatrix4x4 const& model, QVector3D const& lightpos)
{
	GLHandler::setShaderParam(shader, "lightpos", lightpos);

	if(!normal)
	{
		GLHandler::useTextures(
		    {GLHandler::getColorAttachmentTexture(cubemapDiffuse)});
	}
	else
	{
		GLHandler::useTextures(
		    {GLHandler::getColorAttachmentTexture(cubemapDiffuse),
		     GLHandler::getColorAttachmentTexture(cubemapNormal)});
	}
	GLHandler::setUpRender(shader, model);
	GLHandler::render(mesh);
}

void Planet::renderRings(QVector3D const& pos, QVector3D const& lightpos)
{
	if(!rings)
	{
		return;
	}
	QMatrix4x4 ringsModel;
	ringsModel.translate(pos);

	renderRings(ringsModel, lightpos);
}

void Planet::renderRings(QMatrix4x4 const& model, QVector3D const& lightpos)
{
	if(!rings)
	{
		return;
	}

	GLHandler::setShaderParam(ringShader, "lightpos", lightpos);
	GLHandler::beginTransparent();
	GLHandler::setUpRender(ringShader, model);
	if(!ringTextured)
	{
		GLHandler::useTextures(
		    {GLHandler::getColorAttachmentTexture(ringTexTarget)});
	}
	else
	{
		GLHandler::useTextures({ringtex});
	}
	GLHandler::render(ringMesh);
	GLHandler::endTransparent();
}

void Planet::envMap(GLHandler::ShaderProgram& shader, GLHandler::Mesh& mesh,
                    GLHandler::RenderTarget& renderTarget)
{
	QMatrix4x4 perspective;
	perspective.perspective(90.f, 1.f, 0.1f, 10.f);

	std::vector<QVector3D> vecs = {
	    QVector3D(1, 0, 0),  QVector3D(0, -1, 0), QVector3D(-1, 0, 0),
	    QVector3D(0, -1, 0), QVector3D(0, 1, 0),  QVector3D(0, 0, 1),
	    QVector3D(0, -1, 0), QVector3D(0, 0, -1), QVector3D(0, 0, -1),
	    QVector3D(0, -1, 0), QVector3D(0, 0, 1),  QVector3D(0, -1, 0),
	};

	std::vector<GLHandler::CubeFace> faces = {
	    GLHandler::CubeFace::FRONT,  GLHandler::CubeFace::BACK,
	    GLHandler::CubeFace::LEFT,   GLHandler::CubeFace::RIGHT,
	    GLHandler::CubeFace::BOTTOM, GLHandler::CubeFace::TOP,
	};

	for(unsigned int i(0); i < 6; ++i)
	{
		QMatrix4x4 cubeCamera;
		cubeCamera.lookAt(QVector3D(0, 0, 0), vecs[2 * i], vecs[(2 * i) + 1]);
		QMatrix4x4 c = perspective * cubeCamera;
		GLHandler::setUpTransforms(c, c, c, c, c);
		GLHandler::beginRendering(renderTarget, faces[i]);
		GLHandler::setUpRender(shader);
		GLHandler::render(mesh);
	}
}

Planet::~Planet()
{
	GLHandler::deleteRenderTarget(cubemapDiffuse);
	if(!valid)
	{
		return;
	}

	if(normal)
	{
		GLHandler::deleteRenderTarget(cubemapNormal);
	}

	GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);

	if(rings)
	{
		GLHandler::deleteMesh(ringMesh);
		GLHandler::deleteShader(ringShader);
		if(!ringTextured)
		{
			GLHandler::deleteRenderTarget(ringTexTarget);
		}
	}
}
