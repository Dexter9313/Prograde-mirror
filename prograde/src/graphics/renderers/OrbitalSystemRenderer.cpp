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
#include "graphics/renderers/OrbitalSystemRenderer.hpp"

OrbitalSystemRenderer::OrbitalSystemRenderer(OrbitalSystem* drawnSystem)
    : drawnSystem(drawnSystem)
    , pointShader(GLHandler::newShader("colored"))
    , pointMesh(GLHandler::newMesh())
    , billboard("data/prograde/images/star.png")
{
	billboardOriginalEdgeSize = drawnSystem->getCentralRadius() * 512.0 / 30.0;

	for(std::string name : drawnSystem->getAllCelestialBodiesNames())
	{
		bodyRenderers.push_back(new CelestialBodyRenderer(
		    (*drawnSystem)[name], drawnSystem->getCentralRadius(),
		    drawnSystem->getDeclinationTilt()));
	}
	// POINT
	GLHandler::setVertices(pointMesh, {0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f},
	                       pointShader, {{"position", 3}, {"color", 4}});
}

void OrbitalSystemRenderer::updateMesh(UniversalTime uT, Camera const& camera)
{
	camDist = camera.getAbsolutePosition().length();

	sortedRenderers.clear();
	for(CelestialBodyRenderer* bodyRenderer : bodyRenderers)
	{
		sortedRenderers[camera
		                    .getRelativePositionTo(bodyRenderer->getDrawnBody(),
		                                           uT)
		                    .length()]
		    = bodyRenderer;
		bodyRenderer->updateMesh(uT, camera);
	}

	float centerPosition(300.f);

	double scale(centerPosition / camDist);
	pointPos           = Utils::toQt(-1 * scale * camera.getAbsolutePosition());
	billboard.position = pointPos;
	billboard.width    = billboardOriginalEdgeSize * scale;

	for(std::pair<double, CelestialBodyRenderer*> rendererPair :
	    sortedRenderers)
	{
		rendererPair.second->setCenterPosition(centerPosition);
		rendererPair.second->updateMesh(uT, camera);
	}
}

void OrbitalSystemRenderer::render(BasicCamera const& camera)
{
	if(drawnSystem->getCentralRadius() / camDist < 0.0007)
	{
		QMatrix4x4 model;
		model.translate(pointPos);
		GLHandler::setUpRender(pointShader, model);
		GLHandler::render(pointMesh);
	}
	else
	{
		billboard.render(camera);
	}
	auto it(sortedRenderers.end());
	while(it != sortedRenderers.begin())
	{
		--it;
		GLHandler::clearDepthBuffer();
		it->second->render();
	}
}

OrbitalSystemRenderer::~OrbitalSystemRenderer()
{
	for(CelestialBodyRenderer* bodyRenderer : bodyRenderers)
	{
		delete bodyRenderer;
	}
	GLHandler::deleteMesh(pointMesh);
	GLHandler::deleteShader(pointShader);
}
