#ifndef FINDFORM_H
#define FINDFORM_H

#include <QDialog>
#include <QTextEdit>
#include <QMessageBox>

namespace Ui {
class FindForm;
}

class FindForm : public QDialog
{
    Q_OBJECT

public:
    explicit FindForm(QWidget *parent = nullptr, QTextEdit* edit = nullptr, bool mode = 0);
    ~FindForm();

private slots:
    void on_find_lineEdit_textChanged(const QString &arg1);

    void on_find_Button_clicked();

    void on_tabWidget_currentChanged(int index);

    void changeFind();

    void on_find_lineEdit_editingFinished();

    void on_find_lineEdit_3_editingFinished();

    void on_find_lineEdit_3_textChanged(const QString &arg1);

    void on_find_Button_3_clicked();

    void on_replace_Button_clicked();

    void on_direction_comboBox_3_currentIndexChanged(int index);

    void on_direction_comboBox_currentIndexChanged(int index);

    void on_more_Button_clicked(bool checked);

    void on_more_Button_3_clicked(bool checked);

private:
    Ui::FindForm *ui;
    QTextEdit* editor;
    int WhichMode();
};

#endif // FINDFORM_H
