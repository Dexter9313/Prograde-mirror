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

CelestialBodyRenderer::CelestialBodyRenderer(CelestialBody const* drawnBody)
    : drawnBody(drawnBody)
{
	shader = GLHandler::newShader("farcelestialbody");
	GLHandler::setShaderParam(shader, "color",
	                          Utils::toQt(drawnBody->getParameters().color));

	mesh = Primitives::newUnitSphere(shader, 100, 100);

	// node = scene->addSphereSceneNode(this->drawnBody->getParameters().radius,
	// 64, 0, -1);
	// node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	// node->setMaterialType(FarCelestialBodyShader::getInstance()->getMaterialType());
	// this->scnMgr->getMeshManipulator()->setVertexColors(mesh->getMesh(),
	// toIrrlicht(this->drawnBody->getParameters().color));
	model = QMatrix4x4();
	model.translate(Utils::toQt(this->drawnBody->getAbsolutePositionAtUT(0)));
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

	GLHandler::setShaderParam(shader, "bodyCenter", Utils::toQt(bodyCenter));
	GLHandler::setShaderParam(shader, "centralBodyCenter",
	                          Utils::toQt(centralBodyCenter));
}

void CelestialBodyRenderer::render()
{
	GLHandler::setUpRender(shader, model);
	GLHandler::render(mesh);
}

CelestialBodyRenderer::~CelestialBodyRenderer()
{
	GLHandler::deleteMesh(mesh);
	GLHandler::deleteShader(shader);
}
