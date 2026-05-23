#ifndef STYLES_H
#define STYLES_H
#include <QString>

// referinta paleta culori
// Fundal: #0f172a (Deep Slate Blue)
// Accent Primary: #38bdf8 (Cyan Neon)
// Accent Hover: #7dd3fc (Light Cyan)
// Text: #f1f5f9
// Text Secundar: #94a3b8

// buton principal
const QString primaryBtnStyle =
    "QPushButton { "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #38bdf8, stop:1 #0ea5e9); "
    "   color: #ffffff; "
    "   border: 1px solid #7dd3fc; "
    "   border-radius: 12px; "
    "   padding: 14px 28px; "
    "   font-weight: 900; "
    "   font-size: 14px; "
    "   text-transform: uppercase; "
    "   letter-spacing: 1px; "
    "   margin-top: 5px; "
    "} "
    "QPushButton:hover { "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7dd3fc, stop:1 #38bdf8); "
    "   border: 2px solid #ffffff; "
    "   padding: 13px 27px; "
    "} "
    "QPushButton:pressed { "
    "   background-color: #0c4a6e; "
    "   border: 1px solid #0ea5e9; "
    "   margin-top: 7px; "
    "   margin-bottom: -2px; "
    "   color: #e0f2fe; "
    "}";

// butonul secundar
const QString secondaryBtnStyle =
    "QPushButton { "
    "   background-color: rgba(255, 255, 255, 0.03); "
    "   color: #7dd3fc; "
    "   border: 2px solid rgba(125, 211, 252, 0.3); "
    "   border-radius: 12px; "
    "   padding: 12px 26px; "
    "   font-weight: bold; "
    "   font-size: 13px; "
    "} "
    "QPushButton:hover { "
    "   background-color: rgba(125, 211, 252, 0.1); "
    "   border-color: rgba(125, 211, 252, 0.8); "
    "   color: #ffffff; "
    "} "
    "QPushButton:pressed { "
    "   background-color: rgba(125, 211, 252, 0.2); "
    "   border-color: #38bdf8; "
    "}";

// warning button
const QString dangerBtnStyle =
    "QPushButton { "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ef4444, stop:1 #b91c1c); "
    "   color: white; "
    "   border: 1px solid #f87171; "
    "   border-radius: 12px; "
    "   padding: 14px 28px; "
    "   font-weight: 900; "
    "   font-size: 14px; "
    "   text-transform: uppercase; "
    "   margin-top: 5px; "
    "} "
    "QPushButton:hover { "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f87171, stop:1 #ef4444); "
    "   border: 2px solid #ffffff; "
    "   padding: 13px 27px; "
    "} "
    "QPushButton:pressed { "
    "   background-color: #7f1d1d; "
    "   border: 1px solid #f87171; "
    "   margin-top: 7px; "
    "   margin-bottom: -2px; "
    "}";

// campuri text
const QString lineEditStyle =
    "QLineEdit { "
    "   padding: 8px 18px; "
    "   min-height: 22px; "
    "   border: 1px solid rgba(255, 255, 255, 0.1); "
    "   border-radius: 10px; "
    "   background-color: rgba(30, 41, 59, 0.5); " // semi-transparent
    "   color: #f1f5f9; "
    "   font-size: 14px; "
    "} "
    "QLineEdit:focus { "
    "   border: 2px solid #38bdf8; "
    "   background-color: rgba(15, 23, 42, 0.8); "
    "   padding: 7px 15px; "
    "} "
    "QLineEdit:hover { "
    "   border: 1px solid rgba(56, 189, 248, 0.5); "
    "}";

// drop down-uri si date
const QString dropDownStyle =
    "QComboBox, QDateEdit { "
    "   padding: 8px 18px; "
    "   border: 1px solid rgba(255, 255, 255, 0.1); "
    "   border-radius: 10px; "
    "   background-color: rgba(30, 41, 59, 0.5); "
    "   color: #f1f5f9; "
    "   font-size: 14px; "
    "} "
    "QComboBox:focus, QDateEdit:focus { "
    "   border: 2px solid #38bdf8; "
    "} "
    "QComboBox::drop-down, QDateEdit::drop-down { "
    "   subcontrol-origin: padding; "
    "   subcontrol-position: top right; "
    "   width: 35px; "
    "   border-left: 1px solid rgba(255, 255, 255, 0.1); "
    "   border-top-right-radius: 10px; "
    "   border-bottom-right-radius: 10px; "
    "   background-color: rgba(255, 255, 255, 0.02); "
    "} "
    "QComboBox::down-arrow, QDateEdit::down-arrow { "
    "   image: none; "
    "   border-left: 2px solid #94a3b8; "
    "   border-bottom: 2px solid #94a3b8; "
    "   width: 8px; "
    "   height: 8px; "
    "   transform: rotate(-45deg); "
    "   margin-right: 5px; "
    "   margin-top: -2px; "
    "} "
    "QComboBox QAbstractItemView { "
    "   background-color: #1e293b; "
    "   color: #f1f5f9; "
    "   border: 1px solid #38bdf8; "
    "   selection-background-color: #0ea5e9; "
    "   selection-color: white; "
    "   outline: none; "
    "   border-radius: 8px; "
    "   padding: 5px; "
    "}"
    "QComboBox QLineEdit { "
    "   background: transparent; "
    "   border: none; "
    "   padding: 0px; "
    "   color: #f1f5f9; "
    "}";

// checkbox
const QString checkBoxStyle =
    "QCheckBox { "
    "   color: #94a3b8; "
    "   font-size: 14px; "
    "   spacing: 12px; "
    "} "
    "QCheckBox::indicator { "
    "   width: 22px; "
    "   height: 22px; "
    "   border-radius: 7px; "
    "   border: 2px solid #475569; "
    "   background-color: #0f172a; "
    "} "
    "QCheckBox::indicator:hover { "
    "   border: 2px solid #7dd3fc; "
    "   background-color: rgba(56, 189, 248, 0.1); "
    "} "
    "QCheckBox::indicator:checked { "
    "   background-color: #38bdf8; "
    "   border: 2px solid #ffffff; "
    "}";

const QString globalLabelStyle =
    // fundal imagine transparenta
    "QMainWindow { "
    "   background-color: #0f172a; "
    "   background-image: url(:/img/bg.jpg); " // imaginea de background
    "   background-position: center; "
    "   background-repeat: no-repeat; "
    "} "

    "QLabel { border: none; background: transparent; color: #f1f5f9; } "

    "QLabel[heading=\"true\"] { color: #ffffff; font-weight: 900; font-size: 24px; margin-bottom: 15px; } "

    // scrollbar
    "QScrollBar:vertical { "
    "   border: none; "
    "   background-color: rgba(15, 23, 42, 0.5); "
    "   width: 6px; "
    "   margin: 0px; "
    "   border-radius: 3px; "
    "} "
    "QScrollBar::handle:vertical { "
    "   background-color: rgba(56, 189, 248, 0.3); "
    "   min-height: 40px; "
    "   border-radius: 3px; "
    "} "
    "QScrollBar::handle:vertical:hover { "
    "   background-color: #38bdf8; "
    "} "
    "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
    "   border: none; background: none; height: 0px; "
    "} "
    "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; } "

    // pop up-uri (message box)
    "QMessageBox { background-color: #1e293b; border: 2px solid #38bdf8; border-radius: 15px; } "
    "QMessageBox QLabel { color: #f1f5f9; font-size: 15px; padding: 10px; background: transparent; } "
    "QMessageBox QPushButton { "
    "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #38bdf8, stop:1 #0ea5e9); "
    "   color: white; border: none; border-radius: 8px; padding: 10px 20px; min-width: 100px; font-weight: bold; font-size: 12px; text-transform: uppercase; "
    "} "
    "QMessageBox QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7dd3fc, stop:1 #38bdf8); }";

#endif