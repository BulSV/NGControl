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
signals:
    void textInputed(QTextEdit *);
public slots:
    QString notes();
    QFont font() const;
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

    int m_prevFontSizeIndex;

    void fillFontFamilies();
private slots:
    void accepted();
    void setFontFamily();
    void setSize();
    void setBold(bool isChecked);
    void setItalic(bool isChecked);
    void setUnderline(bool isChecked);
    void setColor();
    void updateFontSizes();
};

#endif // NOTESDIALOG_H
