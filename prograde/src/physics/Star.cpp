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

#include "physics/Star.hpp"

Star::Star(QJsonObject const& json)
{
	name                   = json["name"].toString().toStdString();
	parameters.mass        = json["mass"].toDouble();
	parameters.radius      = json["radius"].toDouble(70000000.0);
	parameters.temperature = json["temperature"].toDouble(6000.0);
}

Star::Star(std::string const& name, Parameters const& params)
    : name(name)
    , parameters(params)
{
}

Color Star::getColor() const
{
	Color sRGBcolor(blackbody::colorFromTemperature(parameters.temperature));
	return sRGBcolor;
}

QJsonObject Star::getJSONRepresentation() const
{
	QJsonObject result;
	result["name"]        = name.c_str();
	result["mass"]        = parameters.mass;
	result["radius"]      = parameters.radius;
	result["temperature"] = parameters.temperature;
	return result;
}
