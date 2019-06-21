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
	parameters.type       = strToType(json["type"].toString().toStdString());
	parameters.atmosphere = json["atmosphere"].toDouble();
	parameters.innerRing  = json["innerRing"].toDouble();
	parameters.outerRing  = json["outerRing"].toDouble();
}
