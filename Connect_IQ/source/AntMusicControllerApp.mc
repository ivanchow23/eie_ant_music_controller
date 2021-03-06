// AntMusicControllerApp.mc
// Description: This is where the application starts.
// By: Ivan Chow
// Date: February 25, 2018

using Toybox.Application as App;
using Toybox.WatchUi as Ui;

class AntMusicControllerApp extends App.AppBase {

    function initialize() {
        AppBase.initialize();
    }

    // onStart() is called on application start up
    function onStart(state) {
    }

    // onStop() is called when your application is exiting
    function onStop(state) {
    }

    // Return the initial view of your application here
    function getInitialView() {
        return [ new AntMusicControllerView(), new AntMusicControllerDelegate() ];
    }

}
