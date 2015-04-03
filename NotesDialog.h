#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
//#include <QFontDialog>
#include <QColorDialog>
#include <QFont>

class NotesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NotesDialog(QWidget *parent = 0);
    void show();
signals:
    void textInputed(QTextEdit *, QPointF);
public slots:
    void setText(const QString &text, const QPointF &pos, const QFont &font = QFont(), const QColor &color = QColor());
    void setText(QTextEdit *textEdit, const QPointF &pos);
    QString text();
    QPointF pos() const;
    QFont font() const;
    QColor color() const;
private:
    QComboBox *m_cbFont;
    QComboBox *m_cbSize;
    QPushButton *m_bBold;
    QPushButton *m_bItalic;
    QPushButton *m_bUnderline;
    QPushButton *m_bColor;
    QTextEdit *m_textEdit;
    QPushButton *m_bOK;
    QPushButton *m_bCancel;

    QFont m_font;
    QColor m_color;
    QPointF m_pos;

    int m_prevFontSizeIndex;

    void fillFontFamilies();
private slots:
    void accepted();
    void rejected();
    void setFontFamily();
    void setSize();
    void setBold(bool isChecked);
    void setItalic(bool isChecked);
    void setUnderline(bool isChecked);
    void setColor();
    void updateFontSizes();
};

#endif // NOTESDIALOG_H
