#include "NotesDialog.h"
#include <QGridLayout>
#include <QSpacerItem>
#include <QFontDatabase>
#include <QDebug>

NotesDialog::NotesDialog(QWidget *parent) :
    QDialog(parent)
  , m_cbFont(new QComboBox(this))
  , m_cbSize(new QComboBox(this))
  , m_bBold(new QPushButton("B", this))
  , m_bItalic(new QPushButton("I", this))
  , m_bUnderline(new QPushButton("U", this))
  , m_bColor(new QPushButton("Color", this))
  , m_textEdit(new QTextEdit(this))
  , m_bOK(new QPushButton("OK", this))
  , m_bCancel(new QPushButton("Cancel", this))
  , m_prevFontSizeIndex(0)
{
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_cbFont, 0, 0, 1, 2);
    layout->addWidget(m_cbSize, 0, 2);
    layout->addWidget(m_bBold, 0, 3);
    layout->addWidget(m_bItalic, 0, 4);
    layout->addWidget(m_bUnderline, 0, 5);
    layout->addWidget(m_bColor, 0, 6);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 0, 7);
    layout->addWidget(m_textEdit, 1, 0, 1, 8);
    layout->addWidget(m_bOK, 2, 0);
    layout->addWidget(m_bCancel, 2, 1);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding), 2, 2);

    setLayout(layout);

    fillFontFamilies();
    updateFontSizes();
    setSize();

    m_textEdit->setFocus();

    m_bBold->setCheckable(true);
    m_bItalic->setCheckable(true);
    m_bUnderline->setCheckable(true);

    connect(m_bCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_bOK, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(m_bColor, SIGNAL(clicked()), this, SLOT(setColor()));
    connect(m_bBold, SIGNAL(toggled(bool)), this, SLOT(setBold(bool)));
    connect(m_bItalic, SIGNAL(toggled(bool)), this, SLOT(setItalic(bool)));
    connect(m_bUnderline, SIGNAL(toggled(bool)), this, SLOT(setUnderline(bool)));
    connect(m_cbFont, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFontSizes()));
    connect(m_cbFont, SIGNAL(currentIndexChanged(int)), this, SLOT(setFontFamily()));
    connect(m_cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(setSize()));
}

QString NotesDialog::notes()
{
    return m_textEdit->toPlainText();
}

QFont NotesDialog::font() const
{
    return m_font;
}

void NotesDialog::fillFontFamilies()
{
    QFontDatabase fontsFamilies;
    m_cbFont->addItems(fontsFamilies.families());

    setFontFamily();
}

void NotesDialog::accepted()
{
    if(m_textEdit->toPlainText().isEmpty()) {
        close();
        return;
    }

    emit textInputed(m_textEdit);
    close();
}

void NotesDialog::setColor()
{
    m_textEdit->selectAll();

    m_textEdit->setTextColor(QColorDialog::getColor());

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);
}

void NotesDialog::updateFontSizes()
{
    if(m_cbSize->count() > 0) {
        m_prevFontSizeIndex = m_cbSize->currentIndex();
        m_cbSize->clear();
    }

    QFontDatabase fontsSizes;
    QStringList sizesStrList;

    QList<int> sizesList = fontsSizes.pointSizes(m_cbFont->currentText());

    for(int i = 0; i < sizesList.size(); ++i) {
        sizesStrList << QString::number(sizesList.at(i));
    }

    m_cbSize->addItems(sizesStrList);
    m_cbSize->setCurrentIndex(m_prevFontSizeIndex);
}

void NotesDialog::setFontFamily()
{
    m_textEdit->selectAll();

    m_font.setFamily(m_cbFont->currentText());
    m_textEdit->setFont(m_font);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);
}

void NotesDialog::setSize()
{
    if(m_cbSize->count() > 0) {
        m_textEdit->selectAll();

        m_font.setPointSize(m_cbSize->currentText().toInt());
        m_textEdit->setFont(m_font);

        QTextCursor cursor = m_textEdit->textCursor();
        cursor.clearSelection();
        m_textEdit->setTextCursor(cursor);
    }
}

void NotesDialog::setBold(bool isChecked)
{
    m_textEdit->selectAll();

    m_font.setBold(isChecked);
    m_textEdit->setFont(m_font);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);
}

void NotesDialog::setItalic(bool isChecked)
{
    m_textEdit->selectAll();

    m_font.setItalic(isChecked);
    m_textEdit->setFont(m_font);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);
}

void NotesDialog::setUnderline(bool isChecked)
{
    m_textEdit->selectAll();

    m_font.setUnderline(isChecked);
    m_textEdit->setFont(m_font);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);
}