#include "findform.h"
#include "ui_findform.h"

FindForm::FindForm(QWidget *parent, QTextEdit* edit, bool mode) :
    QDialog(parent),
    ui(new Ui::FindForm),
    editor(edit)
{
    ui->setupUi(this);
    ui->parameter_groupBox->setVisible(0);
    ui->parameter_groupBox_2->setVisible(0);
    ui->direction_comboBox->addItem("Forward");
    ui->direction_comboBox->addItem("Backward");
    ui->direction_comboBox->addItem("Looped");
    ui->direction_comboBox_3->addItem("Forward");
    ui->direction_comboBox_3->addItem("Backward");
    ui->direction_comboBox_3->addItem("Looped");
    if(!mode)
        ui->tabWidget->setCurrentWidget(ui->tab_Find);
    else
        ui->tabWidget->setCurrentWidget(ui->tab_Replace);
    connect(editor, SIGNAL(copyAvailable(bool)), ui->replace_Button,
            SLOT(setEnabled(bool)));
    connect(editor, SIGNAL(selectionChanged()), this,
                SLOT(changeFind()));
}

void FindForm::changeFind()
{
    QString str = editor->textCursor().selectedText();
    ui->find_lineEdit->setText(str);
    ui->find_lineEdit_3->setText(str);
}

FindForm::~FindForm()
{
    delete ui;
}

void FindForm::on_find_lineEdit_textChanged(const QString &arg1)
{
    if(!(ui->find_lineEdit->text().isEmpty()))
        ui->find_Button->setEnabled(1);
    else
        ui->find_Button->setEnabled(0);
}

int FindForm::WhichMode()
{
    int result = ui->direction_comboBox->currentIndex();
    if(ui->register_checkBox->isChecked())
        result += 3;
    if(ui->word_checkBox->isChecked())
        result += 6;
    return result;
}

void FindForm::on_find_Button_clicked()
{
    QString str = ui->find_lineEdit->text();
    QTextDocument::FindFlags mode;
    bool cycle = 0;
    switch (WhichMode())
    {
    case 0: mode =
        QTextDocument::FindFlags();
        break;
    case 1: mode =
        QTextDocument::FindBackward;
        break;
    case 2: mode =
        QTextDocument::FindFlags();
        cycle = 1;
        break;
    case 3: mode =
            QTextDocument::FindFlags() | QTextDocument::FindCaseSensitively;
        break;
    case 4: mode =
            QTextDocument::FindBackward | QTextDocument::FindCaseSensitively;
        break;
    case 5: mode =
            QTextDocument::FindFlags() | QTextDocument::FindCaseSensitively;
        cycle = 1;
        break;
    case 6: mode =
            QTextDocument::FindFlags() | QTextDocument::FindWholeWords;
        break;
    case 7: mode =
            QTextDocument::FindBackward | QTextDocument::FindWholeWords;
        break;
    case 8: mode =
            QTextDocument::FindFlags() | QTextDocument::FindWholeWords;
        cycle = 1;
        break;
    case 9: mode =
            QTextDocument::FindFlags() | QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords;
        break;
    case 10: mode =
            QTextDocument::FindBackward | QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords;
        break;
    case 11: mode =
            QTextDocument::FindFlags() | QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords;
        cycle = 1;
        break;
    }
    if(cycle)
    {
        if(!(editor->find(str, mode)))
        {
            QTextCursor cur = editor->textCursor();
            cur.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor, 1);
            editor->setTextCursor(cur);
            if(!(editor->find(str, mode)))
            {
                QMessageBox::warning(this, tr("Searching"),
                                     tr("No matches found in search direction"));
                return;
            }
        }
    }
    else
    {
      if(!(editor->find(str, mode)))
        {
            QMessageBox::warning(this, tr("Searching"),
                           tr("No matches found in search direction"));
            return;
        }
    }
}


void FindForm::on_tabWidget_currentChanged(int index)
{
    ui->parameter_groupBox->setVisible(0);
    ui->parameter_groupBox_2->setVisible(0);
    ui->more_Button->setChecked(0);
    ui->more_Button_3->setChecked(0);
    on_more_Button_clicked(0);
}


void FindForm::on_find_lineEdit_editingFinished()
{
    ui->find_lineEdit_3->setText(ui->find_lineEdit->text());
}


void FindForm::on_find_lineEdit_3_editingFinished()
{
    ui->find_lineEdit->setText(ui->find_lineEdit_3->text());
}


void FindForm::on_find_lineEdit_3_textChanged(const QString &arg1)
{
    if(!(ui->find_lineEdit_3->text().isEmpty()))
        ui->find_Button_3->setEnabled(1);
    else
        ui->find_Button_3->setEnabled(0);
}


void FindForm::on_find_Button_3_clicked()
{
    on_find_Button_clicked();
}


void FindForm::on_replace_Button_clicked()
{
    QString fd = ui->find_lineEdit_3->text();
    editor->insertPlainText(ui->replace_lineEdit->text());
    ui->find_lineEdit->setText(fd);
    ui->find_lineEdit_3->setText(fd);
    on_find_Button_clicked();
}


void FindForm::on_direction_comboBox_3_currentIndexChanged(int index)
{
    ui->direction_comboBox->setCurrentIndex(index);
}


void FindForm::on_direction_comboBox_currentIndexChanged(int index)
{
    ui->direction_comboBox_3->setCurrentIndex(index);
}


void FindForm::on_more_Button_clicked(bool checked)
{
    if(!checked)
    {
        ui->direction_comboBox->setCurrentIndex(0);
        ui->direction_comboBox_3->setCurrentIndex(0);
        ui->word_checkBox->setChecked(0);
        ui->word_checkBox_3->setChecked(0);
        ui->register_checkBox->setChecked(0);
        ui->register_checkBox_3->setChecked(0);
    }
}


void FindForm::on_more_Button_3_clicked(bool checked)
{
    on_more_Button_clicked(checked);
}

