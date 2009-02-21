TEMPLATE = subdirs
SUBDIRS  = htmlToXBel placesimport

# Only compile cacheinfo if we're using Qt >= 4.5
!lessThan($$[QT_MINOR_VERSION], 5) {
  SUBDIRS += cacheinfo
}
