
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTextCodec>
#include <QString>
#include <QFile>
#include <QMessageBox>
#include <QTextDocumentWriter>
#include <QClipboard>
#include <QMimeData>
#include <QTextCharFormat>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QTextListFormat>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QByteArray>
#include "findform.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_as_triggered();

    void on_actionNew_triggered();

    void on_actionBold_triggered();

    void on_actionItalic_triggered();

    void on_actionUnderline_triggered();

    void on_actionLeft_triggered();

    void on_actionCenter_triggered();

    void on_actionRight_triggered();

    void on_actionJustify_triggered();

    void currentCharFormatChanged(const QTextCharFormat &format);

    void cursorPositionChanged();

    void on_actionFind_triggered();

    void on_actionReplace_triggered();

    void on_actionColor_triggered();

    void textFamily(const QString &f);

    void textSize(const QString &p);

    void textStyle(int styleIndex);

signals:

    void activated(QString str);

private:
    Ui::MainWindow *ui;
    bool loadFile(const QString &fileName);
    void setCurrentFileName(const QString &fileName);
    QString fileName;
    bool maybeSave();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void alignmentChanged(Qt::Alignment a);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    QComboBox* comboStyle;
    QFontComboBox* comboFont;
    QComboBox* comboSize;
    FindForm* form;
};

#endif // MAINWINDOW_H
