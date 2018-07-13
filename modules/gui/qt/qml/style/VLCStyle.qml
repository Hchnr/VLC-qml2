pragma Singleton
import QtQuick 2.0

Item {
    id: vlc_style
    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    property bool nightMode: false;
    function toggleNightMode() {
        if (state == "day")
            state = "night"
        else if (state == "night")
            state = "system"
        else
            state = "day"
    }

    state: "system"
    states: [
        State {
            name: "day"
            PropertyChanges {
                target: vlc_style

                bgColor: "white";
                textColor: "black";

                bgColor_removeFromPlaylist: "#CC0000";
                textColor_removeFromPlaylist: "white";

                hoverBgColor: "#F0F0F0";
                textColor_activeSource: "#FF0000";
                bannerColor: "#e6e6e6";
                hoverBannerColor: "#d6d6d6";
            }
        },
        State {
            name: "night"
            PropertyChanges {
                target: vlc_style

                bgColor: "black";
                textColor: "white";

                bgColor_removeFromPlaylist: "#CC0000";
                textColor_removeFromPlaylist: "white";

                hoverBgColor: "#F0F0F0";
                textColor_activeSource: "#FF0000";
                bannerColor: "#191919";
                hoverBannerColor: "#292929";
            }
        },
        State {
            name: "system"
            PropertyChanges {
                target: vlc_style

                bgColor: myPalette.base;
                textColor: myPalette.text;

                bgColor_removeFromPlaylist: "#CC0000";
                textColor_removeFromPlaylist: "#FFFFFF";

                hoverBgColor: myPalette.highlight;
                textColor_activeSource: myPalette.buttonText;
                bannerColor: myPalette.window;
                hoverBannerColor: myPalette.highlight;
            }
        }
    ]

    // Sizes
    property double margin_xxxsmall: 2;
    property double margin_xxsmall: 4;
    property double margin_xsmall: 8;
    property double margin_small: 12;
    property double margin_normal: 16;
    property double margin_large: 24;
    property double margin_xlarge: 32;

    property int radius_xxxsmall: 1
    property int radius_xxsmall: 2
    property int radius_xsmall: 4
    property int radius_small: 8
    property int radius_normal: 16
    property int radius_large: 32

    property int fontSize_xsmall: 8;
    property int fontSize_small: 10;
    property int fontSize_normal: 12;
    property int fontSize_large: 14;
    property int fontSize_xlarge: 16;

    property int heightAlbumCover_xsmall: 32;
    property int heightAlbumCover_small: 64;
    property int heightAlbumCover_normal: 128;
    property int heightAlbumCover_large: 255;
    property int heightAlbumCover_xlarge: 512;

    property int heightSeekBar: 10;
    property int heightSliderImplicit: 4;
    property int heightToolbar: 50;
    property int heightHandle: 16

    property int icon_xsmall: 8;
    property int icon_small: 16;
    property int icon_normal: 32;
    property int icon_large: 64;
    property int icon_xlarge: 128;

    property int cover_xsmall: 64;
    property int cover_small: 96;
    property int cover_normal: 128;
    property int cover_large: 160;
    property int cover_xlarge: 192;

    property int heightBar_xsmall: 8;
    property int heightBar_small: 16;
    property int heightBar_normal: 32;
    property int heightBar_large: 64;
    property int heightBar_xlarge: 128;
    property int heightBar_xxlarge: 256;

    property int minWidthMediacenter: 500;
    property int maxWidthPlaylist: 400;
    property int defaultWidthPlaylist: 300;
    property int closedWidthPlaylist: 20;

    property int widthVolumeSlider: 80;
    property int widthTimeLabel: 40;
    property int widthHandle: 16
    property int widthSliderImplicit: 100;

    property int widthSortBox: 150;

    //colors
    property color bgColor: myPalette.base;
    property color textColor: myPalette.text;

    property color bgColor_removeFromPlaylist: "#CC0000";
    property color textColor_removeFromPlaylist: "white";

    property color hoverBgColor: myPalette.highlight;
    property color textColor_activeSource: "#FF0000";

    property color bannerColor: myPalette.window;
    property color hoverBannerColor: myPalette.highlight;

    property color vlc_orange: "#FFFF950D";

    property color bgColorSlider: "#bdbebf";
    property color colorSlider: "#c62f2f";
    property color colorSliderHandle: "#f6f6f6";
    property color colorSliderHandleBorder: "#bdbebf";
    property color colorSliderHandlePressed: "#f0f0f0";

    //timings
    property int delayToolTipAppear: 500;
    property int timingPlaylistClose: 1000;
    property int timingPlaylistOpen: 1000;
    property int timingGridExpandOpen: 200;
    property int timingListExpandOpen: 200;

    //default arts
    property url noArtCover: "qrc:///noart.png";
}
