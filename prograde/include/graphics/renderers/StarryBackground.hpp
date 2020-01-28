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

#ifndef STARRYBACKGROUND_H
#define STARRYBACKGROUND_H

#include <QFile>
#include <random>

#include "GLHandler.hpp"
#include "graphics/Utils.hpp"
#include "math/Vector3.hpp"
#include "math/constants.hpp"
#include "physics/Color.hpp"
#include "physics/blackbody.hpp"

class StarryBackground
{
  public:
	StarryBackground();
	void initFromRandomUniform();
	void initFromRandomGalactic();
	void initFromFile(float axialTilt = 0.f);
	void render(float exposure, float pixelSolidAngle);
	~StarryBackground();

  private:
	GLHandler::ShaderProgram shader;
	GLHandler::Mesh mesh;
	GLHandler::Texture tex;
	struct Star
	{
		std::string commonName;
		float latitude;
		float longitude;
		float properMotionLatitude;
		float properMotionLongitude;
		float magnitude;
		QColor color;
	};

	void initMesh(std::vector<Star> const& stars, float axialTilt = 0.f);

	static std::vector<Star> loadStars();
	static QColor colorFromColorIndex(float ci);
};

#endif // STARRYBACKGROUND_H
