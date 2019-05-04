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
#include "../../../include/graphics/renderers/CelestialBodyRenderer.hpp"

CelestialBodyRenderer::CelestialBodyRenderer(CelestialBody const* drawnBody,
                                             std::string const& name)
    : drawnBody(drawnBody)
    , planet(1.f)
{
	/*shader = GLHandler::newShader("farcelestialbody");
	GLHandler::setShaderParam(shader, "color",
	                          Utils::toQt(drawnBody->getParameters().color));

	mesh = Primitives::newUnitSphere(shader, 100, 100);*/

	// node = scene->addSphereSceneNode(this->drawnBody->getParameters().radius,
	// 64, 0, -1);
	// node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	// node->setMaterialType(FarCelestialBodyShader::getInstance()->getMaterialType());
	// this->scnMgr->getMeshManipulator()->setVertexColors(mesh->getMesh(),
	// toIrrlicht(this->drawnBody->getParameters().color));
	model = QMatrix4x4();
	model.translate(Utils::toQt(this->drawnBody->getAbsolutePositionAtUT(0)));

	QString diffuse(""), normal("");
	QString str(QString("data/prograde/images/") + name.c_str()
	            + "/diffuse.jpg");
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	str = QString("data/prograde/images/") + name.c_str() + "/diffuse.png";
	if(QFileInfo(str).exists())
	{
		diffuse = str;
	}
	if(diffuse != "")
	{
		str = QString("data/prograde/images/") + name.c_str() + "/normal.jpg";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
		str = QString("data/prograde/images/") + name.c_str() + "/normal.png";
		if(QFileInfo(str).exists())
		{
			normal = str;
		}
	}

	if(normal != "")
	{
		planet.initFromTex(diffuse, normal,
		                   drawnBody->getParameters().atmosphere);
	}
	else if(diffuse != "")
	{
		planet.initFromTex(diffuse);
	}
	else if(drawnBody->getType() == CelestialBody::Type::GAZGIANT)
	{
		planet.initGazGiant(Utils::toQt(drawnBody->getParameters().color));
	}
	else
	{
		planet.initTerrestrial(Utils::toQt(drawnBody->getParameters().color));
	}

	float outerRing(drawnBody->getParameters().outerRing);
	if(drawnBody->getParameters().outerRing != 0.f)
	{
		float innerRing(drawnBody->getParameters().innerRing);
		float radius(drawnBody->getParameters().radius);
		QString rings;
		str = QString("data/prograde/images/") + name.c_str() + "/rings.jpg";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}
		str = QString("data/prograde/images/") + name.c_str() + "/rings.png";
		if(QFileInfo(str).exists())
		{
			rings = str;
		}

		if(rings != "")
		{
			planet.initRing(innerRing / radius, outerRing / radius, rings);
		}
		else
		{
			planet.initRing(drawnBody->getParameters().innerRing / radius,
			                drawnBody->getParameters().outerRing / radius);
		}
	}
}

void CelestialBodyRenderer::updateMesh(UniversalTime uT,
                                       Vector3 const& cameraPos)
{
	model = QMatrix4x4();
	Vector3 camRelPos(drawnBody->getAbsolutePositionAtUT(uT) - cameraPos);
	double camDist(camRelPos.length());
	double scale(centerPosition / camDist);
	model.translate(Utils::toQt(scale * camRelPos));

	double radiusScale(drawnBody->getParameters().radius * scale);
	if(radiusScale / centerPosition < 0.002)
	{
		radiusScale = 0.002 * centerPosition;
	}
	model.scale(radiusScale);

	Vector3 bodyCenter(scale * camRelPos);
	Vector3 centralBodyCenter(-1 * scale * cameraPos);

	lightpos = Utils::toQt(centralBodyCenter - bodyCenter);
	/*
	    GLHandler::setShaderParam(shader, "bodyCenter",
	   Utils::toQt(bodyCenter)); GLHandler::setShaderParam(shader,
	   "centralBodyCenter", Utils::toQt(centralBodyCenter));
	*/
}

void CelestialBodyRenderer::render()
{
	/*GLHandler::setUpRender(shader, model);
	GLHandler::render(mesh);*/
	planet.renderPlanet(model, lightpos);
	planet.renderRings(model, lightpos);
}

CelestialBodyRenderer::~CelestialBodyRenderer()
{
	/*GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);*/
}
