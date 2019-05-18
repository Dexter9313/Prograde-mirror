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

float CelestialBody::getPrimeMeridianSiderealTimeAtUT(UniversalTime uT) const
{
	uT -= round(uT / parameters.siderealRotationPeriod)
	      * UniversalTime(parameters.siderealRotationPeriod);

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
