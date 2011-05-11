/*
 * gridstylesmodel.h
 * Copyright 2008-2011, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 * Copyright 2011, Gregory Nickonov <gregory@nickonov.ru>
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
#ifndef GRIDSTYLESMODEL_H
#define GRIDSTYLESMODEL_H

#include "gridstyle.h"

#include <QAbstractListModel>
#include <QVector>

namespace Tiled {
namespace Internal {

class GridStylesModel : public QAbstractListModel
{
public:
    GridStylesModel(QVector<GridStyle> gridStyles, QObject *parent=0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    const QVector<GridStyle> &getStyles() const { return mStyles; }

private:
    QVector<GridStyle> mStyles;
};

} // namespace Internal
} // namespace Tiled
#endif // GRIDSTYLESMODEL_H
