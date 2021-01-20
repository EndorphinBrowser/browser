## Changelog

## 0.13.0

The first official Endorphin release. Contains many new features, a new engine, and bug fixes.
**Not released yet.**

### Fixed issues
- endorphin-placesimport sometimes resulted in an "Invalid cross-device link" error.

### Behind the scene
- Port to CMake
- Update the Engine from WebKit to WebEngine

### Other
- Remove ClickToFlash: Flash is no longer supported
- Prepare for Qt6

## 0.12.1

This release contained some small changes to 0.12.0.

### Interface
- Add note about this being an inofficial fork
- Update included user agents
- Improve placeholder text of the searchbox on the front page

### Fixed issues
     Fix language fallback

## 0.12.0

An update that contains all changes that were made to Arora after 0.11.0, and QT5 support.

### Interface
- Show the number of downloads in the download manager dialog title
    
### Fixed issues
- When the 'delete' button is pressed, the downloaded item count was not updated.
- Issue: 912: Autofill manager was always storing forms.
- Issue: 915: Fixed ssl certificate problem with sites like https://webmail.us.army.mil:
        By default QSSLConfiguration was only accepting SSLv3 while we want to
        accept all of them.
- Issue: 918: "Autofill webforms" in Preferences was always checked.
- Issue: 931: Fix for fullscreen on Mac
- Issue: 957: Workaround for a bug in QCompleter
    
### Behind the scene
- Support persistent data storage (HTML5 localStorage)
- Update to QT5 & latest WebKit
- Update some internal pages to HTML5
- Code cleanup

### Other
- Remove OS2 support