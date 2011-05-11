/*
 * gridstylesmodel.cpp
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

#include "gridstylesmodel.h"

using namespace Tiled;
using namespace Tiled::Internal;

GridStylesModel::GridStylesModel(QVector<GridStyle> gridStyles, QObject *parent):
        QAbstractListModel(parent),
        mStyles(gridStyles)
{
}

int GridStylesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mStyles.count();
}

int GridStylesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant GridStylesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mStyles.count())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const GridStyle &style = mStyles.at(index.row());

        return QVariant::fromValue<GridStyle>(style);
    }

    return QVariant();
}

bool GridStylesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();
        const GridStyle style = value.value<GridStyle>();

        mStyles.replace(row, style);
        emit dataChanged(index, index);

        return true;
    }

    return false;
}

Qt::ItemFlags GridStylesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool GridStylesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginInsertRows(QModelIndex(), row, row + count - 1);

    int guessedStep = 0;
    int guessedLimit = 0;

    if (row >= mStyles.count()) {
        if (mStyles.count() > 0) {
            const GridStyle &previous = mStyles.at(mStyles.count() - 1);

            guessedStep = previous.step() * 2;
        } else {
            guessedStep = 1;
        }
    }
    else
    {
        if (row == 0) {
            const GridStyle &next = mStyles.at(0);

            guessedStep = (uint) (next.step() / 2);

            if (guessedStep == 0)
                guessedStep = 1;
        } else {
            const GridStyle &prev = mStyles.at(row - 1);
            const GridStyle &next = mStyles.at(row);

            guessedStep = (uint)(abs(next.step() - prev.step()) / 2) + prev.step();
            guessedLimit = next.step();
        }
    }

    for (int i = 0; i < count; ++i) {
        mStyles.insert(row, GridStyle(guessedStep, QColor(Qt::black), Qt::DotLine));

        if (guessedStep * 2 < guessedLimit)
            guessedStep *= 2;
    }

    endInsertRows();

    return true;
}

bool GridStylesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    mStyles.remove(row, count);

    endRemoveRows();

    return true;
}
