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

#include "physics/Planet.hpp"

#include "physics/OrbitalSystem.hpp"

Planet::Planet(QJsonObject const& json, OrbitalSystem const& system)
    : CelestialBody(CelestialBody::Type::PLANET, json, system)
{
	parseJSON(json);
}

Planet::Planet(QJsonObject const& json, Orbitable const& parent)
    : CelestialBody(CelestialBody::Type::PLANET, json, parent)
{
	parseJSON(json);
}

Planet::Planet(std::string const name,
               CelestialBody::Parameters const& cbParams,
               Parameters const& planetParams, Orbitable const& parent,
               Orbit* orbit)
    : CelestialBody(CelestialBody::Type::PLANET, name, cbParams, parent, orbit)
    , parameters(planetParams)
{
}

Planet::Planet(std::string const name,
               CelestialBody::Parameters const& cbParams,
               Parameters const& planetParams, OrbitalSystem const& system,
               Orbit* orbit)
    : CelestialBody(CelestialBody::Type::PLANET, name, cbParams, system, orbit)
    , parameters(planetParams)
{
}

Star const* Planet::getHostStar() const
{
	if(getParent() == nullptr)
	{
		return nullptr;
	}

	if(getParent()->getOrbitableType() == Orbitable::Type::STAR)
	{
		return dynamic_cast<Star const*>(getParent());
	}
	return this->getSystem().getAllStarsPointers()[0];
}

QJsonObject Planet::getJSONRepresentation() const
{
	QJsonObject result(CelestialBody::getJSONRepresentation());
	result["type"]       = typeToStr(parameters.type).c_str();
	result["atmosphere"] = parameters.atmosphere;
	result["innerRing"]  = parameters.innerRing;
	result["outerRing"]  = parameters.outerRing;
	return result;
}

std::string Planet::typeToStr(Type type)
{
	switch(type)
	{
		case Type::GAZGIANT:
			return "gazgiant";
		case Type::TERRESTRIAL:
			return "terrestrial";
		default:
			return "generic";
	}
}

Planet::Type Planet::strToType(std::string const& str)
{
	if(str == "gazgiant")
	{
		return Type::GAZGIANT;
	}

	if(str == "terrestrial")
	{
		return Type::TERRESTRIAL;
	}

	return Type::GENERIC;
}

void Planet::parseJSON(QJsonObject const& json)
{
	parameters.type
	    = strToType(json["type"].toString(proceduralTypeStr()).toStdString());
	parameters.atmosphere = json["atmosphere"].toDouble(proceduralAtmosphere());
	parameters.innerRing  = json["innerRing"].toDouble(proceduralInnerRings());
	parameters.outerRing  = json["outerRing"].toDouble(proceduralOuterRings());

	if(!json.contains("color"))
	{
		CelestialBody::parameters.color = proceduralColor();
	}
}

QString Planet::proceduralTypeStr() const
{
	srand(getPseudoRandomSeed());
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return r < 0.333f ? "terrestrial" : "gazgiant";
}

Color Planet::proceduralColor() const
{
	srand(getPseudoRandomSeed());
	rand();
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	Color c1(97, 142, 232), c2(255, 255, 255), c3(216, 202, 157);
	if(parameters.type != Type::GAZGIANT)
	{
		c1 = Color(255, 255, 255);
		c2 = Color(54, 57, 48);
		c3 = Color(231, 125, 17);
	}
	Color result(c1);
	if(r <= 0.6f)
	{
		r /= 0.6f;
		result.r = c1.r * (1.f - r) + c2.r * r;
		result.g = c1.g * (1.f - r) + c2.g * r;
		result.b = c1.b * (1.f - r) + c2.b * r;
	}
	else
	{
		r -= 0.6f;
		r /= 0.4f;
		result.r = c2.r * (1.f - r) + c3.r * r;
		result.g = c2.g * (1.f - r) + c3.g * r;
		result.b = c2.b * (1.f - r) + c3.b * r;
	}
	return result;
}

double Planet::proceduralAtmosphere() const
{
	srand(getPseudoRandomSeed());
	rand();
	rand();
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	return 0.3 * pow(r, 6);
}

double Planet::proceduralOuterRings() const
{
	float probability(0.7f);
	if(parameters.type != Type::GAZGIANT)
	{
		probability = 0.05f;
	}
	srand(getPseudoRandomSeed());
	rand();
	rand();
	rand();
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	if(r > probability)
	{
		return 0.0;
	}
	r /= probability;
	r *= r;
	return getCelestialBodyParameters().radius * (2.0 + (10.0 * r));
}

double Planet::proceduralInnerRings() const
{
	srand(getPseudoRandomSeed());
	rand();
	rand();
	rand();
	rand();
	float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	double outerHeight(proceduralOuterRings()
	                   - getCelestialBodyParameters().radius);
	if(outerHeight < 0.0)
	{
		return 0.0;
	}
	return getCelestialBodyParameters().radius + (1.0 - r * r) * outerHeight;
}
