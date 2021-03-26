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

#include "graphics/renderers/StarryBackground.hpp"

StarryBackground::StarryBackground()
    : shader("starrybackground")
{
}

// http://mathworld.wolfram.com/SpherePointPicking.html
void StarryBackground::initFromRandomUniform()
{
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distributionLong(0.0,
	                                                        2 * constant::pi);
	std::uniform_real_distribution<double> distributionLat(-1.0, 1.0);
	// close enough distributions
	std::exponential_distribution<double> distributionMag(1);
	std::uniform_real_distribution<double> distributionCI(-0.33, 1.4);

	std::vector<Star> stars;

	for(unsigned int i(0); i < 120000; ++i)
	{
		float mag(-1.0 * distributionMag(generator) + 10);

		stars.push_back(Star());
		int j = stars.size() - 1;

		stars[j].commonName = "";
		stars[j].latitude
		    = acos(distributionLat(generator)) - (constant::pi / 2.0);
		stars[j].longitude             = distributionLong(generator);
		stars[j].properMotionLatitude  = 0.f;
		stars[j].properMotionLongitude = 0.f;
		stars[j].magnitude             = mag;
		stars[j].color = colorFromColorIndex(distributionCI(generator));
	}

	initMesh(stars);
}

void StarryBackground::initFromRandomGalactic()
{
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distributionLong(0.0,
	                                                        2 * constant::pi);
	std::normal_distribution<double> distributionLat(0, 0.5);
	// close enough distributions
	std::exponential_distribution<double> distributionMag(1);
	std::uniform_real_distribution<double> distributionCI(-0.33, 1.4);

	std::vector<Star> stars;

	for(unsigned int i(0); i < 120000; ++i)
	{
		float mag(-1.0 * distributionMag(generator) + 10);

		stars.push_back(Star());
		int j = stars.size() - 1;

		stars[j].commonName = "";
		stars[j].latitude
		    = acos(distributionLat(generator)) - (constant::pi / 2.0);
		stars[j].longitude             = distributionLong(generator);
		stars[j].properMotionLatitude  = 0.f;
		stars[j].properMotionLongitude = 0.f;
		stars[j].magnitude             = mag;
		stars[j].color = colorFromColorIndex(distributionCI(generator));
	}

	initMesh(stars);
}

void StarryBackground::initFromFile(float axialTilt)
{
	std::vector<Star> stars(loadStars());
	initMesh(stars, axialTilt);
}

void StarryBackground::render(float pixelSolidAngle)
{
	GLHandler::glf().glDisable(GL_MULTISAMPLE);
	GLHandler::beginTransparent(GL_ONE, GL_ONE); // sum points
	shader.setUniform("pixelSolidAngle", pixelSolidAngle);
	GLHandler::setUpRender(shader, QMatrix4x4(),
	                       GLHandler::GeometricSpace::SKYBOX);
	mesh.render();
	GLHandler::endTransparent();
	GLHandler::glf().glEnable(GL_MULTISAMPLE);
}

void StarryBackground::initMesh(std::vector<Star> const& stars, float axialTilt)
{
	std::vector<float> vboContent;

	Vector3 pos;
	for(Star star : stars)
	{
		pos[0] = cos(star.latitude) * cos(star.longitude);
		pos[1] = cos(star.latitude) * sin(star.longitude);
		pos[2] = sin(star.latitude);
		pos.rotateAlongX(-1.f * axialTilt);
		vboContent.push_back(pos[0] * 1000);
		vboContent.push_back(pos[1] * 1000);
		vboContent.push_back(pos[2] * 1000);
		vboContent.push_back(star.magnitude);
		vboContent.push_back(star.color.redF());
		vboContent.push_back(star.color.greenF());
		vboContent.push_back(star.color.blueF());
	}

	mesh.setVertexShaderMapping(shader,
	                            {{"position", 3}, {"mag", 1}, {"color", 3}});
	mesh.setVertices(vboContent);
}

std::vector<StarryBackground::Star> StarryBackground::loadStars()
{
	std::vector<Star> result;

	QFile f("data/prograde/physics/hygdata_v3_min.csv");
	f.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&f);
	while(!in.atEnd())
	{
		QString line        = in.readLine();
		QStringList columns = line.split(',');

		// NAME
		std::string name(columns[0].toStdString());

		// MAGNITUDE
		float mag(columns[1].toFloat());

		// COLOR INDEX
		float ci(columns[2].toFloat());

		// RIGHT ASCENSION RAD
		float rarad(columns[3].toFloat());

		// DECLINATION RAD
		float decrad(columns[4].toFloat());

		// CONSTELLATION
		std::string constellation(columns[5].toStdString());

		if(mag > -10)
		{
			result.push_back(Star());
			int i = result.size() - 1;

			result[i].commonName            = "";
			result[i].latitude              = decrad;
			result[i].longitude             = rarad;
			result[i].properMotionLatitude  = 0.f;
			result[i].properMotionLongitude = 0.f;

			result[i].magnitude = mag;
			result[i].color     = colorFromColorIndex(ci);
		}
	}
	return result;
}

QColor StarryBackground::colorFromColorIndex(float ci)
{
	// https://stackoverflow.com/questions/21977786/star-b-v-color-index-to-apparent-rgb-color
	float t
	    = 4600
	      * ((1.f / ((0.92f * ci) + 1.7f)) + (1.f / ((0.92f * ci) + 0.62f)));

	return GLHandler::sRGBToLinear(
	    Utils::toQt(blackbody::colorFromTemperature(t)));
}
