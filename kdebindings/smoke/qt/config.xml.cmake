<config>
    <resolveTypedefs>true</resolveTypedefs>
    <qtMode>true</qtMode>
    <generator>smoke</generator>
    <includeDirs>
<!-- NOTE: These paths are essentially useless on the Mac, because the
     Qt installer sticks everything under the default framework
     directory where our hacked generator knows to look.  The ideal
     solution would be to have a <frameworkDirs> element in this file,
     which would specify @QT_LIBRARY_DIR@ as a directory element. This
     is awaiting a use-case. Also note that @QT_LIBRARY_DIR@ can
     sometimes be set to a compound path on the Mac. Though obviously
     a bug, this will apparently not be fixed:
     http://public.kitware.com/Bug/view.php?id=10632. As noted above,
     everything will still work. -->
        <dir>@QT_INCLUDE_DIR@</dir>
        <dir>@QT_QTCORE_INCLUDE_DIR@</dir>
        <dir>@QT_QTGUI_INCLUDE_DIR@</dir>
        <dir>@QT_QTNETWORK_INCLUDE_DIR@</dir>
        <dir>@QT_QTDBUS_INCLUDE_DIR@</dir>
        <dir>@QT_QTSQL_INCLUDE_DIR@</dir>
        <dir>@QT_QTXML_INCLUDE_DIR@</dir>
        <dir>@QT_QTXMLPATTERNS_INCLUDE_DIR@</dir>
        <dir>@QT_QTOPENGL_INCLUDE_DIR@</dir>
        <dir>@QT_QTSVG_INCLUDE_DIR@</dir>
        <dir>@QT_QTSCRIPT_INCLUDE_DIR@</dir>
        <dir>@QT_QTTEST_INCLUDE_DIR@</dir>
        <dir>@QT_QTWEBKIT_INCLUDE_DIR@</dir>
        <dir>@QT_QTMULTIMEDIA_INCLUDE_DIR@</dir>
        <dir>@QT_QTHELP_INCLUDE_DIR@</dir>
        <dir>@QT_QTDECLARATIVE_INCLUDE_DIR@</dir>
    </includeDirs>
</config>
