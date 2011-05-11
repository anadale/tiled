/*
 * gridstyle.cpp
 * Copyright 2008-2010, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 * Copyright 2011, Gregory Nickonov <gregory@nickonov.ru>
 *
 * This file is part of libtiled.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gridstyle.h"

#include <QVector>

using namespace Tiled;

static QVector<qreal> customDashPattern = QVector<qreal>() << 2 << 2;

GridStyle::GridStyle():
        mStep(0),
        mColor(Qt::black),
        mPenStyle(Qt::NoPen)
{
}

GridStyle::GridStyle(const uint step, const QColor color, const Qt::PenStyle penStyle):
        mStep(step),
        mColor(color),
        mPenStyle(penStyle)
{
    updatePen();
}

GridStyle::GridStyle(const GridStyle &copy):
        mStep(copy.mStep),
        mColor(copy.mColor),
        mPenStyle(copy.mPenStyle)
{
    updatePen();
}

GridStyle &GridStyle::operator =(const GridStyle &other)
{
    if (this != &other) {
        mStep = other.mStep;
        mColor = other.mColor;
        mPenStyle = other.mPenStyle;

        updatePen();
    }

    return *this;
}

bool GridStyle::operator ==(const GridStyle &other) const
{
    return mStep == other.mStep
            && mColor == other.mColor
            && mPenStyle == other.mPenStyle;
}

bool GridStyle::operator !=(const GridStyle &other) const
{
    return !(*this == other);
}

void GridStyle::updatePen()
{
    QColor adjustedColor = mColor;

    adjustedColor.setAlpha(128);

    mPen.setColor(adjustedColor);
    mPen.setStyle(mPenStyle);

    if (mPenStyle == Qt::CustomDashLine)
        mPen.setDashPattern(customDashPattern);
}
