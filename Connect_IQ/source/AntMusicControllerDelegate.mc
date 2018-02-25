using Toybox.WatchUi as Ui;

class AntMusicControllerDelegate extends Ui.BehaviorDelegate {

    function initialize() {
        BehaviorDelegate.initialize();
    }

    function onMenu() {
        Ui.pushView(new Rez.Menus.MainMenu(), new AntMusicControllerMenuDelegate(), Ui.SLIDE_UP);
        return true;
    }

}