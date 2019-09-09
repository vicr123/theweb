TEMPLATE = subdirs

internal-pagesproj.subdir = internal-pages

browserproj.subdir = browser
browserproj.depends = internal-pagesproj

SUBDIRS += \
    browserproj \
    internal-pagesproj \
    tests
