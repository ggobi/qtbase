## Adapted from Qt's AddressBook tutorial
library(qtbase)

qsetClass("AddressBook", Qt$QWidget,
          function(parent = NULL) {
            super(parent)
            nameLabel <- Qt$QLabel(tr("Name:"))
            this$nameLine <- Qt$QLineEdit()
            nameLine$readOnly <- TRUE

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

qsetMethod(AddressBook, "addContact", function() {
  
});

qsetMethod(AddressBook, "submitContact", function() {
  
});

qsetMethod(AddressBook, "cancel", function() {
  
});

ab <- AddressBook()
ab$show()

