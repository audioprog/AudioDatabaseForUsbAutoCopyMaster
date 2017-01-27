#-------------------------------------------------
#
# Project created by QtCreator 2014-06-13T19:03:02
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = AudioDatenbank2
TEMPLATE = app

unix {
LIBS += -ltag

INCLUDEPATH += /usr/include/taglib \
        /usr/include/taglib/toolkit \
        /usr/include/taglib/ape \
        /usr/include/taglib/asf \
        /usr/include/taglib/flac \
        /usr/include/taglib/it \
        /usr/include/taglib/mod \
        /usr/include/taglib/mp4 \
        /usr/include/taglib/mpc \
        /usr/include/taglib/mpeg \
        /usr/include/taglib/ogg \
        /usr/include/taglib/ogg/flac \
        /usr/include/taglib/ogg/opus \
        /usr/include/taglib/ogg/speex \
        /usr/include/taglib/ogg/vorbis \
        /usr/include/taglib/riff \
        /usr/include/taglib/riff/aiff \
        /usr/include/taglib/riff/wav \
        /usr/include/taglib/s3m \
        /usr/include/taglib/trueaudio \
        /usr/include/taglib/wavpack \
        /usr/include/taglib/xm \
        /usr/include/taglib/mpeg/id3v1 \
        /usr/include/taglib/mpeg/id3v2 \
        /usr/include/taglib/mpeg/id3v2/frames
}
win32 {
INCLUDEPATH += taglib \
	taglib/toolkit \
	taglib/ape \
	taglib/asf \
	taglib/flac \
	taglib/it \
	taglib/mod \
	taglib/mp4 \
	taglib/mpc \
	taglib/mpeg \
	taglib/ogg \
	taglib/ogg/flac \
	taglib/ogg/opus \
	taglib/ogg/speex \
	taglib/ogg/vorbis \
	taglib/riff \
	taglib/riff/aiff \
	taglib/riff/wav \
	taglib/s3m \
	taglib/trueaudio \
	taglib/wavpack \
	taglib/xm \
	taglib/mpeg/id3v1 \
	taglib/mpeg/id3v2 \
	taglib/mpeg/id3v2/frames

SOURCES += \
        taglib/audioproperties.cpp \
        taglib/fileref.cpp \
        taglib/tag.cpp \
        taglib/tagunion.cpp \
        taglib/ape/apefile.cpp \
        taglib/ape/apefooter.cpp \
        taglib/ape/apeitem.cpp \
        taglib/ape/apeproperties.cpp \
        taglib/ape/apetag.cpp \
        taglib/asf/asfattribute.cpp \
        taglib/asf/asffile.cpp \
        taglib/asf/asfpicture.cpp \
        taglib/asf/asfproperties.cpp \
        taglib/asf/asftag.cpp \
        taglib/flac/flacfile.cpp \
        taglib/flac/flacmetadatablock.cpp \
        taglib/flac/flacpicture.cpp \
        taglib/flac/flacproperties.cpp \
        taglib/flac/flacunknownmetadatablock.cpp \
        taglib/it/itfile.cpp \
        taglib/it/itproperties.cpp \
        taglib/mod/modfile.cpp \
        taglib/mod/modfilebase.cpp \
        taglib/mod/modproperties.cpp \
        taglib/mod/modtag.cpp \
        taglib/mp4/mp4atom.cpp \
        taglib/mp4/mp4coverart.cpp \
        taglib/mp4/mp4file.cpp \
        taglib/mp4/mp4item.cpp \
        taglib/mp4/mp4properties.cpp \
        taglib/mp4/mp4tag.cpp \
        taglib/mpc/mpcfile.cpp \
        taglib/mpc/mpcproperties.cpp \
        taglib/mpeg/mpegfile.cpp \
        taglib/mpeg/mpegheader.cpp \
        taglib/mpeg/mpegproperties.cpp \
        taglib/mpeg/xingheader.cpp \
        taglib/mpeg/id3v1/id3v1genres.cpp \
        taglib/mpeg/id3v1/id3v1tag.cpp \
        taglib/mpeg/id3v2/id3v2extendedheader.cpp \
        taglib/mpeg/id3v2/id3v2footer.cpp \
        taglib/mpeg/id3v2/id3v2frame.cpp \
        taglib/mpeg/id3v2/id3v2framefactory.cpp \
        taglib/mpeg/id3v2/id3v2header.cpp \
        taglib/mpeg/id3v2/id3v2synchdata.cpp \
        taglib/mpeg/id3v2/id3v2tag.cpp \
        taglib/mpeg/id3v2/frames/attachedpictureframe.cpp \
        taglib/mpeg/id3v2/frames/commentsframe.cpp \
        taglib/mpeg/id3v2/frames/generalencapsulatedobjectframe.cpp \
        taglib/mpeg/id3v2/frames/ownershipframe.cpp \
        taglib/mpeg/id3v2/frames/popularimeterframe.cpp \
        taglib/mpeg/id3v2/frames/privateframe.cpp \
        taglib/mpeg/id3v2/frames/relativevolumeframe.cpp \
        taglib/mpeg/id3v2/frames/textidentificationframe.cpp \
        taglib/mpeg/id3v2/frames/uniquefileidentifierframe.cpp \
        taglib/mpeg/id3v2/frames/unknownframe.cpp \
        taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.cpp \
        taglib/mpeg/id3v2/frames/urllinkframe.cpp \
        taglib/ogg/oggfile.cpp \
        taglib/ogg/oggpage.cpp \
        taglib/ogg/oggpageheader.cpp \
        taglib/ogg/xiphcomment.cpp \
        taglib/ogg/flac/oggflacfile.cpp \
        taglib/ogg/opus/opusfile.cpp \
        taglib/ogg/opus/opusproperties.cpp \
        taglib/ogg/speex/speexfile.cpp \
        taglib/ogg/speex/speexproperties.cpp \
        taglib/ogg/vorbis/vorbisfile.cpp \
        taglib/ogg/vorbis/vorbisproperties.cpp \
        taglib/riff/rifffile.cpp \
        taglib/riff/aiff/aifffile.cpp \
        taglib/riff/aiff/aiffproperties.cpp \
        taglib/riff/wav/infotag.cpp \
        taglib/riff/wav/wavfile.cpp \
        taglib/riff/wav/wavproperties.cpp \
        taglib/s3m/s3mfile.cpp \
        taglib/s3m/s3mproperties.cpp \
        taglib/toolkit/tbytevector.cpp \
        taglib/toolkit/tbytevectorlist.cpp \
        taglib/toolkit/tbytevectorstream.cpp \
        taglib/toolkit/tdebug.cpp \
        taglib/toolkit/tdebuglistener.cpp \
        taglib/toolkit/tfile.cpp \
        taglib/toolkit/tfilestream.cpp \
        taglib/toolkit/tiostream.cpp \
        taglib/toolkit/tlist.tcc \
        taglib/toolkit/tmap.tcc \
        taglib/toolkit/tpropertymap.cpp \
        taglib/toolkit/trefcounter.cpp \
        taglib/toolkit/tstring.cpp \
        taglib/toolkit/tstringlist.cpp \
        taglib/toolkit/unicode.cpp \
        taglib/trueaudio/trueaudiofile.cpp \
        taglib/trueaudio/trueaudioproperties.cpp \
        taglib/wavpack/wavpackfile.cpp \
        taglib/wavpack/wavpackproperties.cpp \
        taglib/xm/xmfile.cpp \
        taglib/xm/xmproperties.cpp

HEADERS  += \
        taglib/audioproperties.h \
        taglib/fileref.h \
        taglib/tag.h \
        taglib/taglib_export.h \
        taglib/tagunion.h \
        taglib/ape/apefile.h \
        taglib/ape/apefooter.h \
        taglib/ape/apeitem.h \
        taglib/ape/apeproperties.h \
        taglib/ape/apetag.h \
        taglib/asf/asfattribute.h \
        taglib/asf/asffile.h \
        taglib/asf/asfpicture.h \
        taglib/asf/asfproperties.h \
        taglib/asf/asftag.h \
        taglib/flac/flacfile.h \
        taglib/flac/flacmetadatablock.h \
        taglib/flac/flacpicture.h \
        taglib/flac/flacproperties.h \
        taglib/flac/flacunknownmetadatablock.h \
        taglib/it/itfile.h \
        taglib/it/itproperties.h \
        taglib/mod/modfile.h \
        taglib/mod/modfilebase.h \
        taglib/mod/modfileprivate.h \
        taglib/mod/modproperties.h \
        taglib/mod/modtag.h \
        taglib/mp4/mp4atom.h \
        taglib/mp4/mp4coverart.h \
        taglib/mp4/mp4file.h \
        taglib/mp4/mp4item.h \
        taglib/mp4/mp4properties.h \
        taglib/mp4/mp4tag.h \
        taglib/mpc/mpcfile.h \
        taglib/mpc/mpcproperties.h \
        taglib/mpeg/mpegfile.h \
        taglib/mpeg/mpegheader.h \
        taglib/mpeg/mpegproperties.h \
        taglib/mpeg/xingheader.h \
        taglib/mpeg/id3v1/id3v1genres.h \
        taglib/mpeg/id3v1/id3v1tag.h \
        taglib/mpeg/id3v2/id3v2extendedheader.h \
        taglib/mpeg/id3v2/id3v2footer.h \
        taglib/mpeg/id3v2/id3v2frame.h \
        taglib/mpeg/id3v2/id3v2framefactory.h \
        taglib/mpeg/id3v2/id3v2header.h \
        taglib/mpeg/id3v2/id3v2synchdata.h \
        taglib/mpeg/id3v2/id3v2tag.h \
        taglib/mpeg/id3v2/frames/attachedpictureframe.h \
        taglib/mpeg/id3v2/frames/commentsframe.h \
        taglib/mpeg/id3v2/frames/generalencapsulatedobjectframe.h \
        taglib/mpeg/id3v2/frames/ownershipframe.h \
        taglib/mpeg/id3v2/frames/popularimeterframe.h \
        taglib/mpeg/id3v2/frames/privateframe.h \
        taglib/mpeg/id3v2/frames/relativevolumeframe.h \
        taglib/mpeg/id3v2/frames/textidentificationframe.h \
        taglib/mpeg/id3v2/frames/uniquefileidentifierframe.h \
        taglib/mpeg/id3v2/frames/unknownframe.h \
        taglib/mpeg/id3v2/frames/unsynchronizedlyricsframe.h \
        taglib/mpeg/id3v2/frames/urllinkframe.h \
        taglib/ogg/oggfile.h \
        taglib/ogg/oggpage.h \
        taglib/ogg/oggpageheader.h \
        taglib/ogg/xiphcomment.h \
        taglib/ogg/flac/oggflacfile.h \
        taglib/ogg/opus/opusfile.h \
        taglib/ogg/opus/opusproperties.h \
        taglib/ogg/speex/speexfile.h \
        taglib/ogg/speex/speexproperties.h \
        taglib/ogg/vorbis/vorbisfile.h \
        taglib/ogg/vorbis/vorbisproperties.h \
        taglib/riff/rifffile.h \
        taglib/riff/aiff/aifffile.h \
        taglib/riff/aiff/aiffproperties.h \
        taglib/riff/wav/infotag.h \
        taglib/riff/wav/wavfile.h \
        taglib/riff/wav/wavproperties.h \
        taglib/s3m/s3mfile.h \
        taglib/s3m/s3mproperties.h \
        taglib/toolkit/taglib.h \
        taglib/toolkit/tbytevector.h \
        taglib/toolkit/tbytevectorlist.h \
        taglib/toolkit/tbytevectorstream.h \
        taglib/toolkit/tdebug.h \
        taglib/toolkit/tdebuglistener.h \
        taglib/toolkit/tfile.h \
        taglib/toolkit/tfilestream.h \
        taglib/toolkit/tiostream.h \
        taglib/toolkit/tlist.h \
        taglib/toolkit/tmap.h \
        taglib/toolkit/tpropertymap.h \
        taglib/toolkit/trefcounter.h \
        taglib/toolkit/tstring.h \
        taglib/toolkit/tstringlist.h \
        taglib/toolkit/tutils.h \
        taglib/toolkit/unicode.h \
        taglib/trueaudio/trueaudiofile.h \
        taglib/trueaudio/trueaudioproperties.h \
        taglib/wavpack/wavpackfile.h \
        taglib/wavpack/wavpackproperties.h \
        taglib/xm/xmfile.h \
        taglib/xm/xmproperties.h \
        taglib/taglib_config.h
}

SOURCES += main.cpp\
        mainwindow.cpp \
    Einzelbeitrag.cpp \
    foldersearcher.cpp \
    PathScannerThread.cpp \
    lsStringListWidget.cpp \
    lsKeyAndFilterListEditor.cpp \
    lsGlobalSettings.cpp \
    lsMp3Converter.cpp \
    CompletedLineEdit.cpp \
    ListSelectDialog.cpp \
    ListPrintDialog.cpp \
	PainterWidget.cpp \
    lsOutputSource.cpp \
	lsVariantList2D.cpp \
    LsFileList.cpp \
    lsSpinBox.cpp \
    lsVariantList2D.cpp

HEADERS  += mainwindow.h \
    Einzelbeitrag.h \
    foldersearcher.h \
    PathScannerThread.h \
    lsStringListWidget.h \
    lsKeyAndFilterListEditor.h \
    lsGlobalSettings.h \
    lsMp3Converter.h \
    LsFileList.h \
    CompletedLineEdit.h \
    ListSelectDialog.h \
    ListPrintDialog.h \
    PainterWidget.h \
	printInterface.h \
    lsOutputSource.h \
    lsVariantList2D.h \
    lsSpinBox.h \
    lsVariantList2D.h

FORMS    += mainwindow.ui \
    Einzelbeitrag.ui \
    lsKeyAndFilterListEditor.ui \
    lsGlobalSettings.ui \
    ListSelectDialog.ui \
    ListPrintDialog.ui

RESOURCES += \
    res.qrc


INCLUDEPATH += /usr/include/taglib
DEPENDPATH += /usr/include/taglib
