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

OrbitalSystemRenderer::OrbitalSystemRenderer(OrbitalSystem const* drawnSystem)
    : drawnSystem(drawnSystem), billboard("data/prograde/images/star.png")
{
	/*shader = GLHandler::newShader("default");
	GLHandler::setShaderParam(shader, "color", QColor(255, 255, 255, 255));

	mesh = Primitives::newUnitSphere(shader, 100, 100);*/

	billboardOriginalEdgeSize = drawnSystem->getCentralRadius() * 512.0 / 30.0;

	for(CelestialBody* body :
	    this->drawnSystem->getAllCelestialBodiesPointers())
	{
		bodyRenderers.push_back(new CelestialBodyRenderer(body));
	}
}

void OrbitalSystemRenderer::updateMesh(UniversalTime uT,
                                       Vector3 const& cameraPos)
{
	camDist = cameraPos.length();

	sortedRenderers.clear();
	for(CelestialBodyRenderer* bodyRenderer : bodyRenderers)
	{
		sortedRenderers[(bodyRenderer->getDrawnBody()->getAbsolutePositionAtUT(
		                     uT)
		                 - cameraPos)
		                    .length()]
		    = bodyRenderer;
		bodyRenderer->updateMesh(uT, cameraPos);
	}
	bool centralBodyDrawn(false);

	float centerPosition(100);
	float increment(250 / (sortedRenderers.size() + 1));

	for(std::pair<double, CelestialBodyRenderer*> rendererPair :
	    sortedRenderers)
	{
		if(rendererPair.first > camDist && !centralBodyDrawn)
		{
			/*model = QMatrix4x4();
			double camDist(cameraPos.length());
			double scale(centerPosition / camDist);
			model.translate(Utils::toQt(-1 * scale * cameraPos));

			double radiusScale(drawnSystem->getCentralRadius() * scale);
			if(radiusScale / centerPosition < 0.002)
			{
				radiusScale = 0.002 * centerPosition;
			}
			model.scale(radiusScale);*/


			double scale(centerPosition / camDist);
			billboard.position = Utils::toQt(-1 * scale * cameraPos);
			if((scale * drawnSystem->getCentralRadius()) / centerPosition
			   < 0.0007)
			{
			    scale
			        = 0.0007 * centerPosition / drawnSystem->getCentralRadius();
			}
			billboard.width = billboardOriginalEdgeSize * scale;

			centralBodyDrawn = true;
			centerPosition += increment;
		}
		rendererPair.second->setCenterPosition(centerPosition);
		rendererPair.second->updateMesh(uT, cameraPos);
		centerPosition += increment;
	}
}

void OrbitalSystemRenderer::render(BasicCamera const& camera)
{
	billboard.render(camera);
	for(auto bodyRenderer : bodyRenderers)
	{
		bodyRenderer->render();
	}
}

OrbitalSystemRenderer::~OrbitalSystemRenderer()
{
	for(CelestialBodyRenderer* bodyRenderer : bodyRenderers)
	{
		delete bodyRenderer;
	}
}
