/*
 * preferencesdialog.cpp
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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include "languagemanager.h"
#include "preferences.h"
#include "gridstyle.h"
#include "gridstylesmodel.h"

#include <QAbstractItemDelegate>
#include <QKeyEvent>

#ifndef QT_NO_OPENGL
#include <QGLFormat>
#endif

using namespace Tiled;
using namespace Tiled::Internal;

namespace {

class GridStyleDelegate : public QAbstractItemDelegate
{
public:
    GridStyleDelegate(PreferencesDialog *dialog, QObject *parent=0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    bool eventFilter(QObject *, QEvent *);
private:
    QWidget *createEditorInternal() const;

    PreferencesDialog *mDialog;
    QSize mSizeHint;
};

GridStyleDelegate::GridStyleDelegate(PreferencesDialog *dialog, QObject *parent):
        QAbstractItemDelegate(parent),
        mDialog(dialog)
{
    QWidget *editor = createEditorInternal();

    mSizeHint = editor->sizeHint();

    delete editor;
}

void GridStyleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Draw the tile image
    const QVariant display = index.model()->data(index, Qt::DisplayRole);
    const GridStyle style = display.value<GridStyle>();
    QPen pen = style.getPen();
    QPen savedPen = painter->pen();

    painter->drawText( option.rect.adjusted(5, 0, 0, 0), Qt::AlignLeft|Qt::AlignVCenter, tr("Every %1:").arg(style.step()) );

    int startX = option.rect.left() + 75;
    int endX = option.rect.right() - 5;
    int y = option.rect.center().y();

    pen.setWidth( 5 );
    painter->setPen(pen);
    painter->drawLine(startX, y, endX, y);
    painter->setPen(savedPen);

    // Overlay with highlight color when selected
    if (option.state & QStyle::State_Selected) {
        const qreal opacity = painter->opacity();

        painter->setOpacity(0.5);
        painter->fillRect(option.rect, option.palette.highlight());

        painter->setOpacity(opacity);
    }
}

QSize GridStyleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    return mSizeHint;
}

QWidget *GridStyleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = createEditorInternal();

    editor->setParent(parent);

    return editor;
}

static QColor colors[] = {
    QColor(Qt::black),
    QColor(Qt::green),
    QColor(Qt::gray),
    QColor(Qt::red),
    QColor(Qt::blue),
    QColor(Qt::magenta),
    QColor(Qt::yellow),
    QColor(Qt::cyan),
};

static const char *colorNames[] = {
    "black",
    "green",
    "gray",
    "red",
    "blue",
    "magenta",
    "yellow",
    "cyan",
};

const int colorCount = sizeof(colorNames) / sizeof(const char *);

QWidget *GridStyleDelegate::createEditorInternal() const
{
    QWidget *panel = new QWidget();
    QSpinBox *stepEditor = new QSpinBox();
    QLabel *lblEvery = new QLabel(tr("Every"));
    QLabel *lblWith = new QLabel(tr("cell(s) with"));
    QComboBox *comboPenStyle = new QComboBox();
    QComboBox *comboColor = new QComboBox();
    QHBoxLayout *layout = new QHBoxLayout(panel);

    // label settings
    lblWith->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    lblEvery->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    lblEvery->adjustSize();
    // spin box settings
    stepEditor->setMinimum(1);
    stepEditor->setMaximum(128);

    // combo box settings
    comboPenStyle->addItem(tr("solid lines"));
    comboPenStyle->addItem(tr("dashes"));
    comboPenStyle->addItem(tr("dots"));
    comboPenStyle->addItem(tr("dashes & dots"));
    comboPenStyle->addItem(tr("dashes & double dots"));
    comboPenStyle->addItem(tr("beautiful dashes"));
    comboPenStyle->setObjectName(QString::fromAscii("penStyle"));

    // color
    for (int i = 0; i < colorCount; ++i)
        comboColor->addItem(tr(colorNames[i]));

    comboColor->setObjectName(QString::fromAscii("color"));

    layout->setSpacing(1);
    layout->setMargin(0);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->addWidget(lblEvery);
    layout->addWidget(stepEditor);
    layout->addStretch();
    layout->addWidget(lblWith);
    layout->addStretch();
    layout->addWidget(comboColor);
    layout->addWidget(comboPenStyle);

    layout->setSizeConstraint(QLayout::SetNoConstraint);
    panel->setLayout(layout);

    panel->setAutoFillBackground(true);
    return panel;
}

bool GridStyleDelegate::eventFilter(QObject *object, QEvent *event)
{
    QWidget *editor = qobject_cast<QWidget*>(object);

    if (!editor)
        return false;

    if (event->type() == QEvent::KeyPress) {
        switch (static_cast<QKeyEvent *>(event)->key()) {
        case Qt::Key_Tab:
            emit commitData(editor);
            emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
            return true;
        case Qt::Key_Backtab:
            emit commitData(editor);
            emit closeEditor(editor, QAbstractItemDelegate::EditPreviousItem);
            return true;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            emit commitData(editor);
            emit closeEditor(editor);
            return true;
        case Qt::Key_Escape:
            // don't commit data
            emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
            break;
        default:
            return false;
        }
        if (editor->parentWidget())
            editor->parentWidget()->setFocus();
        return true;
    } else if (event->type() == QEvent::FocusOut || (event->type() == QEvent::Hide && editor->isWindow())) {
        //the Hide event will take care of he editors that are in fact complete dialogs
        if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
            QWidget *w = QApplication::focusWidget();
            while (w) { // don't worry about focus changes internally in the editor
                if (w == editor)
                    return false;
                w = w->parentWidget();
            }

            emit commitData(editor);
            emit closeEditor(editor, NoHint);
        }
    } else if (event->type() == QEvent::ShortcutOverride) {
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
            event->accept();
            return true;
        }
    }
    return false;
}

void GridStyleDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect = option.rect;
    QSize sizeHint = editor->sizeHint();

    if (rect.width() < sizeHint.width()) rect.setWidth(sizeHint.width());
    if (rect.height() < sizeHint.height()) rect.setHeight(sizeHint.height());

    editor->setGeometry(rect);
}

void GridStyleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const GridStyle &style = index.data(Qt::DisplayRole).value<GridStyle>();

    QSpinBox *stepEditor = editor->findChild<QSpinBox *>();
    QComboBox *penStyleEditor = editor->findChild<QComboBox *>(QString::fromAscii("penStyle"));
    QComboBox *colorEditor = editor->findChild<QComboBox *>(QString::fromAscii("color"));

    stepEditor->setValue(style.step());
    penStyleEditor->setCurrentIndex(((int) style.penStyle()) - 1);

    for (int i = 0; i < colorCount; ++i) {
        if (colors[i] == style.color()) {
            colorEditor->setCurrentIndex(i);
            break;
        }
    }
}

void GridStyleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(model);

    QSpinBox *stepEditor = editor->findChild<QSpinBox *>();
    QComboBox *penStyleEditor = editor->findChild<QComboBox *>(QString::fromAscii("penStyle"));
    QComboBox *colorEditor = editor->findChild<QComboBox *>(QString::fromAscii("color"));

    const uint step = (uint) stepEditor->value();
    const Qt::PenStyle penStyle = (Qt::PenStyle) (penStyleEditor->currentIndex() + 1);
    const QColor color = colors[colorEditor->currentIndex()];
    GridStyle style( step, color, penStyle);

    model->setData(index, QVariant::fromValue<GridStyle>(style));
}
} // Anonymous namespace

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::PreferencesDialog),
    mLanguages(LanguageManager::instance()->availableLanguages())
{
    mUi->setupUi(this);

#ifndef QT_NO_OPENGL
    mUi->openGL->setEnabled(QGLFormat::hasOpenGL());
#else
    mUi->openGL->setEnabled(false);
#endif

    foreach (const QString &name, mLanguages) {
        QLocale locale(name);
        QString string = QString(QLatin1String("%1 (%2)"))
            .arg(QLocale::languageToString(locale.language()))
            .arg(QLocale::countryToString(locale.country()));
        mUi->languageCombo->addItem(string, name);
    }

    mUi->languageCombo->model()->sort(0);
    mUi->languageCombo->insertItem(0, tr("System default"));

    fromPreferences();

    connect(mUi->languageCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(languageSelected(int)));
    connect(mUi->openGL, SIGNAL(toggled(bool)), SLOT(useOpenGLToggled(bool)));

    QItemSelectionModel *selectionModel = mUi->gridStylesView->selectionModel();

    connect(selectionModel, SIGNAL(currentRowChanged(const QModelIndex&,const QModelIndex&)), this, SLOT(gridStylesViewSelectionChanged(const QModelIndex&,const QModelIndex&)));
    connect(mUi->addGridStyle, SIGNAL(clicked()), this, SLOT(addGridStyleClicked()));
    connect(mUi->removeGridStyle, SIGNAL(clicked()), this, SLOT(removeGridStyleClicked()));
}

PreferencesDialog::~PreferencesDialog()
{
    toPreferences();
    delete mUi;
}

void PreferencesDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange: {
            const int formatIndex = mUi->layerDataCombo->currentIndex();
            mUi->retranslateUi(this);
            mUi->layerDataCombo->setCurrentIndex(formatIndex);
            mUi->languageCombo->setItemText(0, tr("System default"));
        }
        break;
    default:
        break;
    }
}

void PreferencesDialog::languageSelected(int index)
{
    const QString language = mUi->languageCombo->itemData(index).toString();
    Preferences *prefs = Preferences::instance();
    prefs->setLanguage(language);
}

void PreferencesDialog::useOpenGLToggled(bool useOpenGL)
{
    Preferences::instance()->setUseOpenGL(useOpenGL);
}

void PreferencesDialog::gridStylesViewSelectionChanged(const QModelIndex &current,const QModelIndex &previous)
{
    Q_UNUSED(previous);

    mUi->removeGridStyle->setEnabled(current.isValid());
}

void PreferencesDialog::addGridStyleClicked()
{
    int row = mUi->gridStylesView->currentIndex().isValid() ? mUi->gridStylesView->currentIndex().row() + 1: mGridStylesModel->rowCount();

    mGridStylesModel->insertRows(row, 1);
}

void PreferencesDialog::removeGridStyleClicked()
{
    mGridStylesModel->removeRows(mUi->gridStylesView->currentIndex().row(), 1);
}

void PreferencesDialog::fromPreferences()
{
    const Preferences *prefs = Preferences::instance();
    mUi->reloadTilesetImages->setChecked(prefs->reloadTilesetsOnChange());
    mUi->enableDtd->setChecked(prefs->dtdEnabled());
    if (mUi->openGL->isEnabled())
        mUi->openGL->setChecked(prefs->useOpenGL());

    int formatIndex = 0;
    switch (prefs->layerDataFormat()) {
    case MapWriter::XML:
        formatIndex = 0;
        break;
    case MapWriter::Base64:
        formatIndex = 1;
        break;
    case MapWriter::Base64Gzip:
        formatIndex = 2;
        break;
    default:
    case MapWriter::Base64Zlib:
        formatIndex = 3;
        break;
    case MapWriter::CSV:
        formatIndex = 4;
        break;
    }
    mUi->layerDataCombo->setCurrentIndex(formatIndex);

    // Not found (-1) ends up at index 0, system default
    int languageIndex = mUi->languageCombo->findData(prefs->language());
    if (languageIndex == -1)
        languageIndex = 0;
    mUi->languageCombo->setCurrentIndex(languageIndex);

    mUi->backgroundColor->setColor(prefs->backgroundColor());

    mGridStylesModel = new GridStylesModel(prefs->gridStyles());
    mUi->gridStylesView->setModel(mGridStylesModel);
    mUi->gridStylesView->setItemDelegate(new GridStyleDelegate(this, this));
}

void PreferencesDialog::toPreferences()
{
    Preferences *prefs = Preferences::instance();

    prefs->setReloadTilesetsOnChanged(mUi->reloadTilesetImages->isChecked());
    prefs->setDtdEnabled(mUi->enableDtd->isChecked());
    prefs->setLayerDataFormat(layerDataFormat());

    prefs->setBackgroundColor(mUi->backgroundColor->color());
    prefs->setGridStyles(mGridStylesModel->getStyles());
}

MapWriter::LayerDataFormat PreferencesDialog::layerDataFormat() const
{
    switch (mUi->layerDataCombo->currentIndex()) {
    case 0:
        return MapWriter::XML;
    case 1:
        return MapWriter::Base64;
    case 2:
        return MapWriter::Base64Gzip;
    case 3:
    default:
        return MapWriter::Base64Zlib;
    case 4:
        return MapWriter::CSV;
    }
}
