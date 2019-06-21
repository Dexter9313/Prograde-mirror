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
{
	for(auto body : drawnSystem->getAllPlanetsPointers())
	{
		bodyRenderers.push_back(new PlanetRenderer(body));
	}
	for(auto star : drawnSystem->getAllStarsPointers())
	{
		bodyRenderers.push_back(new StarRenderer(star));
	}
}

void OrbitalSystemRenderer::updateMesh(UniversalTime uT, Camera const& camera)
{
	sortedRenderers.clear();
	for(auto bodyRenderer : bodyRenderers)
	{
		sortedRenderers[camera
		                    .getRelativePositionTo(bodyRenderer->getDrawnBody(),
		                                           uT)
		                    .length()]
		    = bodyRenderer;
		bodyRenderer->updateMesh(uT, camera);
	}

	for(auto rendererPair : sortedRenderers)
	{
		rendererPair.second->updateMesh(uT, camera);
	}
}

void OrbitalSystemRenderer::render(BasicCamera const& camera)
{
	auto it(sortedRenderers.end());
	while(it != sortedRenderers.begin())
	{
		--it;
		GLHandler::clearDepthBuffer();
		it->second->render(camera);
	}
}

OrbitalSystemRenderer::~OrbitalSystemRenderer()
{
	for(auto bodyRenderer : bodyRenderers)
	{
		delete bodyRenderer;
	}
}
