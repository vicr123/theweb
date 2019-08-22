#include "mainwindow.h"
#import <AppKit/AppKit.h>

void MainWindow::setupMacOS() {
    //Disable automatic window tabbing
    [NSWindow setAllowsAutomaticWindowTabbing:NO];
}
