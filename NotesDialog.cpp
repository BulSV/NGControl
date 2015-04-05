#include "NotesDialog.h"
#include <QGridLayout>
#include <QSpacerItem>
#include <QFontDatabase>
#include <QDebug>

NotesDialog::NotesDialog(QWidget *parent) :
    QDialog(parent)
  , m_cbFont(new QComboBox(this))
  , m_cbSize(new QComboBox(this))
  , m_bBold(new QPushButton(QIcon(":Resources/bold.png"), QString::null, this))
  , m_bItalic(new QPushButton(QIcon(":Resources/italic.png"), QString::null, this))
  , m_bUnderline(new QPushButton(QIcon(":Resources/underline.png"), QString::null, this))
  , m_bColor(new QPushButton(QIcon(":Resources/color.png"), QString::null, this))
  , m_textEdit(new QTextEdit(this))
  , m_bOK(new QPushButton(QString::fromUtf8("OK"), this))
  , m_bCancel(new QPushButton(QString::fromUtf8("Cancel"), this))
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

    m_bBold->setCheckable(true);
    m_bItalic->setCheckable(true);
    m_bUnderline->setCheckable(true);

    m_textEdit->setAutoFormatting(QTextEdit::AutoAll);
    m_textEdit->setAcceptRichText(false);

    connect(this, SIGNAL(rejected()), SLOT(rejected()));
    connect(m_bCancel, SIGNAL(clicked()), this, SLOT(rejected()));
    connect(m_bOK, SIGNAL(clicked()), this, SLOT(accepted()));
    connect(m_bColor, SIGNAL(clicked()), this, SLOT(setColor()));
    connect(m_bBold, SIGNAL(toggled(bool)), this, SLOT(setBold(bool)));
    connect(m_bItalic, SIGNAL(toggled(bool)), this, SLOT(setItalic(bool)));
    connect(m_bUnderline, SIGNAL(toggled(bool)), this, SLOT(setUnderline(bool)));
    connect(m_cbFont, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFontSizes()));
    connect(m_cbFont, SIGNAL(currentIndexChanged(int)), this, SLOT(setFontFamily()));
    connect(m_cbSize, SIGNAL(currentIndexChanged(int)), this, SLOT(setSize()));

    connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(refresh()));
}

void NotesDialog::show()
{
    QDialog::show();
    m_textEdit->setFocus();
}

void NotesDialog::setText(const QString &text, const QPointF &pos, const QFont &font, const QColor &color)
{
    m_textEdit->setText(text);
    m_textEdit->setFont(font);
    m_cbFont->setCurrentText(font.family());
    m_cbSize->setCurrentText(QString::number(font.pointSize()));
    m_color = color;
    m_textEdit->setTextColor(m_color);
    m_pos = pos;

    refresh();

    qDebug() << "setText\nfont:" << "\nfamily:" << m_textEdit->font().family()
             << "\nsize:" << m_textEdit->font().pointSize() << "\nbold:" << m_textEdit->font().bold()
             << "\nitalic:" << m_textEdit->font().italic() << "\nunderline:" << m_textEdit->font().underline();
    qDebug() << "color:" << m_textEdit->textColor().name();
}

void NotesDialog::setText(QTextEdit *textEdit, const QPointF &pos)
{
    if( textEdit ) {
        m_textEdit = textEdit;
        m_pos = pos;
    }
}

QString NotesDialog::text()
{
    return m_textEdit->toPlainText();
}

QPointF NotesDialog::pos() const
{
    return m_pos;
}

QFont NotesDialog::font() const
{
    return m_font;
}

QColor NotesDialog::color() const
{
    return m_color;
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
        accept();
        return;
    }

    emit textInputed(m_textEdit, m_pos);
    accept();
}

void NotesDialog::rejected()
{
    accept();
}

void NotesDialog::setColor()
{
    m_textEdit->selectAll();

    m_color = QColorDialog::getColor();
    m_textEdit->setTextColor(m_color);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);

    m_textEdit->setFocus();
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

void NotesDialog::refresh()
{
    m_cbFont->setCurrentText(m_font.family());
    m_cbSize->setCurrentText(QString::number(m_font.pointSize()));
    m_textEdit->selectAll();

    m_textEdit->setFont(m_font);
    m_textEdit->setTextColor(m_color);
    qDebug() << "M_color:" << m_color.name();

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);

    m_textEdit->setFocus();
}

void NotesDialog::setFontFamily()
{
    m_textEdit->selectAll();

    m_font.setFamily(m_cbFont->currentText());
    m_textEdit->setFont(m_font);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);

    m_textEdit->setFocus();
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

        m_textEdit->setFocus();
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

    m_textEdit->setFocus();
}

void NotesDialog::setItalic(bool isChecked)
{
    m_textEdit->selectAll();

    m_font.setItalic(isChecked);
    m_textEdit->setFont(m_font);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);

    m_textEdit->setFocus();
}

void NotesDialog::setUnderline(bool isChecked)
{
    m_textEdit->selectAll();

    m_font.setUnderline(isChecked);
    m_textEdit->setFont(m_font);

    QTextCursor cursor = m_textEdit->textCursor();
    cursor.clearSelection();
    m_textEdit->setTextCursor(cursor);

    m_textEdit->setFocus();
}
