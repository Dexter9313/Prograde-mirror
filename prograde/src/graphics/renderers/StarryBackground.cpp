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

StarryBackground::StarryBackground() {}

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
		float lum(lumFromMagnitude(mag));
		Color baseColor(colorFromColorIndex(distributionCI(generator)));

		stars.push_back(Star());
		int j = stars.size() - 1;

		stars[j].commonName = "";
		stars[j].latitude
		    = acos(distributionLat(generator)) - (constant::pi / 2.0);
		stars[j].longitude             = distributionLong(generator);
		stars[j].properMotionLatitude  = 0.f;
		stars[j].properMotionLongitude = 0.f;
		stars[j].magnitude             = mag;
		stars[j].color = Color(lum * 255, lum * baseColor.r, lum * baseColor.g,
		                       lum * baseColor.b);
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
		float lum(lumFromMagnitude(mag));
		Color baseColor(colorFromColorIndex(distributionCI(generator)));

		stars.push_back(Star());
		int j = stars.size() - 1;

		stars[j].commonName = "";
		stars[j].latitude
		    = acos(distributionLat(generator)) - (constant::pi / 2.0);
		stars[j].longitude             = distributionLong(generator);
		stars[j].properMotionLatitude  = 0.f;
		stars[j].properMotionLongitude = 0.f;
		stars[j].magnitude             = mag;
		stars[j].color = Color(lum * 255, lum * baseColor.r, lum * baseColor.g,
		                       lum * baseColor.b);
	}

	initMesh(stars);
}

void StarryBackground::initFromFile(float axialTilt)
{
	std::vector<Star> stars(loadStars());
	initMesh(stars, axialTilt);
}

void StarryBackground::render()
{
	GLHandler::beginTransparent();
	GLHandler::setUpRender(shader);
	GLHandler::render(mesh);
	GLHandler::endTransparent();
}

void StarryBackground::initMesh(std::vector<Star> const& stars, float axialTilt)
{
	shader = GLHandler::newShader("colored");

	mesh = GLHandler::newMesh();
	std::vector<float> vboContent;

	Vector3 pos;
	for(Star star : stars)
	{
		pos[0] = cos(star.latitude) * cos(star.longitude);
		pos[1] = -1 * cos(star.latitude) * sin(star.longitude);
		pos[2] = sin(star.latitude);
		pos.rotateAlongX(axialTilt);
		vboContent.push_back(pos[0] * 1000);
		vboContent.push_back(pos[1] * 1000);
		vboContent.push_back(pos[2] * 1000);
		vboContent.push_back(star.color.r / 255.f);
		vboContent.push_back(star.color.g / 255.f);
		vboContent.push_back(star.color.b / 255.f);
		vboContent.push_back(star.color.alpha / 255.f);
	}

	GLHandler::setVertices(mesh, vboContent, shader,
	                       {{"position", 3}, {"color", 4}});
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

			float lum(lumFromMagnitude(mag));
			Color baseColor(colorFromColorIndex(ci));
			result[i].color = Color(lum * 255, lum * baseColor.r,
			                        lum * baseColor.g, lum * baseColor.b);
		}
	}
	return result;
}

float StarryBackground::lumFromMagnitude(float magnitude)
{
	// return magnitude < 3 ? 3 - magnitude : 0;
	// return pow(10, (-1.44f - magnitude) / 9.f);
	return pow(10, (-1.44f - magnitude) / 9.f) * 0.8f;
}

Color StarryBackground::colorFromColorIndex(float ci)
{
	// https://stackoverflow.com/questions/21977786/star-b-v-color-index-to-apparent-rgb-color
	float t
	    = 4600
	      * ((1.f / ((0.92f * ci) + 1.7f)) + (1.f / ((0.92f * ci) + 0.62f)));

	float x = 0.f, y = 0.f, X, Y, Z, r, g, b;

	if(t >= 1667 && t <= 4000)
	{
		x = ((-0.2661239 * pow(10, 9)) / (t * t * t))
		    + ((-0.2343580 * pow(10, 6)) / (t * t))
		    + ((0.8776956 * pow(10, 3)) / t) + 0.179910;
	}
	else if(t > 4000 && t <= 25000)
	{
		x = ((-3.0258469 * pow(10, 9)) / (t * t * t))
		    + ((2.1070379 * pow(10, 6)) / (t * t))
		    + ((0.2226347 * pow(10, 3)) / t) + 0.240390;
	}

	if(t >= 1667 && t <= 2222)
	{
		y = -1.1063814 * x * x * x - 1.34811020 * x * x + 2.18555832 * x
		    - 0.20219683;
	}
	else if(t > 2222 && t <= 4000)
	{
		y = -0.9549476 * x * x * x - 1.37418593 * x * x + 2.09137015 * x
		    - 0.16748867;
	}
	else if(t > 4000 && t <= 25000)
	{
		y = 3.0817580 * x * x * x - 5.87338670 * x * x + 3.75112997 * x
		    - 0.37001483;
	}

	Y = (y == 0.f) ? 0.f : 1.f;
	X = (y == 0.f) ? 0.f : x / y;
	Z = (y == 0.f) ? 0.f : (1.f - x - y) / y;

	r = 3.240479 * X - 1.537150 * Y - 0.498535 * Z;
	g = -0.969256 * X + 1.875992 * Y + 0.041556 * Z;
	b = 0.055648 * X - 0.204043 * Y + 1.057311 * Z;

	return Color(255, 255 * r, 255 * g, 255 * b);
}

StarryBackground::~StarryBackground()
{
	GLHandler::deleteShader(shader);
	GLHandler::deleteMesh(mesh);
}
