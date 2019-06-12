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

CelestialBody::CelestialBody(QJsonObject const& json,
                             std::string const& ownName,
                             double influentBodyMass)
    : parent(nullptr)
{
	if(json.contains("orbit"))
	{
		orbit = new Orbit(json["orbit"].toObject());
	}
	else
	{
		orbit = new CSVOrbit(Orbit::MassiveBodyMass(influentBodyMass), ownName);
	}
	QJsonObject jp(json["parameters"].toObject());
	parameters.type   = strToType(jp["type"].toString().toStdString());
	parameters.mass   = jp["mass"].toDouble();
	parameters.radius = jp["radius"].toDouble(70000000.0);
	parameters.oblateness
	    = jsonToVector3(jp["oblateness"].toObject(), Vector3(1.0, 1.0, 1.0));
	parameters.color
	    = jsonToColor(jp["color"].toObject(), Color(255, 255, 255, 255));
	parameters.atmosphere          = jp["atmosphere"].toDouble();
	parameters.innerRing           = jp["innerRing"].toDouble();
	parameters.outerRing           = jp["outerRing"].toDouble();
	parameters.siderealTimeAtEpoch = jp["siderealTimeAtEpoch"].toDouble();
	parameters.siderealRotationPeriod
	    = jp["siderealRotationPeriod"].toDouble(DBL_MAX);
	parameters.northPoleRightAsc = jp["northPoleRightAsc"].toDouble();
	parameters.northPoleDeclination
	    = jp["northPoleDeclination"].toDouble(1.5707963705062866);
}

CelestialBody::CelestialBody(QJsonObject const& json,
                             std::string const& ownName,
                             CelestialBody const& parent)
    : parent(&parent)
{
	if(json.contains("orbit"))
	{
		orbit = new Orbit(json["orbit"].toObject());
	}
	else
	{
		orbit = new CSVOrbit(
		    Orbit::MassiveBodyMass(parent.getParameters().mass), ownName);
	}
	QJsonObject jp(json["parameters"].toObject());
	parameters.type   = strToType(jp["type"].toString().toStdString());
	parameters.mass   = jp["mass"].toDouble();
	parameters.radius = jp["radius"].toDouble(70000000.0);
	parameters.oblateness
	    = jsonToVector3(jp["oblateness"].toObject(), Vector3(1.0, 1.0, 1.0));
	parameters.color
	    = jsonToColor(jp["color"].toObject(), Color(255, 255, 255, 255));
	parameters.atmosphere          = jp["atmosphere"].toDouble();
	parameters.innerRing           = jp["innerRing"].toDouble();
	parameters.outerRing           = jp["outerRing"].toDouble();
	parameters.siderealTimeAtEpoch = jp["siderealTimeAtEpoch"].toDouble();
	parameters.siderealRotationPeriod
	    = jp["siderealRotationPeriod"].toDouble(DBL_MAX);
	parameters.northPoleRightAsc = jp["northPoleRightAsc"].toDouble();
	parameters.northPoleDeclination
	    = jp["northPoleDeclination"].toDouble(1.5707963705062866);
}

CelestialBody::CelestialBody(double influentBodyMass,
                             Orbit::Parameters orbitalParams,
                             Parameters physicalParams)
    : parent(nullptr)
    , orbit(new Orbit(Orbit::MassiveBodyMass(influentBodyMass), orbitalParams))
    , parameters(std::move(physicalParams))
{
}

CelestialBody::CelestialBody(CelestialBody const& parent,
                             Orbit::Parameters const& orbitalParams,
                             Parameters physicalParams)
    : parent(&parent)
    , orbit(new Orbit(Orbit::MassiveBodyMass(parent.getParameters().mass),
                      orbitalParams))
    , parameters(std::move(physicalParams))
{
}

CelestialBody::CelestialBody(double influentBodyMass,
                             std::string const& ownName,
                             Parameters physicalParams)
    : parent(nullptr)
    , orbit(new CSVOrbit(Orbit::MassiveBodyMass(influentBodyMass), ownName))
    , parameters(std::move(physicalParams))
{
}

CelestialBody::CelestialBody(CelestialBody const* parent,
                             std::string const& ownName,
                             Parameters physicalParams)
    : parent(parent)
    , orbit(new CSVOrbit(Orbit::MassiveBodyMass(parent->getParameters().mass),
                         ownName))
    , parameters(std::move(physicalParams))
{
}

CelestialBody const* CelestialBody::getParent() const
{
	return parent;
}

std::vector<CelestialBody*> const& CelestialBody::getChildren() const
{
	return children;
}

CelestialBody* CelestialBody::createChild(QJsonObject const& json,
                                          std::string const& childName)
{
	auto newChild = new CelestialBody(json, childName, *this);
	children.push_back(newChild);
	return newChild;
}

CelestialBody*
    CelestialBody::createChild(Orbit::Parameters const& orbitalParams,
                               Parameters const& physicalParams)
{
	auto newChild = new CelestialBody(*this, orbitalParams, physicalParams);
	children.push_back(newChild);
	return newChild;
}

CelestialBody* CelestialBody::createChild(std::string const& childName,
                                          Parameters const& physicalParams)
{
	auto newChild = new CelestialBody(this, childName, physicalParams);
	children.push_back(newChild);
	return newChild;
}

Orbit const* CelestialBody::getOrbit() const
{
	return orbit;
}

Orbit* CelestialBody::getOrbit()
{
	return orbit;
}

CelestialBody::Parameters CelestialBody::getParameters() const
{
	return parameters;
}

Vector3 CelestialBody::getRelativePositionAtUT(UniversalTime uT) const
{
	return orbit->getPositionAtUT(uT);
}

Vector3 CelestialBody::getAbsolutePositionAtUT(UniversalTime uT) const
{
	Vector3 result(orbit->getPositionAtUT(uT));

	if(parent != nullptr)
	{
		// At least works for JPL Horizon Data
		result += parent->getAbsolutePositionAtUT(uT);
		/*result = getAbsolutePositionFromRelative(
		    parent->getAttachedCoordinateSystemAtUT(uT), result);*/
	}

	return result;
}

CoordinateSystem
    CelestialBody::getAttachedCoordinateSystemAtUT(UniversalTime uT) const
{
	return orbit->getRelativeCoordinateSystemAtUT(uT);
}

double CelestialBody::getPrimeMeridianSiderealTimeAtUT(UniversalTime uT) const
{
	operator%=<UniversalTime>(uT, parameters.siderealRotationPeriod);

	return parameters.siderealTimeAtEpoch
	       + 2.0 * constant::pi * static_cast<double>(uT)
	             / parameters.siderealRotationPeriod;
}

CelestialBody::~CelestialBody()
{
	for(CelestialBody* child : children)
	{
		delete child;
	}
	delete orbit;
}

Vector3 CelestialBody::getRelativePositionAtUt(CelestialBody const* from,
                                               CelestialBody const* to,
                                               UniversalTime uT)
{
	if(from == to)
	{
		return Vector3(0.0, 0.0, 0.0);
	}

	// Chain of relative positions from central body to from.
	// The sum of the stored Vector3 should be absolute position
	// of from.
	std::vector<std::pair<CelestialBody const*, Vector3>>
	    fromAncestorsRelativePositions;
	// Chain of relative positions from central body to to.
	// The sum of the stored Vector3 should be absolute position
	// of to.
	std::vector<std::pair<CelestialBody const*, Vector3>>
	    toAncestorsRelativePositions;

	// construct fromAncestorsRelativePositions
	fromAncestorsRelativePositions.push_back(
	    {from, from->getRelativePositionAtUT(uT)});
	CelestialBody const* currentBody(from);
	CelestialBody const* parent(currentBody->getParent());
	while(parent != nullptr)
	{
		fromAncestorsRelativePositions.push_back(
		    {parent, currentBody->getRelativePositionAtUT(uT)});
		currentBody = parent;
		parent      = currentBody->getParent();
	}

	// construct toAncestorsRelativePositions
	toAncestorsRelativePositions.push_back(
	    {to, to->getRelativePositionAtUT(uT)});
	currentBody = to;
	parent      = currentBody->getParent();
	while(parent != nullptr)
	{
		toAncestorsRelativePositions.push_back(
		    {parent, currentBody->getRelativePositionAtUT(uT)});
		currentBody = parent;
		parent      = currentBody->getParent();
	}

	// go up the tree from lowest node to start from both sides at the same
	// depth
	unsigned int i(0), j(0);
	if(fromAncestorsRelativePositions.size()
	   > toAncestorsRelativePositions.size())
	{
		i = fromAncestorsRelativePositions.size()
		    - toAncestorsRelativePositions.size();
	}
	else
	{
		j = toAncestorsRelativePositions.size()
		    - fromAncestorsRelativePositions.size();
	}

	CelestialBody const* commonAncestor(nullptr);
	for(; i < fromAncestorsRelativePositions.size(); ++i, ++j)
	{
		if(fromAncestorsRelativePositions[i].first
		   == toAncestorsRelativePositions[j].first)
		{
			commonAncestor = fromAncestorsRelativePositions[i].first;
		}
	}

	// now we are in the reference frame of the common ancestor
	// "absolute" is in this reference frame
	Vector3 fromAbsolute(0.0, 0.0, 0.0), toAbsolute(0.0, 0.0, 0.0);
	// gain some computation but the algorithm would world without this check
	if(commonAncestor == nullptr)
	{
		fromAbsolute = from->getAbsolutePositionAtUT(uT);
		toAbsolute   = to->getAbsolutePositionAtUT(uT);
	}
	else
	{
		for(unsigned int k(0);
		    fromAncestorsRelativePositions[k].first != commonAncestor
		    // add if commonAncestor can be nullptr
		    // && i < fromAncestorsRelativePositions.size()
		    ;
		    ++k)
		{
			fromAbsolute += fromAncestorsRelativePositions[k].second;
		}

		for(unsigned int k(0);
		    toAncestorsRelativePositions[k].first != commonAncestor
		    // add if commonAncestor can be nullptr
		    // && i < toAncestorsRelativePositions.size()
		    ;
		    ++k)
		{
			toAbsolute += toAncestorsRelativePositions[k].second;
		}
	}

	return toAbsolute - fromAbsolute;
}

QJsonObject CelestialBody::getJSONRepresentation() const
{
	QJsonObject result;

	if(!orbit->isLoadedFromFile())
	{
		result["orbit"] = orbit->getJSONRepresentation();
	}

	QJsonObject parametersResult;
	parametersResult["type"]       = typeToStr(parameters.type).c_str();
	parametersResult["mass"]       = parameters.mass;
	parametersResult["radius"]     = parameters.radius;
	parametersResult["oblateness"] = vector3ToJSON(parameters.oblateness);
	parametersResult["color"]      = colorToJSON(parameters.color);
	parametersResult["atmosphere"] = parameters.atmosphere;
	parametersResult["innerRing"]  = parameters.innerRing;
	parametersResult["outerRing"]  = parameters.outerRing;
	parametersResult["siderealTimeAtEpoch"] = parameters.siderealTimeAtEpoch;
	parametersResult["siderealRotationPeriod"]
	    = parameters.siderealRotationPeriod;
	parametersResult["northPoleRightAsc"]    = parameters.northPoleRightAsc;
	parametersResult["northPoleDeclination"] = parameters.northPoleDeclination;

	result["parameters"] = parametersResult;

	return result;
}

std::string CelestialBody::typeToStr(Type type)
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

CelestialBody::Type CelestialBody::strToType(std::string const& str)
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
