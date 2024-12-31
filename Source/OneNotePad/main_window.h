#pragma once

#include <QMainWindow>
#include <QSet>

#include "ui_main_window.h"

class QString;
class QCloseEvent;
class MainCore;

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   protected:
    void closeEvent(QCloseEvent* event) override;

   private:
    void InitUi();
    void InitValue();
    void InitConnect();

   private:
    // V
    Ui::MainWindow ui;

    // M
    MainCore* m_mainCore = nullptr;
};
