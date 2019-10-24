/*
    Copyright (C) 2019 Florian Cabot <florian.cabot@epfl.ch>

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

#include "ProgradeSettings.hpp"

ProgradeSettings::ProgradeSettings(QWidget* parent)
    : SettingsWidget(parent)
{
	insertGroup("simulation", tr("Simulation"), 0);
	addDateTimeSetting("starttime", QDateTime().currentDateTimeUtc(),
	                   tr("Start time (UTC)"));
	addBoolSetting("lockedrealtime", false, tr("Lock to Real Time"));

	addDirPathSetting(
	    "planetsystemdir",
	    QFileInfo(QSettings().fileName()).absoluteDir().absolutePath()
	        + "/systems/",
	    tr("Planetary System Root Directory"));

	addBoolSetting("randomsystem", false,
	               tr("Random Planetary System (from subdir)"));

	insertGroup("quality", tr("Quality"), 1);
	addUIntSetting("texmaxsize", 4, tr("Textures max size (x2048)"), 1, 8);
	addUIntSetting("gentexload", 3, tr("Texture generation GPU load"), 1, 4);
	addUIntSetting("atmoquality", 6, tr("Atmosphere rendering quality"), 1, 8);

	setCurrentIndex(0);
}
