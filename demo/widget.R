### From the Qt Widget tutorial
library(qtbase)

## construct a widget
window <- Qt$QWidget()

## resize and show
window$resize(320, 240)
window$show()

## now a widget that does something
button <- Qt$QPushButton("Press me", window)
button$move(100, 100)
button$show()
