import QtQuick 2.7

ListModel {

    ListElement {
    	letters: "Dv"
        title: "YouTube - Download Vlogs"
        description: "Download the latest vlogs using the vlogger's name."
        script: '_stack_page.create_macro("youtube_download_vlogs", "YouTube - Download Vlogs", "Download the latest vlogs.");'
    }
    ListElement {
    	letters: "Dt"
        title: "YouTube - Download Trending Videos"
        description: "Download the latest trending videos."
        script: '_stack_page.create_macro("youtube_download_trending", "YouTube - Download Trending Videos", "Download the latest trending videos.");'
    }
    ListElement {
    	letters: "Pr"
        title: "YouTube - Play Random Videos"
        description: "Play random videos"
        script: '_stack_page.create_macro("youtube_play_random", "YouTube - Play Random Videos", "Play random videos from YouTube.");'
    }
    ListElement {
    	letters: "Dc"
        title: "Vimeo - Download Videos"
        description: "Download the latest videos using the creator's id"
        script: '_stack_page.create_macro("vimeo_download_videos", "Vimeo - Download Videos", "Download the latest from a creator.");'
    }
    ListElement {
    	letters: "Pv"
        title: "Vimeo - Download Trending Videos"
        description: "Download the latest trending videos."
        script: '_stack_page.create_macro("vimeo_download_trending", "Vimeo - Download Trending Videos", "Download the latest trending videos.");'
    }
    //ListElement {
    //	letters: "Gl"
    //    title: "GMail - Login"
    //    description: "Log into your GMail account."
    //    script: '_stack_page.create_macro("gmail_login", "GMail - Login", "Log into your GMail account.");'
    //}
    //ListElement {
    //	letters: "Ol"
    //    title: "Outlook - Login"
    //    description: "Log into your Outlook account."
    //    script: '_stack_page.create_macro("outlook_login", "Outlook - Login", "Log into your Outlook account.");'
    //}
    //ListElement {
    //	letters: "Yl"
    //    title: "Yahoo - Login"
    //    description: "Log into your Yahoo account."
    //    script: '_stack_page.create_macro("yahoo_login", "Yahoo - Login", "Log into your Yahoo account.");'
    //}
    ListElement {
    	letters: "Bl"
        title: "Blank - Does Nothing"
        description: "Create your own macro from a blank slate."
        script: '_stack_page.create_macro("blank", "Blank - Does Nothing", "A blank macro which does nothing.");'
    }
}