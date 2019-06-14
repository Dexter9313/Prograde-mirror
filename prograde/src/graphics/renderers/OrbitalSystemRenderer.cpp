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
    , starRenderer(drawnSystem->getStar())
{
	for(std::string name : drawnSystem->getAllCelestialBodiesNames())
	{
		bodyRenderers.push_back(new CelestialBodyRenderer(
		    (*drawnSystem)[name], drawnSystem->getStar(),
		    drawnSystem->getDeclinationTilt()));
	}
}

void OrbitalSystemRenderer::updateMesh(UniversalTime uT, Camera const& camera)
{
	starRenderer.updateMesh(uT, camera);

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
	for(std::pair<double, CelestialBodyRenderer*> rendererPair :
	    sortedRenderers)
	{
		rendererPair.second->setCenterPosition(centerPosition);
		rendererPair.second->updateMesh(uT, camera);
	}
}

void OrbitalSystemRenderer::render(BasicCamera const& camera)
{
	starRenderer.render(camera);

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
}
