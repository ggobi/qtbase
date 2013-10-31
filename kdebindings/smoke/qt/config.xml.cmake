<config>
    <resolveTypedefs>true</resolveTypedefs>
    <qtMode>true</qtMode>
    <generator>smoke</generator>
    <includeDirs>
<!-- NOTE: These paths are essentially useless on the Mac, because the
     Qt installer sticks everything under the default framework
     directory where our hacked generator knows to look.  The ideal
     solution would be to have a <frameworkDirs> element in this file,
     which would specify @QT_INCLUDE_DIR@ as a directory element. This
     is awaiting a use-case.  -->
@SMOKE_CONFIG_INCLUDE_DIR_ELEMENTS@
    </includeDirs>
</config>
