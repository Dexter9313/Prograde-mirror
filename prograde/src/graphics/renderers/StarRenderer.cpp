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

#include "graphics/renderers/StarRenderer.hpp"

StarRenderer::StarRenderer(Star const* drawnStar)
    : drawnStar(drawnStar)
    , billboard("data/prograde/images/star.png")
    , pointShader(GLHandler::newShader("colored"))
    , pointMesh(GLHandler::newMesh())
{
	billboardOriginalEdgeSize
	    = drawnStar->getParameters().radius * 512.0 / 30.0;

	// POINT
	GLHandler::setVertices(pointMesh, {0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f},
	                       pointShader, {{"position", 3}, {"color", 4}});
}

void StarRenderer::updateMesh(UniversalTime /* uT */, Camera const& camera)
{
	camDist = camera.getAbsolutePosition().length();
	float centerPosition(300.f);
	double scale(centerPosition / camDist);
	pointPos           = Utils::toQt(-1 * scale * camera.getAbsolutePosition());
	billboard.position = pointPos;
	billboard.width    = billboardOriginalEdgeSize * scale;
}

void StarRenderer::render(BasicCamera const& camera)
{
	if(drawnStar->getParameters().radius / camDist < 0.0007)
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
}

StarRenderer::~StarRenderer()
{
	GLHandler::deleteMesh(pointMesh);
	GLHandler::deleteShader(pointShader);
}
