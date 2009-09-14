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
            addressText$readOnly <- TRUE

            this$addButton <- Qt$QPushButton(tr("&Add"))
            addButton$show()
            this$submitButton <- Qt$QPushButton(tr("&Submit"))
            submitButton$hide()
            this$cancelButton <- Qt$QPushButton(tr("&Cancel"))
            cancelButton$hide()

            qconnect(addButton, "clicked", addContact)
            qconnect(submitButton, "clicked", submitContact)
            qconnect(cancelButton, "clicked", cancel)

            buttonLayout1 <- Qt$QVBoxLayout()
            buttonLayout1$addWidget(addButton, Qt$Qt$AlignTop)
            buttonLayout1$addWidget(submitButton)
            buttonLayout1$addWidget(cancelButton)
            buttonLayout1$addStretch()
            
            mainLayout <- Qt$QGridLayout()
            mainLayout$addWidget(nameLabel, 0, 0)
            mainLayout$addWidget(nameLine, 0, 1)
            mainLayout$addWidget(addressLabel, 1, 0, Qt$Qt$AlignTop)
            mainLayout$addWidget(addressText, 1, 1)
            mainLayout$addLayout(buttonLayout1, 1, 2)
            
            setLayout(mainLayout)
            setWindowTitle(tr("Simple Address Book"))
          })

qsetMethod(AddressBook, "addContact", function() {
  print("hello world")
});

qsetMethod(AddressBook, "submitContact", function() {
  
});

qsetMethod(AddressBook, "cancel", function() {
  
});

ab <- AddressBook()
ab$show()

