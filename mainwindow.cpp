#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->textEdit);
    ui->textEdit->setFocus();
    comboStyle = new QComboBox(ui->toolBar_2);
    ui->toolBar_2->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Bullet List (Disc)");
    comboStyle->addItem("Bullet List (Circle)");
    comboStyle->addItem("Bullet List (Square)");
    comboStyle->addItem("Ordered List (Decimal)");
    comboStyle->addItem("Ordered List (Alpha lower)");
    comboStyle->addItem("Ordered List (Alpha upper)");
    comboStyle->addItem("Ordered List (Roman lower)");
    comboStyle->addItem("Ordered List (Roman upper)");
    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    comboFont = new QFontComboBox(ui->toolBar_2);
    ui->toolBar_2->addWidget(comboFont);
    connect(comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

    comboSize = new QComboBox(ui->toolBar_2);
    comboSize->setObjectName("comboSize");
    ui->toolBar_2->addWidget(comboSize);
    comboSize->setEditable(true);
    QFontDatabase db;
    foreach(int size, db.standardSizes()) comboSize->addItem(QString::number(size));
    connect(comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));
    QFile file("last.txt");
    if(file.open(QFile::ReadOnly))
    {
        QString file_name;
        QTextStream fin(&file);
        file_name = fin.readAll();
        if (loadFile(file_name))
            setCurrentFileName(file_name);
    }
    else
    {
        setCurrentFileName(QString());
    }
    file.close();
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        ui->actionPaste->setEnabled(md->hasText());
#endif
    connect(ui->textEdit, SIGNAL(copyAvailable(bool)), ui->actionCut,
            SLOT(setEnabled(bool)));
    connect(ui->textEdit, SIGNAL(copyAvailable(bool)), ui->actionCopy,
            SLOT(setEnabled(bool)));
    connect(ui->textEdit, SIGNAL(undoAvailable(bool)), ui->actionUndo,
            SLOT(setEnabled(bool)));
    connect(ui->textEdit, SIGNAL(redoAvailable(bool)), ui->actionRedo,
            SLOT(setEnabled(bool)));
    alignmentChanged(ui->textEdit->alignment());
    connect(ui->textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this,
            SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(ui->textEdit, SIGNAL(cursorPositionChanged()), this,
            SLOT(cursorPositionChanged()));
    fontChanged(ui->textEdit->font());
    colorChanged(ui->textEdit->textColor());
}

MainWindow::~MainWindow()
{
    if (maybeSave())
    {
        delete ui;
    }
    QFile file("last.txt");
    file.open(QFile::WriteOnly);
    QTextStream fout(&file);
    fout << fileName;
    file.close();
}

void MainWindow::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    ui->textEdit->document()->setModified(false);
    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();
    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Rich Text")));
    setWindowModified(false);
}
bool MainWindow::loadFile(const QString &fileName)
{
    if (!QFile::exists(fileName))
    {
        QMessageBox::warning(this, tr("Text editor"),
                             tr("Cannot find file %1:\n%2.")
                                 .arg(fileName).arg("Not exist"));
        return 0;
    }
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, tr("Text editor"),
                             tr("Cannot open file %1:\n%2.")
                                 .arg(fileName).arg("The file is corrupted or does not exist"));
        return 0;
    }
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor); //делаем курсор "часики"
#endif
    QByteArray data;
//    if(fileName.endsWith(".odt", Qt::CaseInsensitive) || fileName.endsWith(".zip", Qt::CaseInsensitive))
//    {
//        data = file.readAll();
//        QFile file2("temp.zip");
//        file2.open(QFile::WriteOnly);
//        file2.write(data);
//        file2.close();
//        file2.open(QFile::ReadOnly);
//        data = file2.readAll();
//        file2.remove();
//        QByteArray data2 = qUncompress(data);
//    }
//    else
//    {
        data = file.readAll();
//    }
    QTextCodec *codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) ui->textEdit->setHtml(str);
    else {
        str = QString::fromLocal8Bit(data);
        ui->textEdit->setPlainText(str);
    }
    setCurrentFileName(fileName);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor(); //восстанавливаем обычный курсор35
#endif
    statusBar()->showMessage(tr("File loaded"), 2000);
    file.close();
    return 1;
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified())
        return true;
    if (fileName.startsWith(QLatin1String(":/")))
        return true;
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
    {
        on_actionSave_triggered();
        return true;
    }
    else if (ret == QMessageBox::Cancel) return false;
    return true;
}

void MainWindow::on_actionOpen_triggered()
{
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()) loadFile(fileName);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (fileName.isEmpty())
    {
        on_actionSave_as_triggered(); return;
    }
    QTextDocumentWriter writer(fileName);
    bool success = writer.write(ui->textEdit->document());
    if (success) ui->textEdit->document()->setModified(false);
}


void MainWindow::on_actionSave_as_triggered()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(),
                                              tr("Text-Files (*.txt);;HTML-Files (*.htm *.html);;Open document format (*.odt);;All Files (*)"));
    if (fn.isEmpty()) return;
    if (! (fn.endsWith(".txt", Qt::CaseInsensitive) || fn.endsWith(".htm", Qt::CaseInsensitive) ||
          fn.endsWith(".html", Qt::CaseInsensitive) || fn.endsWith(".odt", Qt::CaseInsensitive)))
        fn += ".txt";
    // default
    setCurrentFileName(fn);
    on_actionSave_triggered();
}


void MainWindow::on_actionNew_triggered()
{
    if (maybeSave())
    {
        ui->textEdit->clear();
        setCurrentFileName(QString());
    }
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    ui->textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::on_actionBold_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->actionBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}


void MainWindow::on_actionItalic_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->actionItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}


void MainWindow::on_actionUnderline_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->actionUnderline->isChecked());
    //fmt.setFontOverline(ui->actionUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft) {
        ui->actionLeft->setChecked(true);
        ui->actionCenter->setChecked(false);
        ui->actionRight->setChecked(false);
        ui->actionJustify->setChecked(false);
    } else if (a & Qt::AlignHCenter) {
        ui->actionLeft->setChecked(false);
        ui->actionCenter->setChecked(true);
        ui->actionRight->setChecked(false);
        ui->actionJustify->setChecked(false);
    } else if (a & Qt::AlignRight) {
        ui->actionLeft->setChecked(false);
        ui->actionCenter->setChecked(false);
        ui->actionRight->setChecked(true);
        ui->actionJustify->setChecked(false);
    } else if (a & Qt::AlignJustify) {
        ui->actionLeft->setChecked(false);
        ui->actionCenter->setChecked(false);
        ui->actionRight->setChecked(false);
        ui->actionJustify->setChecked(true);
    }
}


void MainWindow::on_actionLeft_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    alignmentChanged(ui->textEdit->alignment());
}


void MainWindow::on_actionCenter_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignCenter);
    alignmentChanged(ui->textEdit->alignment());
}


void MainWindow::on_actionRight_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    alignmentChanged(ui->textEdit->alignment());
}


void MainWindow::on_actionJustify_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignJustify);
    alignmentChanged(ui->textEdit->alignment());
}

void MainWindow::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}
void MainWindow::cursorPositionChanged()
{
    alignmentChanged(ui->textEdit->alignment());
}
void MainWindow::fontChanged(const QFont &f)
{
    ui->actionBold->setChecked(f.bold());
    ui->actionItalic->setChecked(f.italic());
    ui->actionUnderline->setChecked(f.underline());
}

void MainWindow::on_actionFind_triggered()
{
    form = new FindForm(this, ui->textEdit, 0);
    (*form).show();
}


void MainWindow::on_actionReplace_triggered()
{
    form = new FindForm(this, ui->textEdit, 1);
    (*form).show();
}

void MainWindow::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    ui->actionColor->setIcon(pix);
}

void MainWindow::on_actionColor_triggered()
{
    QColor col = QColorDialog::getColor(ui->textEdit->textColor(), this);
    if (!col.isValid()) return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void MainWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}
void MainWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}
void MainWindow::textStyle(int styleIndex)
{
    QTextCursor cursor = ui->textEdit->textCursor();
    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;
        switch (styleIndex) {
        default:
        case 1:
            style = QTextListFormat::ListDisc;
            break;
        case 2:
            style = QTextListFormat::ListCircle;
            break;
        case 3:
            style = QTextListFormat::ListSquare;
            break;
        case 4:
            style = QTextListFormat::ListDecimal;
            break;
        case 5:
            style = QTextListFormat::ListLowerAlpha;
            break;
        case 6:
            style = QTextListFormat::ListUpperAlpha;
            break;
        case 7:
            style = QTextListFormat::ListLowerRoman;
            break;
        case 8:
            style = QTextListFormat::ListUpperRoman;
            break;
        }
        cursor.beginEditBlock();
        QTextBlockFormat blockFmt = cursor.blockFormat();
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            //listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
        cursor.endEditBlock();
    } else {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}
