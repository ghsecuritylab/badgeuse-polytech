#include "presencesdialog.h"

PresencesDialog::PresencesDialog(PresencesModel *presenceModel, CardReadersModel *cardreadersModel, StudentsModel *studentsModel, QWidget *parent, QString presenceUuid) :
    QDialog(parent),
    ui(new Ui::PresencesDialog)
{
    ui->setupUi(this);

    _presenceModel = presenceModel;
    _cardReadersModel = cardreadersModel;
    _studentsDialog = studentsModel;
    _presenceUuid = &presenceUuid;



    // Connect
    connect(this, SIGNAL(accepted()), this, SLOT(validateValues()));


    // Init cardreaders combobox
    for (QMap<QString, QVariant> item : _cardReadersModel->get()) {
        ui->cb_cardReader->addItem(item["information"].toString(), item["uuid"].toByteArray().toHex());
    }

    // Init students combobox
    for (QMap<QString, QVariant> item : _studentsDialog->get()) {
        ui->cb_student->addItem(item["text"].toString(), item["uuid"].toByteArray().toHex());
    }

    // Init dateTime
    ui->dte_entry->setDateTime(QDateTime::currentDateTime());
    ui->dte_exit->setDateTime(QDateTime::currentDateTime());



    if (!_presenceUuid->isEmpty()) {

        ui->l_title->setText("Modification d'une présence");

        QMap<QString, QVariant> presenceInfo = _presenceModel->getPresence(*_presenceUuid);

        ui->cb_cardReader->setCurrentIndex(ui->cb_cardReader->findData(presenceInfo["cardreaderUuid"].toByteArray().toHex()));
        ui->cb_student->setCurrentIndex(ui->cb_student->findData(presenceInfo["studentUuid"].toByteArray().toHex()));

        if (presenceInfo["dateTimeExit"].toUInt() == 0) {
            ui->gb_enableexit->setChecked(false);
        } else {
            QDateTime exit = QDateTime::fromTime_t(presenceInfo["dateTimeExit"].toUInt());
            exit.setTimeSpec(Qt::UTC);
            ui->dte_exit->setDateTime(exit.toLocalTime());
        }
        QDateTime entry = QDateTime::fromTime_t(presenceInfo["dateTimeEntry"].toUInt());
        entry.setTimeSpec(Qt::UTC);
        ui->dte_entry->setDateTime(entry.toLocalTime());



    } else {
        ui->l_title->setText("Ajout d'une présence");
    }

}



PresencesDialog::~PresencesDialog()
{
    delete ui;
}



void PresencesDialog::accept()
{
    if (validateValues()) {
        if (_presenceUuid->isEmpty()) {
            _presenceModel->add(
                        ui->dte_entry->dateTime(),
                        ui->dte_exit->dateTime(),
                        ui->cb_cardReader->currentData().toString(),
                        ui->cb_student->currentData().toString(),
                        !ui->gb_enableexit->isChecked()
                        );
        } else {
            _presenceModel->modify(
                        *_presenceUuid,
                        ui->dte_entry->dateTime(),
                        ui->dte_exit->dateTime(),
                        ui->cb_cardReader->currentData().toString(),
                        ui->cb_student->currentData().toString(),
                        !ui->gb_enableexit->isChecked()
                        );
        }
        QDialog::accept();
    }
}


bool PresencesDialog::validateValues()
{
    QString error = QString();

    if (ui->gb_enableexit->isChecked()) {
        if (ui->dte_entry->dateTime().toTime_t() >= ui->dte_exit->dateTime().toTime_t()) {
            error += "\n - La date de sortie doit être supérieur à la date d'entrée.";
        }
    }

    if (error.length()>0) {
        QMessageBox::critical(this, "Attention",
        "Certain champ ne sont pas correct : " + error,
        QMessageBox::Ok);
        return false;
    }

    return true;
}
