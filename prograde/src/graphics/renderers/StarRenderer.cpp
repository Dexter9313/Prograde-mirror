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
    : CelestialBodyRenderer(drawnStar,
                            GLHandler::sRGBToLinear(Utils::toQt(
                                drawnStar->getCelestialBodyParameters().color)))
    , billboard(getBillboardImage(drawnStar))
{
	billboardOriginalEdgeSize
	    = drawnStar->getCelestialBodyParameters().radius * 39.253; // eyeballed
}

void StarRenderer::updateMesh(UniversalTime uT, Camera const& camera)
{
	CelestialBodyRenderer::updateMesh(uT, camera);
	billboard.position = position;
	billboard.width    = billboardOriginalEdgeSize * scale;
}

void StarRenderer::render(BasicCamera const& camera)
{
	double camDist(camRelPos.length());
	if(drawnBody->getCelestialBodyParameters().radius / camDist < 0.0007)
	{
		CelestialBodyRenderer::render(camera);
	}
	else
	{
		billboard.render(camera);
	}
}

QImage StarRenderer::getBillboardImage(Star const* star)
{
	QColor starCol(Utils::toQt(star->getCelestialBodyParameters().color));
	QImage img("data/prograde/images/star.png");
	for(int i(0); i < img.height(); ++i)
	{
		for(int j(0); j < img.width(); ++j)
		{
			QRgb colRGB(img.pixel(i, j));
			QColor col(colRGB);
			col.setAlpha(colRGB >> 24);
			col.setRedF(col.redF() * starCol.redF());
			col.setGreenF(col.greenF() * starCol.greenF());
			col.setBlueF(col.blueF() * starCol.blueF());
			img.setPixel(i, j, col.rgba());
		}
	}
	return img;
}
