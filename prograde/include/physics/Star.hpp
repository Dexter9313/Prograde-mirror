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

#ifndef STAR_HPP
#define STAR_HPP

#include <QJsonObject>
#include <string>

class Star
{
  public:
	struct Parameters
	{
		double mass;        // kg
		double radius;      // m
		double temperature; // K
	};

	Star(QJsonObject const& json);
	Star(std::string const& name, Parameters const& params);
	Star(Star const& copiedStar) = default;
	std::string const& getName() { return name; };
	Parameters getParameters() const { return parameters; };
	QJsonObject getJSONRepresentation() const;

  private:
	std::string name;

	Parameters parameters;
};

#endif // STAR_HPP
