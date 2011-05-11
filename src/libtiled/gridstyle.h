/*
 * gridstyle.h
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

#ifndef GRIDSTYLE_H
#define GRIDSTYLE_H

#include "tiled_global.h"

#include <QColor>
#include <QPen>
#include <QMetaType>

namespace Tiled {

class TILEDSHARED_EXPORT GridStyle
{
public:
    explicit GridStyle();
    GridStyle(const uint step, const QColor color, const Qt::PenStyle penStyle);
    GridStyle(const GridStyle &copy);

    uint step() const { return mStep; }
    void setStep( const uint step ) { mStep = step; }

    QColor color() const { return mColor; }
    void setColor(const QColor color) { mColor = color; updatePen(); }

    Qt::PenStyle penStyle() const { return mPenStyle; }
    void setPenStyle(const Qt::PenStyle style) { mPenStyle = style; updatePen(); }

    bool isValid() const { return mStep > 0 && mPenStyle != Qt::NoPen; }

    GridStyle &operator =(const GridStyle &other);
    bool operator ==(const GridStyle &other) const;
    bool operator !=(const GridStyle &other) const;

    QPen getPen() const { return mPen; }

private:
    void updatePen();

    uint mStep;
    QColor mColor;
    Qt::PenStyle mPenStyle;
    QPen mPen;
};

} // namespace Tiled

Q_DECLARE_METATYPE(Tiled::GridStyle);

#endif // GRIDSTYLE_H
