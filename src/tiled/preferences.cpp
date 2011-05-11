/*
 * preferences.cpp
 * Copyright 2009-2010, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "preferences.h"

#include "languagemanager.h"
#include "tilesetmanager.h"

#include <QSettings>

using namespace Tiled;
using namespace Tiled::Internal;

Preferences *Preferences::mInstance = 0;

Preferences *Preferences::instance()
{
    if (!mInstance)
        mInstance = new Preferences;
    return mInstance;
}

void Preferences::deleteInstance()
{
    delete mInstance;
    mInstance = 0;
}

Preferences::Preferences()
    : mSettings(new QSettings)
{
    // Retrieve storage settings
    mSettings->beginGroup(QLatin1String("Storage"));
    mLayerDataFormat = (MapWriter::LayerDataFormat)
                       mSettings->value(QLatin1String("LayerDataFormat"),
                                        MapWriter::Base64Zlib).toInt();
    mDtdEnabled = mSettings->value(QLatin1String("DtdEnabled")).toBool();
    mReloadTilesetsOnChange =
            mSettings->value(QLatin1String("ReloadTilesets"), true).toBool();
    mSettings->endGroup();

    // Retrieve interface settings
    mSettings->beginGroup(QLatin1String("Interface"));
    mShowGrid = mSettings->value(QLatin1String("ShowGrid"), false).toBool();
    mSnapToGrid = mSettings->value(QLatin1String("SnapToGrid"),
                                   false).toBool();
    mLanguage = mSettings->value(QLatin1String("Language"),
                                 QString()).toString();
    mUseOpenGL = mSettings->value(QLatin1String("OpenGL"), false).toBool();
    mSettings->endGroup();

    // Retrieve Grid and Background settings
    mSettings->beginGroup(QLatin1String("BackgroundAndGrid"));
    QString colorName = mSettings->value(QLatin1String("BackgroundColor")).toString();
    mBackgroundColor = colorName.isEmpty() ? QColor(Qt::darkGray) : QColor(colorName);

    int styleCount = mSettings->beginReadArray(QLatin1String("GridStyles"));

    if (styleCount > 0) {
        for (int index = 0; index < styleCount; ++index) {
            mSettings->setArrayIndex(index);
            int step = mSettings->value(QLatin1String("Step")).toInt();
            QColor color = QColor(mSettings->value(QLatin1String("Color")).toString());
            Qt::PenStyle penStyle = (Qt::PenStyle) mSettings->value(QLatin1String("PenStyle")).toInt();

            mGridStyles.append(GridStyle(step, color, penStyle));
        }
    } else {
        // Creating default grid settings

        mGridStyles.append(GridStyle(1, QColor(Qt::black), Qt::DotLine));
        mGridStyles.append(GridStyle(5, QColor(Qt::black), Qt::CustomDashLine));
    }

    mSettings->endArray();
    mSettings->endGroup();

    TilesetManager *tilesetManager = TilesetManager::instance();
    tilesetManager->setReloadTilesetsOnChange(mReloadTilesetsOnChange);
}

Preferences::~Preferences()
{
    delete mSettings;
}

void Preferences::setShowGrid(bool showGrid)
{
    if (mShowGrid == showGrid)
        return;

    mShowGrid = showGrid;
    mSettings->setValue(QLatin1String("Interface/ShowGrid"), mShowGrid);
    emit showGridChanged(mShowGrid);
}

void Preferences::setSnapToGrid(bool snapToGrid)
{
    if (mSnapToGrid == snapToGrid)
        return;

    mSnapToGrid = snapToGrid;
    mSettings->setValue(QLatin1String("Interface/SnapToGrid"), mSnapToGrid);
    emit snapToGridChanged(mSnapToGrid);
}

void Preferences::setBackgroundColor(const QColor backgroundColor)
{
    if (mBackgroundColor == backgroundColor)
        return;

    mBackgroundColor = backgroundColor;
    mSettings->setValue(QLatin1String("BackgroundAndGrid/BackgroundColor"), mBackgroundColor.name());
    emit backgroundColorChanged(backgroundColor);
}

bool stepLessThan(const GridStyle &s1, const GridStyle &s2)
{
    return s1.step() < s2.step();
}

void Preferences::setGridStyles(const QVector<GridStyle> &gridStyles)
{
    mGridStyles = gridStyles;
    qSort(mGridStyles.begin(), mGridStyles.end(), stepLessThan);

    mSettings->beginWriteArray(QLatin1String("BackgroundAndGrid/GridStyles"), mGridStyles.count());

    for (int i = 0; i < mGridStyles.count(); ++i) {
        const GridStyle &style = mGridStyles.at(i);

        mSettings->setArrayIndex(i);
        mSettings->setValue(QLatin1String("Step"), style.step());
        mSettings->setValue(QLatin1String("Color"), style.color().name());
        mSettings->setValue(QLatin1String("PenStyle"), style.penStyle());
    }

    mSettings->endArray();

    emit gridStylesChanged();
}

MapWriter::LayerDataFormat Preferences::layerDataFormat() const
{
    return mLayerDataFormat;
}

void Preferences::setLayerDataFormat(MapWriter::LayerDataFormat
                                     layerDataFormat)
{
    if (mLayerDataFormat == layerDataFormat)
        return;

    mLayerDataFormat = layerDataFormat;
    mSettings->setValue(QLatin1String("Storage/LayerDataFormat"),
                        mLayerDataFormat);
}

bool Preferences::dtdEnabled() const
{
    return mDtdEnabled;
}

void Preferences::setDtdEnabled(bool enabled)
{
    mDtdEnabled = enabled;
    mSettings->setValue(QLatin1String("Storage/DtdEnabled"), enabled);
}

QString Preferences::language() const
{
    return mLanguage;
}

void Preferences::setLanguage(const QString &language)
{
    if (mLanguage == language)
        return;

    mLanguage = language;
    mSettings->setValue(QLatin1String("Interface/Language"),
                        mLanguage);

    LanguageManager::instance()->installTranslators();
}

bool Preferences::reloadTilesetsOnChange() const
{
    return mReloadTilesetsOnChange;
}

void Preferences::setReloadTilesetsOnChanged(bool value)
{
    if (mReloadTilesetsOnChange == value)
        return;

    mReloadTilesetsOnChange = value;
    mSettings->setValue(QLatin1String("Storage/ReloadTilesets"),
                        mReloadTilesetsOnChange);

    TilesetManager *tilesetManager = TilesetManager::instance();
    tilesetManager->setReloadTilesetsOnChange(mReloadTilesetsOnChange);
}

void Preferences::setUseOpenGL(bool useOpenGL)
{
    if (mUseOpenGL == useOpenGL)
        return;

    mUseOpenGL = useOpenGL;
    mSettings->setValue(QLatin1String("Interface/OpenGL"), mUseOpenGL);

    emit useOpenGLChanged(mUseOpenGL);
}
