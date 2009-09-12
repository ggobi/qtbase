## Adapted from Qt's AddressBook tutorial
library(qtbase)

qsetClass("AddressBook", Qt$QWidget,
          function(parent = NULL) {
            super(parent)
            nameLabel <- Qt$QLabel(tr("Name:"))
            this$nameLine <- Qt$QLineEdit()

            addressLabel <- Qt$QLabel(tr("Address:"))
            this$addressText <- Qt$QTextEdit()

            mainLayout <- Qt$QGridLayout()
            mainLayout$addWidget(nameLabel, 0, 0)
            mainLayout$addWidget(nameLine, 0, 1)
            mainLayout$addWidget(addressLabel, 1, 0, Qt$Qt$AlignTop)
            mainLayout$addWidget(addressText, 1, 1)

            setLayout(mainLayout)
            setWindowTitle(tr("Simple Address Book"))
          })

ab <- AddressBook()
