# Arora Changelog

This is the changelog of the Arora browser. Arora had a different issue log (not accessible anymore)
and it might be confusing if we used the Endorphin and Arora issue log in the changelog, so the Changelog
was moved to another file.

## 0.11.0

### Interface
- When the privacy indicator is visible clicking on it will result in private mode being turned off.
- Add a way to modify the user agent string from within the application.
- Support arguments for the external download program
- Tweak the adblock dialog spacing and size.
- Change the AdBlock dialog to use a simple check box instead of a group box.
- Replace Stopped with Download Complete (like in ffx it's less confusing )
- Update the copyright year in the about dialog to include 2010
- Issue: 768: Fix spelling mistake
- Issue: 821: Sort columns in cookie dialog by data values instead of string value. This fixes the dates being alphanumerically sorted.

### Behind the scene
- Issue: 891: Fix Text Encoding to work correctly
- Issue: 826: Correctly save the history state of a tab when using Qt 4.6
- Fix build breakage with QtWebKit 2.0.
- Remove remnant of delayed QWebView creation. Fixes tab restore again.
- Style fix: convert tabs to spaces.

#### Build system
- Add an application icon for os2.
- For finding the locale tools on os2 behave like win32
- When building in debug mode on OS2 just like with Windows enable the console.
- Move Arora's hooks to the git-hooks format to utilize git-hooks (See: http://github.com/icefox/git-hooks)
- Default to lrelease and not lrelease-qt4 on non unix platforms.
- QMAKE_EXTRA_UNIX_TARGETS is deprecated; use QMAKE_EXTRA_TARGETS instead.
- Enable the raster graphics system by default for X11

### Translation
- Make "Show Hidden Files" translatable.
- Issue: 811: Fix typos in the German translation
- Replaced "leichtgewichtiger" with "schlanker" in translation with source "Lightweight WebKit-based wen browser"
- Fixed capitalization typo whare the word "Tab" began with a lowercase "t"
- Renamed Dutch translation file from nl_BE to nl.


## 0.10.2
### The major issues
- When compiling with Qt 4.6 Arora had been using some API that changes
    shortly before the release of Qt 4.6.0 after an API review was done for
    the new API in QtWebKit.  This release changes the code to build with the
    final API that is found in Qt 4.6.0
- A segfault that could occur when opening a URL on a new window from the command-line.


## 0.10.1
### The major issues
- Issue: 666 Sites were showing a "SSL handshake failure". With the new NetworkAccessManagerProxy the signals were being sent to the proxy and not the primary networkaccessmanager.
- Issue: 683: Arora crashes when using privacy mode.
	When switching to private mode the primary networkaccessmanagers swaps cookiejar's which caused a segfault. Introduce a CookieJarProxy class which will pass the cookie requests to the primary networkaccessmanager's cookiejar.

### Translations
- Add Portuguese translation from Américo Monteiro <a_monteiro@netcabo.pt>
- Updated Turkish translation from Mehmet Nur Olcay <mnurolcay@gmail.com>
- Issue: 682,677 Fix several FR spelling mistakes.

### Fixed Issues
- Issue: 79 Only tab down the completer when the popup is visible.
- Issue: 221 When using Qt 4.6 hardcode a webpage to have a white background even when using a dark theme.
- Issue: 472 Set the url on the location bar that matches the webview that we are loading the url in, not in the current location bar which might not be the correct one.
- Issue: 637 Startup Crash with Qt 4.5.0 (not with any other version)
- Issue: 679 Selecting Show only one close button instead of one for each tab requires an application restart to be applied.
- Issue: 676 Fix openstreetmap.org's login saving and improve the Wallet password dialog when finding the login name.
	Rather than using the document.form[0].name use document.form[0].elements["name"] this allows the name to contain things such as []'s without causing a problem.

### Other issues / changes
- Add support for the home, favorites, search, and openurl multimedia keys found on some keyboards
- Turn on AutoFill by default
- Remove the accidental 100MB limit on the disk cache size in the settings dialog.
- Make source viewer remember the window size and not block access to the main window

## 0.10.0
Interface
- Issue #24: Add support for AdBlock
- Issue #40: Support several common modifiers in the location bar for modifying the typed in url.
    ctrl-enter       - append .com
    ctrl-shift-enter - append .org
    shift-enter      - append .net
- Issue #79: Trap tab key to cycle through potential URLs in the location bar.
- Issue #85: Alt+Enter on the location bar opens the url in a new tab.
- Issue #119: Add Password Management / AutoFill
- Issue #411: Save and restore the tab back/forward history
- Issue #630: Limit download progress updates to 5 per second to reduces CPU use from 18% to 3% when downloading.
- Issue #650: Save the toolbar location allowing the toolbars to be next to each other
- Add ctrl-z shortcut to undo the closing of the last tab.
- Behave better when access keys are enabled by adding a slight delay to make sure that the ctrl is actually for access keys and not for another shortcut such as ctrl-tab or ctrl-a
- When Arora crashed on the previous startup give the user a way to bypass the failsafe and restore their session if they really want.
- Add the ability to set the length of a session cookie
- Enable DNS prefetching and WebKit version in the about dialog when building against Qt 4.6.
- Remember what tab you were on when you close the settings dialog
- Add a browse button next to the style sheet line edit to help users find files and automatically translate them into url's.
- Add icons to the menu actions on freedesktop systems.
- Show the configure search engines action in the tools menu
- Change default bookmarks to only have a link to htpp://Arora-browser.org
    Removed all of Qt development type entries
    From discussion on http://Arorabrowser.blogspot.com/2009/08/Arora-090.html and also the Kubuntu guys strip the bookmarks in their release so it make sense.
- Move the Preferences menu item to Tools|Options
- Set placeholder text for the start page search box
- Add a checkbox to hide/show hidden files in the directory listing.
- Change the about dialog text so it can be selected with a mouse so users can copy the application version.
- Allow urls to be dropped on the bookmark bar and bookmark menu

### Behind the scenes
- Force the history completer to be LTR: same as the url line.
- Create a new stills class, NetworkAcessManagerProxy that is useful for tracking what QWebPage a QWebReply came from.
- Load QWebView settings when the View is created so m_enableAccessKeys will be read from settings
- Rename lineedit -> locationbar functions & variables
- Enhance SingleApplication so that the host can send messages back to the second application.
    On Windows this is used to send the window id back to the application that was just started
    so it can raise the Arora window to the front.
- Fix some old MSVC compile warnings
- Make it possible to retranslate the bookmarks toolbar title
- Set the maximumPagesInCache to 3 (default is 0) and provide a way for users to set this value through QSettings.
- Add a path from the users directory to the locale search path
- Added RTL support for the start page
- Add support for multiple directories to LanguageManager.
- Fix the naming of the QAction m_toolsEnableInspector to m_toolsEnableInspectorAction to be consistent with the rest of the code
- Add new TreeSortFilterProxyModel class to utils that will only filter child nodes in a QAbstractItemModel. (under the BSD license)
- Rename getConfigFile to dataFilePath
- Rename dataDirectory to installedDataDirectory to give a much better description of what it does
- Add missing drop functionality to the ModelMenu class. The implementation is symetric to the ModelToolBar class.
- ModelMenu can now accept drops (both moving and copying) and can apply them to the underlying model.
- Refactor BookmarksToolBar by making it inherit from ModelToolBar.
- Add new ModelToolBar class, that is a subclass of QToolBar and an equivalent of ModelMenu for toolbars. It can be used to populate a toolbar with actions based on a given model.
- Fix character encoding in directory listing.
- Clean up the private browsing message box to make it easier to translate.

#### Build system
- Allow building against a shadow-built WebKit trunk
- Ignore more misc generated build files on windows
- Add Git hook to check commits for the proper copyright year
- Ignore the doc directory which is generated by doxygen
- Check for spaces at the start and end of an expression
- Re-enable sharing the temporary compilation objects for all subprojects. Use the 'ordered' configuration to make sure that we walk through the subdirectories one by one and not all simultaneously when building in parallel.


## 0.9.0

Drop support for Qt 4.4.

### Interface
#### Features
- Add support for search keywords in the location bar.
- POST support for OpenSearch suggestion requests.
- Add WebKit version to the About dialog [WebKit trunk only].
- Use HTTP pipelining for all network requests [Qt 4.6 only].

#### Improvements
- Remember last used save directory.
- Added file dialog to download location settings.
- Restrict drag and drop to the same page unless a QWebView accepts it.
- Open an URL dropped on the tab bar as new tab or replace existing tab.
- Allow url to be dropped on the tab bar from other applications.
- Add a Select All action.
- Rather than not allowing setting or getting of any cookies in private mode create a blank cookie jar.
- Significantly speed up the cookie dialog by keeping a cached copy of all the cookies.
- Enhance the language manager to fallback to the country file if the country_language file doesn't exists.
- Add drag support for downloaded item in the download manager.
- Add Planet Qt to the default set of bookmarks.
- Remove the text wrapping option from the source viewer's menu and instead enable it by default.

### Bug fixes
- Avoid duplicated and contradictory rules on the cookie rule list.
- Change the shortcut for showing the bookmark toolbar as it was conflicting with the show bookmark dialog shortcut.
- Encode the user input before inserting it into an URL template.
- Fix a crash when cloning a blank tab.
- When the widget losses focus hide the access keys.
- Fix detection of flash on various websites when ClickToFlash is used.
- Avoid an infinite recursion when asking the desktop to open an URL.
- When the download manager prevents the application from closing add a new tab when there are none.
- Only show the url completer if the user is typing a url.
- Always add the Inspect Element Action to the context menu when the developer extra tools are enabled.
- Bookmarklets were not working properly.
- Work around for a segfault in Qt 4.5.2 where you go into privacy mode while a QNetworkReply is still working.
- Clarify the networkdisk cache behavior in the messagebox.
- Start page: Reduce the top margin to 100px from so it fits on a netbook screen.
- Hide the bookmarks toolbar by default.
- Turn off cookie tracking filtering by default because it breaks sites like StackOverflow.com.
- Before clearing the cookie jar, load it so the exceptions and other settings are loaded.
- When removing cookies from the cookie jar via the cookie dialog notify the autosaver.
- When receiving a drop event on a bookmark folder, make sure that we don't drop a child bookmark.
- Avoid overwriting the user agent string in the WebPage autotest.
- Fix non-ASCII characters displayed garbled for Authors.

### Behind the scenes
- Merge two blocks of code that deals with oneCloseButton policy.
- Make parentWindow() prettier.
- Simplify updates of actions in view menu.
- Save the window state when toggling the menu bar.
- Save UI changes in the active window before creating a new one.
- Move all network related files into one directory.

#### Build system
- When building on osx use qmake -r.
- Ignore generated files built on windows.


## 0.8.0
### Interface
#### Features
- When the ctrl key is pressed show keyboard accelerators on the screen. [Qt 4.6 only].
- Location bar: implement full text search and more accurate sorting.
- Add a simple start page with a search box pointing at the active search engine.
- Allow to create new open search engines from any input forms displayed in websites [Qt 4.6 only].
- Add a menu to set the default text encoding for all rendering [Qt 4.6 only].
- Implement directory listing when accessing file:/// URLs that point to directories.
- When a POST request is about to be resent, warn user about that fact to make sure that the operation is intended.
- Add a checkable push button to the search bar that allows to highlight a specific string in a website [Qt 4.6 only].
- Add a setting that specifies if the url supplied by the user should be forwarded to the default search engine if it isn't valid.

#### Improvements
- Show a more verbose message when the htmlToXBel tool is not installed.
- Show the complete tab title as tool tip to improve GUI usability for long website titles.
- Download manager: Create the download directory as needed.
- Location bar: If escape is pressed on location bar revert to the original URL, this emulates the firefox behaviour.
- Add Google "I'm Feeling Lucky" search engine to the default set.
- Drag & Drop: Add the ability to drop an url on the tab bar which will open it in a new tab.
- Drag & Drop: Allow dropping URLs on the location bar.
- When creating new windows, use the startup setting to decide if the homepage should be loaded.
- Remove the network monitor tool as the current Inspector now lets you see request headers and response headers.

### Bug fixes
- Fix several memory leaks: set the Qt::WA_DeleteOnClose flag on dialogs that are executed asynchronously.
- Download manager: Fix displaying file size when downloading huge files.
- Download manager: Give correct name to downloaded files with no suffix.
- Location bar: Fix background color when using Oxygen style.
- Bookmarks manager: Don't allow to edit the url in any nodes but normal bookmarks.
- Private browsing now disables the disk cache.

### Behind the scenes
- Added custom network scheme handlers, i.e. a single one for listing local directories.
- Add a setting so the user can swap the location of the new and close tab buttons.
- Add the ability for a user to specify the userAgent through QSettings.
- Split up the bookmarks classes into separate files.
- Remake the bookmarks toolbar.
- Clean up the AddBookmarkDialog class.
- OpenSearch: Implement the Referrer extension.
- Implement the WebPage::linkedResources() method using the DOM API.
- Tweak the certToFormattedString() function.
- OpenSearch: localization improvements.

#### Build system
- Include an additional XML file in the installation, which will make Arora appear in Gnome Control Center on the list of available web browsers that can be set as default.
- Add man pages for the tools.
- webkit.pri now supports building QtWebKit as a framework on mac.


## 0.7.1
### Behind the scenes
#### Build system
- Fix Windows build.
- Fix building in parallel (make -jX).


## 0.7.0
### Interface
- Add support for OpenSearch to the toolbar search
- Add the ability to search from the webpage context menu
- Remember the boxes checked in the Clear Private Data dialog
- Support Back, Forward, Reload and Stop keys on multimedia keyboards
- Add the ability to click a button before loading flash (clickToFlash)
- Allow to enable/disable the cache
- Allow to specify the maximum cache size
- Add a setting specifying if the application should quit when last tab is closed.
- Revert the check for the Oxygen style so when under KDE4 Oxygen will be
used even if it has issues.
- Fix mid click to paste urls into Arora to have them be loaded
- Fix crash in Clear Privacy Dialog when cache is disabled
- In the download manager change the "Ok" Button to "Close"
- Fix the positioning of new tab and close tab buttons. Comparing to 0.6 they are just swapped now.
- Only set a git version if the string is not empty
- Update the tab bar visibility action when retranslating.
- Make refresh be both F5 and Ctrl+R
- Update year in copyright
- Fixed some spelling mistakes
- Ctrl+L does not function when the toolbar is hidden.
- Fix a typo in Info_mac.plist.
- Enable multiple selection in the history tree view.
- Respect the setting to hide confirmation of closing multiple tabs when quitting too
- Fix a problem with cookie rule deletion
- Improve handling of unreachable sites.
- Create a new tab only when left button is double-clicked.
- Improve XBel importing
- Don't fill the history with error pages.
- Improve the site icon when dragging.  When the mouse is over it show the arrow cursor, Set the drag icon to the site icon, and set the drag text to the page title and not the url. (Example drag to the toolbar you want to get the title not the url)
- Only save the cookie exception rules if the dialog is accepted.
- Set cookies to 'session only' if new rule introduced that would only allow them for the session.
- Add ability to filter out tracking cookies, eg Google Analytics.
- Add 'Add Rule' button to the cookie dialog to create an exception on the current cookie.
- When a cookie is a session cookie show the string 'Session Cookie'
- Improve the cooking blocking implementation
- Add subdomain checking, eg when the policy says 'block def.com', then it will block cookies from 'def.com', 'www.def.com' but not from 'abcdef.com'
- Handle cookie rules with starting dot correctly

### Behind the scenes
- Add a subclass of QWebPluginFactory that can be used for managing QWebPlugin's
- Add a static BrowserMainWindow::parentWindow(QWidget *) method that returns a main window being one of the passed widget's parents.
- Move QTRY functions into their own header file as it has no dependency on Arora and can be used by tests that don't require BrowserApplication
- Mark strings as not translatable where it doesn't make sense
- Move location bar site icon class into its own file
- Move the privacy indicator out into its own class and file
- Move the location bar classes into a location bar folder
- Add api to get the clear button and search button in the SearchWidget
- Remove the word 'slot' from the functions that are slots to be consistent with the reset of the source code.
- Don't translate dummy strings in the ui file
- Move HistoryManager into its own file and move the history classes into their own folder.
- Conserve memory by atomizing history strings
- Code style: Add m_ prefix before private variables
- Improvements & cleanup to the about dialog
- Improve the three editviews
- Add manualtest for the three edit views and an autotest for the editlistview.
- Change placesimport to correctly use SingleApplication
- Add WebPage::linkedResources(const QString&) method that returns a list of resources attached to the main document
- BrowserApplication::mainWindow() returns the currently active window

#### Build system
- Remove one second punishment because it didn't work and I didn't make autotests when I was punished
- Add foreach() style error to look for and fix existing occurences.
- When building by default don't have lrelase be verbose as it du/mps a lot of junk on the console
- Add commit hook to do basic style checking on the files being commited
- unset GIT_DIR to fix warnings that are printed to the console
- When building Arora also don't allow casts to ascii to detect bugs.
- Share compiled object files with the main Arora binary to reduce build times


## 0.6.1
When using Arora with Qt 4.5.1 after a little while pages will stop rendering.
This is because of a bug in 4.5.1 where renaming a file will cause the file
descriptor to not be closed.  QNetworkDiskCache is a simple cache and uses
QTemporaryFile's for each new cached file and eventually the kernel
wont let QNetworkDiskCache open a cache file because all of the previous files were not
closed.  When running against Qt 4.5.1 Arora will now disable the disk cache.


## 0.6
### Interface
- Save more of the main window state, window's fullscreen status, maximized status, menu bar visibility, normal size, and ensure that the menu bar and status bar are returned to their proper states if saving while the window is fullscreen.
- Open a downloaded file instead of the containing directory
- Add check for the Oxygen style and switch to Plastique unless the user specifies oxygen on the command line
- The action to show the status bar isn't updated when entering/exiting fullscreen.
- slow down the loading animation to use less X11 resources
- Add Google suggestions to the search widget
- Ctrl-0 is now zoom-reset, only support jumping to the first 9 tabs rather then 10
- Add action to add a bookmark folder to the bookmark menu
- Display more loading information in the status bar.
- Make 'Url' and 'Title' string translatable in Add Bookmark dialog.
- Making SSL warnings more verbose Especially show the Certs a bit
- If there is any active download on quit, warn user and ask for confirmation.
- Add "Open in Tabs" action to all bookmark menu's.
- Add "Save all Tabs" action to the bookmark menu and the tabbar context menu.
- Support preferred web content language
- Add common location bar shortcuts such as Alt-D
- Allow to close the browser with more than one window opened, which was previously impossible (it might be useful when you restore session on startup).
- Revert to the default tab selection behavior after closing a tab to match other browsers
- Middle button role should be inverted when "Select tabs and windows as they created" is selected
- Add support for MidClick/Ctrl-Click/Shift-Ctrl-Click to all urls in Arora.  Be it in menus, toolbars, links and javascript links.
- Add support for XButton1 and XButton2 to be back/forward on the webview
- Add the ability to turn pop-up blocking off
- More control to the users! Added a possibility to control where target="_blank" links will be opened and default to a new tab.
- When opening a url from history include the old title which can populate the tab title.
- Clearing the cache should only be enabled by default when compiled with Qt 4.5
- Set bookmarks bar as default folder when executing Add Bookmark Dialog from bookmarks toolbar.
- Fixed typo that had clear cookies set checked twice instead of checking cookies once and cache once.
- Force selected window to be on top of the windows stack (previous solution was only activating the window on the taskbar, at least on some desktop environments).
- One tab close button option should not affect tab open button appearance.
- Set Open action disabled in the download manager until the download is finished.
- Add the ability to import html bookmarks using the existing htmlToXBel tool
- Fix issue where shortcuts where not working
- Fix issue where actions in the menus were not being translated when you change the language.
- Add support for caching HTTP proxies when using Qt 4.5
- Add full page zooming
- Added a new tool 'placesimport' to import Firefox 3+ history.
- Fix the download manager SqueezeLabel class so that doesn't use 100% cpu, also now under the BSD
- Tweak html error message to make more sense
- Several spelling errors in the interface and code.


### Behind the scenes
#### Upgrade the SearchLineEdit class
 - Rather then using a QMenu use QCompleter now that LineEdit can let you set the text margin and the completer popup takes up the whole width.
 - Re-code searchlineedit to just be a LineEdit subclass that has a search button and a clear button and nothing more.
 - Re-code the SearchButton to use the QCompleter, take up less width when there isn't one and use a QImage.
 - Update the toolbar search widget to use the completer rather then the old QMenu

#### Other improvementsv behind the scenes
- Significantly improve the startup time of Arora.
- Add new access functions to the DownloadItem data.
- Move the ClearButton and SearchButton class into its own file
- Fix searchbar for special chars such as '+'. This seems a bug in Qt's addQueryItem.  Workaround is to use addEncodedQueryItem.
- delete the root bookmark node on exit to not generate errors in valgrind
- fix memory leaks in the autotest to confirm that Arora's xbel doesn't have any leaks
- Only clear the cookies if we have loaded them
- In ModelMenu rather then all submenu's being QMenu let them be specified by the implementation and as the default use ModelMenu.
- Add new menumodel manualtest
- Set the default max rows of the model menu to -1 and put the special case of 7 in the history menu code
- In 4.5 and beyond store the icon database in the cache directory
- Add a few more mimetypes and match the Firefox desktop file
- Only guess a url string is for a file if it is an absolute path.
- Improve the overall design and implimentation of url loading especially when dealing with a startin
- Several improvements to SingleApplication to make it more robust.
- Improve the git hooks and add a hook to spell check commits
- Don't hard code the location of tmp as it could easily not be /tmp as it is on OS X
- Remove executable bit from html not found file
- Be more vigilant about always using encoded urls when storing them in QString

#### Build system
- Ignore generated files on Windows
- Add a common way to install binaries and install the tools
- Add commit hook commit-msg with support for aspell to check the spelling of the commit message
- In a source tarball and there is no .git hard code the version and change number so a message doesn't goto the console complaining about no git
- Windows needs to link to advapi32 for GetUserName call, so add it to LIBS.
- Don't force the binary to be re-build every time make is called because of the locale.pri
- Install tools
- Improve the build times of the manualtests by sharing object files and simplifying the includes


## 0.5
### Interface
- Add new Network Access Monitor tool that shows each network request as it passes.
- Added option for showing only one close Button
- Don't add empty tabs if closed to recently closed tabs menu.
- Support HTTP proxies which listen in ports higher than 10000
- Save more space in full screen mode - hide menu and status bars.
- Added an option in source viewer menu for wrapping code lines.
- Open pages from autocompletion list directly instead of filling the bar with their addresses.

- Don't show Open actions if the item is a Folder in the Bookmark Manager
- Allow filtering bookmarks and histories by URLs as well.
- Add drag functionality to Bookmark and History menu's
- Allow selecting multiple bookmarks when ctrl is pressed.
- Added support for renaming bookmarks through their context menus.
- Expanded bookmark menu - added Change URL and made Rename displayed also on folders.
- Allow for unlimited bookmarks in a folder on bookmark bar

- Added privacy indicator to the location bar
- Uncheck the privacy action when user aborts initial private browsing dialog.
- When enabling or disabling private browsing only the checkbox on the current window is updated

- Display missing Copy action in context menu if a link is selected.
- Make tab reload action in the context menu work
- Fix Middleclick on open tab to not have inconsistent behavior.
- On some web pages, middle-clicking a link opened it in the same window.
- Refined drag & drop handling in bookmarks toolbar. Fixed a bug, when page was assigned to inappropriate folder due to more than one folder having same name.
- Implement simple context menu in bookmarks toolbar. Let user change bookmark URL in Add Bookmark dialog.

### Behind the scenes
- Brand new more compliant, faster cookie jar
- Brand new Language Manager which selects the correct language on start and lets you choose a different language via the Help menu.
- Separate the cookie classes into separate files.
- Fix a memory leak when creating the context menu under Qt 4.5+
- LineEdit utility class is now under the BSD license
- The error web pages are now translatable.
- Encode urls rather then use toString in various places to prevent loss of url data.
- When using foreach try to use a const reference so a copy isn't made (slower) compared to the reference (faster).
- Add a new tool to convert bookmarks that are in the html format into the XBel format (not used in Arora yet).
- Various code style and general keeping the code clean fixes
- Add Implementation to get the current username on Windows for the single application

#### Build system
- Add script to build OS X package using the new macdeployqt tool in Qt 4.5
- Building with QT_STRICT_ITERATORS to catch errors.
- Enhance support for building with multiple WebKit trunk branches
- Adds the ability to generate code documentation with doxygen

## 0.4
### Interface
- New source viewer with search and syntax highlighting.
- Make middle click on bookmarks menus open the user in a new tab.
- Added the ability to change the language from the Help menu.
- Double left click location bar selects all of the text.
- Incorporate Jens Explorer style into Arora on Window for a better look and feel on XP and Vista. http://labs.trolltech.com/blogs/2007/06/08/explorer-style-toolbars/
- Change the "icon" on the next/prev search for RTL desktops: next points to the right, and previous points to the right
- Various RightToLeft fixes for for Mozilla's bug https://bugzilla.mozilla.org/show_bug.cgi?id=219070
- When the url scheme is https make the location bar background turn yellow.
- When the history is cleared also clear the recently closed tabs list
- Always populate the location bar with the completer
- Only shrink the toolbar icons on OS X.
- Only offset the search widget text when using Qt 4.5
- When clicking on a bookmark in a folder in the toolbar emit the proper signals to the url is opened.
- When dropping text on the webpage load the url
- When the location bar has focus don't change the text.

### Behind the scenes
- Make a dedicated SingleApplication class
- Tweak HistoryManager API to be easier to read.
- Improvements to the code style to be more consistent
- Move the binaries for autotests and manual tests into the .gitignore in that directory rather then all in one file at the top.
- Update manualtest for the location bar now that the urllineedit class has become the new location bar class and add a new git hook to make sure manual tests build.

### Build system
- Move locale build instructions to its own pri file in the locale directory
- Correctly detect qt3's qmake on Debian
- Enhance support for building with WebKit trunk


## 0.3
### Interface
- With Qt 4.5 use movable tabs
- With Qt 4.5 use disk cache
- Created a new LineEdit that handles adding side widgets in a clean, simple and elegant way.
- Add a way to set the minimum font size in the settings.
- When you ctrl-click on a bookmark item in the bookmark toolbar open it in a new tab
- Fix possible crash on startup; Initialize htmls resource
- When making the text larger or smaller, use predefined zoom levels.
- Mouse button 4 and 5 connected to action back and forward. like firefox and ie
- Clear history: Added confirm dialog
- Shortcuts in the Edit menu like in Firefox.
- Save the state of inspector across sessions.
- When only a hostname and port are entered in the url prepend http://
- Base the width of the tab on a font rather then a hard coded value
- Tweak the main window look better on OS X and less out of place
- Add support for javascript bookmarklets
- Add option to automatically restore the session on startup.
- Fix segfault caused by the clear private data dialog.
- Save the location of the toolbars
- Improve a number of the strings (feedback from MentalMaelstrom)
- Tweak action accelerators (the same ones were being used)

### Build system
- Allow building Arora with both Debug/Release of WebKit trunk
- Only include the sha1 and change number in the version when using git.
- Create utils directory to contain non-Arora specific classes
- Run desktop file through desktop-file-validate and correct the errors

#### Autotests
- Add QTRY_COMPARE and QTRY_VERIFY for the autotests to use.
- add AUTOTESTS define when running autotests to prevent accessing running Arora's and sending url's to it
- Fix autotest failures (test related, not Arora)
- Detect which version of qmake to use when building the autotests
- Give a 1 second punishment if there is no autotest for the source file that was changed
- Emit a warning of a file doesn't have a matching autotest
- Update modeltest code to match Trolltech's svn


## 0.2
 - Many Bug fixes and improvements.
 - Add make install on unix
 - Improved tabs
 - Translations
 - New reset dialog
 - New about dialog
 - Add search banner
 - Rename to Arora and a new application icon
 - Project specific git hooks
 - Branch from the Qt demo browser with new autotests and manualtests
