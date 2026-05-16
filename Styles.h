#ifndef STYLES_H
#define STYLES_H
#include <QString>

const QString primaryBtnStyle
    = "QPushButton { background-color: #2563eb; color: white; border: none; border-radius: 6px; "
      "padding: 10px 20px; font-weight: bold; font-size: 14px; } QPushButton:hover { "
      "background-color: #60a5fa; }";
const QString secondaryBtnStyle
    = "QPushButton { background-color: transparent; color: #2563eb; border: 2px solid #2563eb; "
      "border-radius: 6px; padding: 8px 16px; font-weight: bold; font-size: 13px; } "
      "QPushButton:hover { background-color: #f8fafc; color: #1d4ed8; }";
const QString dangerBtnStyle
    = "QPushButton { background-color: #ef4444; color: white; border: none; border-radius: 6px; "
      "padding: 10px 20px; font-weight: bold; font-size: 14px; } QPushButton:hover { "
      "background-color: #fca5a5; }";
const QString lineEditStyle
    = "QLineEdit { padding: 10px; border: 1px solid #475569; border-radius: 5px; background-color: "
      "#1e293b; color: #f8fafc; font-size: 14px; } QLineEdit:focus { border: 2px solid #3b82f6; }";
const QString dropDownStyle
    = "QComboBox, QDateEdit { padding: 5px 10px; border: 1px solid #475569; border-radius: 5px; "
      "background-color: #1e293b; color: #f8fafc; font-size: 14px; }";
const QString globalLabelStyle
    = "QLabel { border: none; background: transparent; outline: none; padding: 0px; margin: 0px; }";
const QString checkBoxStyle
    = "QCheckBox { color: white; font-size: 14px; } QCheckBox::indicator { width: 18px; height: "
      "18px; border-radius: 4px; border: 2px solid #475569; background-color: #1e293b; } "
      "QCheckBox::indicator:checked { background-color: #2563eb; border: 2px solid #2563eb; }";

#endif // STYLES_H
