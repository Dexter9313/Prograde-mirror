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

#include "../../include/physics/CelestialBody.hpp"

CelestialBody::CelestialBody(Type type, QJsonObject const& json,
                             OrbitalSystem const& system)
    : Orbitable(orbitableType(type), json, system)
    , type(type)
{
	parseJSON(json);
}

CelestialBody::CelestialBody(Type type, QJsonObject const& json,
                             Orbitable const& parent)
    : Orbitable(orbitableType(type), json, parent)
    , type(type)
{
	parseJSON(json);
}

CelestialBody::CelestialBody(Type type, std::string const name,
                             Parameters const& parameters,
                             OrbitalSystem const& system, Orbit* orbit)
    : Orbitable(orbitableType(type), name, system, orbit)
    , parameters(std::move(parameters))
    , type(type)
{
}

CelestialBody::CelestialBody(Type type, std::string const name,
                             Parameters const& parameters,
                             Orbitable const& parent, Orbit* orbit)
    : Orbitable(orbitableType(type), name, parent, orbit)
    , parameters(std::move(parameters))
    , type(type)
{
}

double CelestialBody::getPrimeMeridianSiderealTimeAtUT(UniversalTime uT) const
{
	operator%=<UniversalTime>(uT, parameters.siderealRotationPeriod);

	return parameters.siderealTimeAtEpoch
	       + 2.0 * constant::pi * static_cast<double>(uT)
	             / parameters.siderealRotationPeriod;
}

QJsonObject CelestialBody::getJSONRepresentation() const
{
	QJsonObject result = Orbitable::getJSONRepresentation();

	result["mass"]                   = parameters.mass;
	result["radius"]                 = parameters.radius;
	result["oblateness"]             = vector3ToJSON(parameters.oblateness);
	result["color"]                  = colorToJSON(parameters.color);
	result["siderealTimeAtEpoch"]    = parameters.siderealTimeAtEpoch;
	result["siderealRotationPeriod"] = parameters.siderealRotationPeriod;
	result["northPoleRightAsc"]      = parameters.northPoleRightAsc;
	result["northPoleDeclination"]   = parameters.northPoleDeclination;

	return result;
}

QJsonObject CelestialBody::vector3ToJSON(Vector3 const& v)
{
	return QJsonObject({{"x", v[0]}, {"y", v[1]}, {"z", v[2]}});
}

Vector3 CelestialBody::jsonToVector3(QJsonObject const& obj,
                                     Vector3 const& defaultValue)
{
	return Vector3(obj["x"].toDouble(defaultValue[0]),
	               obj["y"].toDouble(defaultValue[1]),
	               obj["z"].toDouble(defaultValue[2]));
}

QJsonObject CelestialBody::colorToJSON(Color const& c)
{
	return QJsonObject({{"r", static_cast<int>(c.r)},
	                    {"g", static_cast<int>(c.g)},
	                    {"b", static_cast<int>(c.b)},
	                    {"alpha", static_cast<int>(c.alpha)}});
}

Color CelestialBody::jsonToColor(QJsonObject const& obj,
                                 Color const& defaultValue)
{
	return Color(obj["alpha"].toInt(defaultValue.alpha),
	             obj["r"].toInt(defaultValue.r), obj["g"].toInt(defaultValue.g),
	             obj["b"].toInt(defaultValue.b));
}

void CelestialBody::parseJSON(QJsonObject const& json)
{
	parameters.mass   = json["mass"].toDouble();
	parameters.radius = json["radius"].toDouble(70000000.0);
	parameters.oblateness
	    = jsonToVector3(json["oblateness"].toObject(), Vector3(1.0, 1.0, 1.0));
	parameters.color
	    = jsonToColor(json["color"].toObject(), Color(255, 255, 255, 255));
	parameters.siderealTimeAtEpoch = json["siderealTimeAtEpoch"].toDouble();
	parameters.siderealRotationPeriod
	    = json["siderealRotationPeriod"].toDouble(DBL_MAX);
	parameters.northPoleRightAsc = json["northPoleRightAsc"].toDouble();
	parameters.northPoleDeclination
	    = json["northPoleDeclination"].toDouble(1.5707963705062866);
}

Orbitable::Type CelestialBody::orbitableType(Type celestialBodyType)
{
	return celestialBodyType == Type::STAR ? Orbitable::Type::STAR
	                                       : Orbitable::Type::PLANET;
}
