## Adapted from Qt's AddressBook tutorial
library(qtbase)

qsetClass("AddressBook", Qt$QWidget, function(parent = NULL) {
  super(parent)
  
  nameLabel <- Qt$QLabel("Name:")
  this$nameLine <- Qt$QLineEdit()
  nameLine$readOnly <- TRUE

  addressLabel <- Qt$QLabel("Address:")
  this$addressText <- Qt$QTextEdit()
  addressText$readOnly <- TRUE

  this$addButton <- Qt$QPushButton("&Add")
  addButton$show()
  this$submitButton <- Qt$QPushButton("&Submit")
  submitButton$hide()
  this$cancelButton <- Qt$QPushButton("&Cancel")
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
  setWindowTitle("Simple Address Book")

  this$contacts <- character()
})

qsetMethod(AddressBook, "addContact", function() {
  this$oldName <- nameLine$text
  this$oldAddress <- addressText$toPlainText()

  nameLine$clear()
  addressText$clear()

  nameLine$readOnly <- FALSE
  nameLine$setFocus()
  addressText$readOnly <- FALSE

  addButton$enabled <- FALSE
  submitButton$show()
  cancelButton$show()
})

qsetMethod(AddressBook, "submitContact", function() {
  name <- nameLine$text
  address <- addressText$toPlainText()

  if (name == "" || address == "") {
    Qt$QMessageBox$information(this, "Empty Field",
                               "Please enter a name and address.");
    return;
  }

  if (is.na(contacts[name])) {
    contacts[name] <<- address
    msg <- sprintf("\"%s\" has been added to your address book.", name)
    Qt$QMessageBox$information(this, "Add Successful", msg)
  } else {
    msg <- sprintf("Sorry, \"%s\" is already in your address book.", name)
    Qt$QMessageBox$information(this, "Add Unsuccessful", msg)
    return;
  }

  if (length(contacts)) {
    nameLine$clear()
    addressText$clear()
  }
  
  nameLine$readOnly <- TRUE
  addressText$readOnly <- TRUE
  addButton$enabled <- TRUE
  submitButton$hide()
  cancelButton$hide()
})

qsetMethod(AddressBook, "cancel", function() {
  nameLine$text <- oldName
  nameLine$readOnly <- TRUE

  addressText$plainText <- oldAddress
  addressText$readOnly <- TRUE

  addButton$enabled <- TRUE
  submitButton$hide()
  cancelButton$hide()
})

ab <- AddressBook()
ab$show()

